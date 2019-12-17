#include "r5_video_decoder_factory.h"
#include <gtest/gtest.h>
#include <api/video_codecs/sdp_video_format.h>

TEST(R5VideoDecoderFactory, CreateVideoCodecVp8) {
    r5::R5VideoDecoderFactory factory;
    std::unique_ptr<webrtc::VideoDecoderFactory> codec = std::move(factory.CreateVideoCodec(r5::VIDEO_CODEC_VP8));
    EXPECT_NE(nullptr, codec);

    std::vector<webrtc::SdpVideoFormat> specs = codec->GetSupportedFormats();
    EXPECT_EQ(1, specs.size());
    EXPECT_EQ("VP8", specs[0].name);
}

TEST(R5VideoDecoderFactory, CreateVideoCodecVp9) {
    r5::R5VideoDecoderFactory factory;
    std::unique_ptr<webrtc::VideoDecoderFactory> codec = std::move(factory.CreateVideoCodec(r5::VIDEO_CODEC_VP9));
    EXPECT_NE(nullptr, codec);

    std::vector<webrtc::SdpVideoFormat> specs = codec->GetSupportedFormats();
    EXPECT_EQ(1, specs.size());
    EXPECT_EQ("VP9", specs[0].name);
}

TEST(R5VideoDecoderFactory, CreateVideoCodecNone) {
    r5::R5VideoDecoderFactory factory;
    auto codec = factory.CreateVideoCodec(r5::VIDEO_CODEC_NONE);
    EXPECT_NE(nullptr, codec);

    std::vector<webrtc::SdpVideoFormat> specs = codec->GetSupportedFormats();
    // EXPECT_EQ(0, specs.size());
}
