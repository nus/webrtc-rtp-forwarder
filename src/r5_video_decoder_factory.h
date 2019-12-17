#pragma once

#include <api/video_codecs/video_decoder_factory.h>

namespace r5 {

enum VideoCodec {
    VIDEO_CODEC_NONE,
    VIDEO_CODEC_VP8,
    VIDEO_CODEC_VP9,
};

class R5VideoDecoderFactoryInterface {
public:
    virtual std::unique_ptr<webrtc::VideoDecoderFactory> CreateVideoCodec(VideoCodec codec) = 0;
    virtual ~R5VideoDecoderFactoryInterface() {}
};

class R5VideoDecoderFactory : public R5VideoDecoderFactoryInterface {
public:
    R5VideoDecoderFactory();
    ~R5VideoDecoderFactory();
    std::unique_ptr<webrtc::VideoDecoderFactory> CreateVideoCodec(VideoCodec codec) override;
};

}
