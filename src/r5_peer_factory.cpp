#include "r5_peer_factory.h"
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <modules/audio_device/include/fake_audio_device.h>

#include "peer.h"
#include "log.h"

namespace r5 {

R5PeerFactory::R5PeerFactory(rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory,
                             std::unique_ptr<r5::R5AudioDecoderFactoryInterface> r5_audio_decoder_factory_,
                             std::unique_ptr<r5::R5VideoDecoderFactoryInterface> r5_video_decoder_factory_,
                             std::unique_ptr<r5::R5RtpPacketSenderFactoryInterface> r5_rtp_packet_sender_factory_)
    : peer_connection_factory(factory)
    , r5_audio_decoder_factory(std::move(r5_audio_decoder_factory_))
    , r5_video_decoder_factory(std::move(r5_video_decoder_factory_))
    , r5_rtp_packet_sender_factory(std::move(r5_rtp_packet_sender_factory_)) {
    if (!r5_audio_decoder_factory) {
        r5_audio_decoder_factory = std::make_unique<R5AudioDecoderFactory>();
    }
    if (!r5_video_decoder_factory) {
        r5_video_decoder_factory = std::make_unique<R5VideoDecoderFactory>();
    }
    if (!r5_rtp_packet_sender_factory) {
        r5_rtp_packet_sender_factory = std::make_unique<R5RtpPacketSenderFactory>();
    }
    signaling_thread = rtc::Thread::Create();
    signaling_thread->Start();
}

std::unique_ptr<R5PeerInterface> R5PeerFactory::CreatePeer(const R5PeerParameter &parameter) {
    rtc::scoped_refptr<webrtc::AudioDecoderFactory> adf;
    std::unique_ptr<webrtc::VideoDecoderFactory> vdf;
    adf = r5_audio_decoder_factory->CreateAudioCodec(
        parameter.audio ? AUDIO_CODEC_OPUS : AUDIO_CODEC_NONE);
    vdf = r5_video_decoder_factory->CreateVideoCodec(
        parameter.video ? VIDEO_CODEC_VP8 : VIDEO_CODEC_NONE);

    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;
    if (peer_connection_factory) {
        factory = peer_connection_factory;
    } else {
        factory = webrtc::CreatePeerConnectionFactory(
            nullptr /* network_thread */,
            nullptr /* worker_thread */,
            signaling_thread.get(),
            new rtc::RefCountedObject<webrtc::FakeAudioDeviceModule>(),
            webrtc::CreateBuiltinAudioEncoderFactory(),
            adf,
            webrtc::CreateBuiltinVideoEncoderFactory(),
            std::move(vdf),
            nullptr /* audio_mixer */,
            nullptr /* audio_processing */);
    }
    if (!factory) {
        LOG_ERROR("webrtc::CreatePeerConnectionFactory() failed.");
        return nullptr;
    }

    auto peer = Peer::Create(parameter, factory, r5_rtp_packet_sender_factory.get());
    if (!peer) {
        LOG_ERROR("Peer::Create() failed.");
        return nullptr;
    }
    return peer;
}

}
