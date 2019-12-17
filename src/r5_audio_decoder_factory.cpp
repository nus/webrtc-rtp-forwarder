#include "r5_audio_decoder_factory.h"
#include <api/audio_codecs/audio_decoder_factory_template.h>
#include <api/audio_codecs/opus/audio_decoder_opus.h>
#include "log.h"


namespace {

struct AudioDecoderEmpty {
    using Config = webrtc::AudioDecoderOpus::Config;

    static absl::optional<Config> SdpToConfig(
            const webrtc::SdpAudioFormat& audio_format) {
        return webrtc::AudioDecoderOpus::SdpToConfig(audio_format);
    }
    static void AppendSupportedDecoders(std::vector<webrtc::AudioCodecSpec>* specs) {
    }
    static std::unique_ptr<webrtc::AudioDecoder> MakeAudioDecoder(
            const Config& config,
            absl::optional<webrtc::AudioCodecPairId> codec_pair_id = absl::nullopt) {
        return webrtc::AudioDecoderOpus::MakeAudioDecoder(config, codec_pair_id);
    }
};

}

namespace r5 {

R5AudioDecoderFactory::R5AudioDecoderFactory() {
}

R5AudioDecoderFactory::~R5AudioDecoderFactory() {
}

rtc::scoped_refptr<webrtc::AudioDecoderFactory> R5AudioDecoderFactory::CreateAudioCodec(AudioCodec codec) {
    switch (codec) {
    case AUDIO_CODEC_NONE:
        return webrtc::CreateAudioDecoderFactory<AudioDecoderEmpty>();
    case AUDIO_CODEC_OPUS:
        return webrtc::CreateAudioDecoderFactory<webrtc::AudioDecoderOpus>();
    default:
        LOG_ERROR("CreateAudioCodec() Unexpected AudioCodec(%d)", codec);
        return nullptr;
    }
}

}
