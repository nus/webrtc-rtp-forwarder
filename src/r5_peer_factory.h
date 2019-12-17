#pragma once

#include "r5_peer_interface.h"
#include "r5_peer_parameter.h"
#include <api/create_peerconnection_factory.h>
#include <api/audio_codecs/audio_decoder_factory.h>
#include <api/video_codecs/video_decoder_factory.h>
#include "r5_audio_decoder_factory.h"
#include "r5_video_decoder_factory.h"
#include "r5_rtp_packet_sender_factory.h"
#include <memory>
#include <string>

namespace r5 {

class R5PeerFactoryInterface {
public:
    virtual std::unique_ptr<R5PeerInterface> CreatePeer(const R5PeerParameter &) = 0;
    virtual ~R5PeerFactoryInterface() {}
};

class R5PeerFactory : public R5PeerFactoryInterface {
public:
    R5PeerFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = nullptr,
                  std::unique_ptr<r5::R5AudioDecoderFactoryInterface> r5_audio_decoder_factory = nullptr,
                  std::unique_ptr<r5::R5VideoDecoderFactoryInterface> r5_video_decoder_factory = nullptr,
                  std::unique_ptr<r5::R5RtpPacketSenderFactoryInterface> r5_rtp_packet_sender_factory = nullptr);

    // R5PeerFactoryInterface
    std::unique_ptr<R5PeerInterface> CreatePeer(const R5PeerParameter &parameter) override;

private:
    std::unique_ptr<rtc::Thread> signaling_thread;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
    std::unique_ptr<r5::R5AudioDecoderFactoryInterface> r5_audio_decoder_factory;
    std::unique_ptr<r5::R5VideoDecoderFactoryInterface> r5_video_decoder_factory;
    std::unique_ptr<r5::R5RtpPacketSenderFactoryInterface> r5_rtp_packet_sender_factory;
};

}


