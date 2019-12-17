#include "r5_audio_decoder_factory.h"
#include <gtest/gtest.h>
#include <api/scoped_refptr.h>
#include <rtc_base/ref_counted_object.h>
#include <vector>

TEST(R5AudioDecoderFactory, CreateAudioCodecOpus) {
    r5::R5AudioDecoderFactory factory;
    auto codec = factory.CreateAudioCodec(r5::AUDIO_CODEC_OPUS);
    EXPECT_NE(nullptr, codec);

    std::vector<webrtc::AudioCodecSpec> specs = codec->GetSupportedDecoders();
    EXPECT_EQ(1, specs.size());
    EXPECT_EQ("opus", specs[0].format.name);
}

TEST(R5AudioDecoderFactory, CreateAudioCodecNone) {
    r5::R5AudioDecoderFactory factory;
    auto codec = factory.CreateAudioCodec(r5::AUDIO_CODEC_NONE);
    EXPECT_NE(nullptr, codec);

    std::vector<webrtc::AudioCodecSpec> specs = codec->GetSupportedDecoders();
    EXPECT_EQ(0, specs.size());
}

TEST(R5AudioDecoderFactory, CreateAudioCodecUnsupported) {
    r5::R5AudioDecoderFactory factory;
    auto codec = factory.CreateAudioCodec(static_cast<r5::AudioCodec>(-1));
    EXPECT_EQ(nullptr, codec);
}
