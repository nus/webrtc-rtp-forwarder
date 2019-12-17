#include "r5_video_decoder_factory.h"
#include <absl/strings/match.h>
#include <api/video_codecs/sdp_video_format.h>
#include <media/base/media_constants.h>
#include <media/engine/internal_decoder_factory.h>
#include <modules/video_coding/codecs/vp8/include/vp8.h>
#include <modules/video_coding/codecs/vp9/include/vp9.h>
#include "log.h"

namespace {

class R5VideoCodecFactory : public webrtc::VideoDecoderFactory {
public:
    R5VideoCodecFactory(r5::VideoCodec codec_)
        : codec(codec_) {
    }
    ~R5VideoCodecFactory() {}

    std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override {
        std::vector<webrtc::SdpVideoFormat> formats;

        switch (codec) {
        case r5::VIDEO_CODEC_VP8:
            formats.push_back(webrtc::SdpVideoFormat(cricket::kVp8CodecName));
            break;
        case r5::VIDEO_CODEC_VP9:
            formats.push_back(webrtc::SdpVideoFormat(cricket::kVp9CodecName));
            break;
        default:
            break;
        }
        return formats;
    }

    std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(const webrtc::SdpVideoFormat& format) override {
        if (absl::EqualsIgnoreCase(format.name, cricket::kVp8CodecName)) {
            LOG_DEBUG("cricket::kVp8CodecName");
            return webrtc::VP8Decoder::Create();
        } else if (absl::EqualsIgnoreCase(format.name, cricket::kVp9CodecName)) {
            LOG_DEBUG("cricket::kVp9CodecName");
            return webrtc::VP9Decoder::Create();
        } else {
            LOG_ERROR("format %s", format.name.c_str());
            return nullptr;
        }
    }

private:
    const r5::VideoCodec codec;
};

}

namespace r5 {

R5VideoDecoderFactory::R5VideoDecoderFactory() {
}

R5VideoDecoderFactory::~R5VideoDecoderFactory() {
}

std::unique_ptr<webrtc::VideoDecoderFactory> R5VideoDecoderFactory::CreateVideoCodec(VideoCodec codec) {
    return std::make_unique<R5VideoCodecFactory>(codec);
}

}

