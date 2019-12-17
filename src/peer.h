#pragma once

#include "r5_peer_interface.h"
#include "r5_peer_parameter.h"
#include "r5_rtp_packet_sender.h"
#include "r5_rtp_packet_sender_factory.h"
#include <api/peer_connection_interface.h>
#include <call/call.h>

namespace r5 {

class ICreateSdpObserverCallback {
public:
    virtual void OnCreateSdpSuccess(webrtc::SessionDescriptionInterface* desc) = 0;
    virtual void OnCreateSdpFailure(webrtc::RTCError error) = 0;
    virtual void OnCreateSdpFailure(const std::string& error) = 0;
    ~ICreateSdpObserverCallback() {}
};

class CreateSdpObserverProxy : public webrtc::CreateSessionDescriptionObserver {
public:
    ICreateSdpObserverCallback *callback;

    CreateSdpObserverProxy(ICreateSdpObserverCallback *callback_);
    ~CreateSdpObserverProxy();

    // webrtc::CreateSessionDescriptionObserver
    void OnSuccess(webrtc::SessionDescriptionInterface* desc);
    void OnFailure(webrtc::RTCError error);
    void OnFailure(const std::string& error);
};

class ISetSdpObserverCallback {
public:
    virtual void OnSetSdpSuccess() = 0;
    virtual void OnSetSdpFailure(webrtc::RTCError error) = 0;
    virtual void OnSetSdpFailure(const std::string& error) = 0;

    ~ISetSdpObserverCallback() {}
};

class SetSdpObserverProxy : public webrtc::SetSessionDescriptionObserver {
public:
    SetSdpObserverProxy(ISetSdpObserverCallback *callback_);
    ~SetSdpObserverProxy();

    void OnSuccess();
    void OnFailure(webrtc::RTCError error);
    void OnFailure(const std::string& error);

    ISetSdpObserverCallback *callback;
};

class Peer : public r5::R5PeerInterface, public webrtc::PeerConnectionObserver, public ICreateSdpObserverCallback, public ISetSdpObserverCallback, public webrtc::RtpPacketCallback {
private:
    Peer(const R5PeerParameter &parameter,
         rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory,
         R5RtpPacketSenderFactoryInterface *r5_rtp_packet_sender_factory);

public:
    ~Peer();

    static void Initialize();
    static void Finalize();
    static std::unique_ptr<R5PeerInterface> Create(const R5PeerParameter &parameter,
                                                   rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory,
                                                   R5RtpPacketSenderFactoryInterface *r5_rtp_packet_sender_factory);

public:

    // R5PeerInterface
    bool CreateOffer(R5SdpObserverInterface *sdp_observer) override;
    void SetRemoteDescription(const std::string &sdp) override;
    void AddIceCandidate(const std::string &ice) override;
    void Close() override;

    // webrtc::PeerConnectionObserver
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
    void OnRenegotiationNeeded() override;
    void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;

    // ICreateSdpObserverCallback
    void OnCreateSdpSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnCreateSdpFailure(webrtc::RTCError error) override;
    void OnCreateSdpFailure(const std::string& error) override;

    // ISetSdpObserverCallback
    void OnSetSdpSuccess() override;
    void OnSetSdpFailure(webrtc::RTCError error) override;
    void OnSetSdpFailure(const std::string& error) override;

    // webrtc::RtpPacketCallback
    void OnRtpPacket(const webrtc::RtpPacketReceived &packet) override;

private:

    std::unique_ptr<rtc::Thread> rtp_thread;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
    rtc::scoped_refptr<webrtc::CreateSessionDescriptionObserver> create_sdp_observer;
    rtc::scoped_refptr<webrtc::SetSessionDescriptionObserver> set_sdp_observer;
    R5RtpPacketSenderFactoryInterface *r5_rtp_packet_sender_factory;
    std::unique_ptr<R5RtpPacketSenderInterface> rtp_sender;
    R5PeerParameter peer_parameter;

    R5SdpObserverInterface *r5_sdp_obeserver;
    std::string offer_sdp;
};



}
