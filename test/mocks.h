#include "r5_peer_interface.h"
#include "r5_peer_factory.h"
#include <gmock/gmock.h>
#include <string>

class DummyPeer : public r5::R5PeerInterface {
public:
    MOCK_METHOD(bool, CreateOffer, (r5::R5SdpObserverInterface *sdp_observer), (override));
    MOCK_METHOD(void, SetRemoteDescription, (const std::string &sdp), (override));
    MOCK_METHOD(void, AddIceCandidate, (const std::string &ice), (override));
    MOCK_METHOD(void, Close, (), (override));

    bool InvokeCreateOffer(r5::R5SdpObserverInterface *so) {
        sdp_observer = so;
        return true;
    }
    r5::R5SdpObserverInterface *sdp_observer;
};

class DummyPeerFactory : public r5::R5PeerFactoryInterface {
public:
    MOCK_METHOD(std::unique_ptr<r5::R5PeerInterface>, CreatePeer, (const r5::R5PeerParameter &parameter), (override));
};

class DummyPeerConnectionFactory : public webrtc::PeerConnectionFactoryInterface {
public:
    MOCK_METHOD(void, SetOptions, (const webrtc::PeerConnectionFactoryInterface::Options& options), (override));
    MOCK_METHOD(rtc::scoped_refptr<webrtc::PeerConnectionInterface>, CreatePeerConnection, (
        const webrtc::PeerConnectionInterface::RTCConfiguration& configuration,
        std::unique_ptr<cricket::PortAllocator> allocator,
        std::unique_ptr<rtc::RTCCertificateGeneratorInterface> cert_generator,
        webrtc::PeerConnectionObserver* observer), (override));
    MOCK_METHOD(rtc::scoped_refptr<webrtc::MediaStreamInterface>, CreateLocalMediaStream, (const std::string& stream_id), (override));
    MOCK_METHOD(rtc::scoped_refptr<webrtc::AudioSourceInterface>, CreateAudioSource, (const cricket::AudioOptions& options), (override));
    MOCK_METHOD(rtc::scoped_refptr<webrtc::VideoTrackInterface>, CreateVideoTrack, (const std::string& label, webrtc::VideoTrackSourceInterface* source), (override));
    MOCK_METHOD(rtc::scoped_refptr<webrtc::AudioTrackInterface>, CreateAudioTrack, (const std::string& label, webrtc::AudioSourceInterface* source), (override));
    MOCK_METHOD(void, StopAecDump, (), (override));

    rtc::scoped_refptr<webrtc::PeerConnectionInterface> InvokeCreatePeerConnection(
        const webrtc::PeerConnectionInterface::RTCConfiguration& configuration,
        std::unique_ptr<cricket::PortAllocator> allocator,
        std::unique_ptr<rtc::RTCCertificateGeneratorInterface> cert_generator,
        webrtc::PeerConnectionObserver* observer) {
        return nullptr;
    }
};
