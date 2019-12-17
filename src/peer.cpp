#include "peer.h"

#include <rtc_base/ssl_adapter.h>
#include <pc/peer_connection.h>
#include <api/peer_connection_proxy.h>
#include <pc/peer_connection_wrapper.h>
#include <new>

#include "log.h"

namespace {

class DummySetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver {
public:
    static DummySetSessionDescriptionObserver* Create() {
        return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
    }
    virtual void OnSuccess() {}
    virtual void OnFailure(webrtc::RTCError error) {
        LOG_DEBUG("DummySetSessionDescriptionObserver::OnFailure() %s: %s", ToString(error.type()), error.message());
    }
};

}

namespace r5 {

CreateSdpObserverProxy::CreateSdpObserverProxy(ICreateSdpObserverCallback *callback_)
    : callback(callback_) {
}

CreateSdpObserverProxy::~CreateSdpObserverProxy() {
    callback = nullptr;
}

// webrtc::CreateSessionDescriptionObserver
void CreateSdpObserverProxy::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
    LOG_DEBUG("OnSuccess");
    callback->OnCreateSdpSuccess(desc);
}

void CreateSdpObserverProxy::OnFailure(webrtc::RTCError error) {
    LOG_DEBUG("OnFailure");
    callback->OnCreateSdpFailure(std::move(error));
}

void CreateSdpObserverProxy::OnFailure(const std::string& error) {
    LOG_DEBUG("OnFailure");
    callback->OnCreateSdpFailure(error);
}

SetSdpObserverProxy::SetSdpObserverProxy(ISetSdpObserverCallback *callback_)
    : callback(callback_) {
}

SetSdpObserverProxy::~SetSdpObserverProxy() {
    callback = nullptr;
}

void SetSdpObserverProxy::OnSuccess() {
    callback->OnSetSdpSuccess();
}

void SetSdpObserverProxy::OnFailure(webrtc::RTCError error) {
    callback->OnSetSdpFailure(std::move(error));
}

void SetSdpObserverProxy::OnFailure(const std::string& error) {
    callback->OnSetSdpFailure(error);
}

Peer::Peer(const R5PeerParameter &parameter,
           rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory,
           R5RtpPacketSenderFactoryInterface *r5_rtp_packet_sender_factory_)
    : peer_connection(nullptr)
    , rtp_thread(rtc::Thread::CreateWithSocketServer())
    , r5_sdp_obeserver(nullptr)
    , peer_parameter(parameter)
    , r5_rtp_packet_sender_factory(r5_rtp_packet_sender_factory_) {
    rtp_thread->Start();

    webrtc::PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    config.enable_dtls_srtp = true;

    webrtc::PeerConnectionInterface::IceServer server;
    server.uri = parameter.ice_server_uri;
    config.servers.push_back(server);

    peer_connection = peer_connection_factory->CreatePeerConnection(config, nullptr, nullptr, this);
    if (!peer_connection) {
        LOG_ERROR("peer_connection_factory->CreatePeerConnection() failed.");
    }

    auto* pci = static_cast<webrtc::PeerConnectionProxyWithInternal<webrtc::PeerConnectionInterface>*>(
            peer_connection.get());
    if (pci) {
        auto pc = static_cast<webrtc::PeerConnection*>(pci->internal());
        webrtc::Call *call = pc->CallPtr();
        call->SetRtpPacketCallback(this);

        rtp_thread->PostTask(RTC_FROM_HERE, [this] {
            rtp_sender = std::move(r5_rtp_packet_sender_factory->CreateRtpPacketSender(
                rtp_thread.get(),
                "127.0.0.1", // TODO: Set same as the gRPC server address.
                0));
            if (!rtp_sender) {
                LOG_ERROR("r5_rtp_packet_sender_factory->CreateRtpPacketSender() failed.");
                return;
            }
            rtp_sender->SetAudioPayloadType(
                peer_parameter.audio ? RTP_PAYLOD_TYPE_OPUS : RTP_PAYLOD_TYPE_NONE,
                peer_parameter.audio_rtp_host,
                peer_parameter.audio_rtp_port);
            rtp_sender->SetVideoPayloadType(
                peer_parameter.video ? RTP_PAYLOD_TYPE_VP8 : RTP_PAYLOD_TYPE_NONE,
                peer_parameter.video_rtp_host,
                peer_parameter.video_rtp_port);
        });
    }
}

Peer::~Peer() {}

void Peer::Initialize() {
    rtc::InitializeSSL();
}

void Peer::Finalize() {
    rtc::CleanupSSL();
}

std::unique_ptr<R5PeerInterface> Peer::Create(const R5PeerParameter &parameter,
                                              rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory,
                                              R5RtpPacketSenderFactoryInterface *r5_rtp_packet_sender_factory) {
    if (parameter.audio) {
        if (parameter.audio_rtp_host.size() == 0) {
            return nullptr;
        } else if (parameter.audio_rtp_port == 0) {
            return nullptr;
        }
    }
    if (parameter.video) {
        if (parameter.video_rtp_host.size() == 0) {
            return nullptr;
        } else if (parameter.video_rtp_port == 0) {
            return nullptr;
        }
    }

    try {
        return std::unique_ptr<R5PeerInterface>(new Peer(parameter,
                                                         peer_connection_factory,
                                                         r5_rtp_packet_sender_factory));
    } catch (std::bad_alloc &e) {
        LOG_DEBUG("new Peer() failed. %s", e.what());
        return nullptr;
    } catch (std::exception &e) {
        LOG_DEBUG("new Peer() failed. %s", e.what());
        return nullptr;
    }
}

// R5PeerInterface
bool Peer::CreateOffer(R5SdpObserverInterface *sdp_observer) {
    if (!peer_connection) {
        LOG_ERROR("peer_connection must be not null on Peer::CreateOffer.");
        return false;
    }
    r5_sdp_obeserver = sdp_observer;

    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offer_options;
    offer_options.offer_to_receive_audio = peer_parameter.audio ? 1 : 0;
    offer_options.offer_to_receive_video = peer_parameter.video ? 1 : 0;

    create_sdp_observer = new rtc::RefCountedObject<CreateSdpObserverProxy>(this);
    peer_connection->CreateOffer(create_sdp_observer, offer_options);
    return true;
}

void Peer::SetRemoteDescription(const std::string &sdp) {
    LOG_DEBUG("Peer::SetRemoteDescription");
    std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
        webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp);
    peer_connection->SetRemoteDescription(
        DummySetSessionDescriptionObserver::Create(), session_description.release());
}

void Peer::AddIceCandidate(const std::string &ice) {
}

void Peer::Close() {
    if (peer_connection) {
        peer_connection->Close();
    }
}

// webrtc::PeerConnectionObserver
void Peer::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) {
    LOG_DEBUG("OnSignalingChange %d", new_state);
}

void Peer::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) {
    LOG_DEBUG("OnDataChannel");
}

void Peer::OnRenegotiationNeeded() {
    LOG_DEBUG("OnRenegotiationNeeded");
}

void Peer::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state) {
    LOG_DEBUG("OnConnectionChange %d", new_state);
}

void Peer::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) {
    LOG_DEBUG("OnIceGatheringChange: %d", new_state);

    if (new_state != webrtc::PeerConnectionInterface::kIceGatheringComplete) {
        return;
    }

    auto desc = peer_connection->local_description();
    if (desc) {
        desc->ToString(&offer_sdp);
        LOG_DEBUG("OfferSdp is updated: %s", offer_sdp.c_str());
        if (r5_sdp_obeserver) {
            std::stringstream rtp_sdp;
            rtp_sdp << "SDP:\r\n"
                "v=0\r\n"
                "o=- 0 0 IN IP6 ::1\r\n"
                "s=WebRTC-RTP Forwarder\r\n"
                "c=IN IP6 ::1\r\n"
                "t=0 0\r\n";
            if (peer_parameter.audio) {
                // TODO Set payload type.
                rtp_sdp << "m=audio " << peer_parameter.audio_rtp_port << " RTP/AVP 111\r\n"
                        << "a=rtpmap:111 OPUS/90000\r\n";
            }
            if (peer_parameter.video) {
                // TODO Set payload type.
                rtp_sdp << "m=video " << peer_parameter.video_rtp_port << " RTP/AVP 96\r\n"
                        << "a=rtpmap:96 VP8/90000\r\n";
            }
            std::string s = rtp_sdp.str();
            LOG_DEBUG("rtp_sdp: %s", s.c_str());
            r5_sdp_obeserver->OnOfferSdpSuccess(offer_sdp, std::move(s));
            r5_sdp_obeserver = nullptr;
        }
    } else {
        LOG_DEBUG("OfferSdp is not updated.");
    }
}

void Peer::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
    std::string out;
    candidate->ToString(&out);
    LOG_DEBUG("OnIceCandidate: %s", out.c_str());
}

void Peer::OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) {
    LOG_DEBUG("Peer::OnTrack %s", (*transceiver->mid()).c_str());
}


// ICreateSdpObserverCallback
void Peer::OnCreateSdpSuccess(webrtc::SessionDescriptionInterface* desc) {
    LOG_DEBUG("OnSdpSuccess");

    set_sdp_observer = new rtc::RefCountedObject<SetSdpObserverProxy>(this);
    peer_connection->SetLocalDescription(set_sdp_observer, desc);

    // TODO Remove unnecessary codecs in desc.

    create_sdp_observer = nullptr;
}

void Peer::OnCreateSdpFailure(webrtc::RTCError error) {
    LOG_DEBUG("OnSdpFailure");

    if (r5_sdp_obeserver) {
        r5_sdp_obeserver->OnOfferSdpFailure();
        r5_sdp_obeserver = nullptr;
    }

    create_sdp_observer = nullptr;
}

void Peer::OnCreateSdpFailure(const std::string& error) {
    LOG_DEBUG("OnSdpFailure");

    if (r5_sdp_obeserver) {
        r5_sdp_obeserver->OnOfferSdpFailure();
        r5_sdp_obeserver = nullptr;
    }

    create_sdp_observer = nullptr;
}

// webrtc::RtpPacketCallback
void Peer::OnRtpPacket(const webrtc::RtpPacketReceived &packet) {
    if (rtp_sender) {
        rtp_thread->PostTask(RTC_FROM_HERE, [this, packet] {
            rtp_sender->SendProcess(packet);
        });
    }
}

// ISetSdpObserverCallback
void Peer::OnSetSdpSuccess() {
    LOG_DEBUG("OnSetSdpSuccess");

    set_sdp_observer = nullptr;
};

void Peer::OnSetSdpFailure(webrtc::RTCError error) {
    LOG_DEBUG("OnSetSdpFailure");

    if (r5_sdp_obeserver) {
        r5_sdp_obeserver->OnOfferSdpFailure();
        r5_sdp_obeserver = nullptr;
    }

    set_sdp_observer = nullptr;
}

void Peer::OnSetSdpFailure(const std::string& error) {
    LOG_DEBUG("OnSetSdpFailure");

    if (r5_sdp_obeserver) {
        r5_sdp_obeserver->OnOfferSdpFailure();
        r5_sdp_obeserver = nullptr;
    }

    set_sdp_observer = nullptr;
}

}
