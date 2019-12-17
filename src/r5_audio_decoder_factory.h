#pragma once

#include <api/audio_codecs/audio_decoder_factory.h>
#include <api/scoped_refptr.h>

namespace r5 {

enum AudioCodec {
    AUDIO_CODEC_NONE,
    AUDIO_CODEC_OPUS,
};

class R5AudioDecoderFactoryInterface {
public:
    virtual rtc::scoped_refptr<webrtc::AudioDecoderFactory> CreateAudioCodec(AudioCodec codec) = 0;
    virtual ~R5AudioDecoderFactoryInterface() {}
};

class R5AudioDecoderFactory : public R5AudioDecoderFactoryInterface {
public:
    R5AudioDecoderFactory();
    ~R5AudioDecoderFactory();

    rtc::scoped_refptr<webrtc::AudioDecoderFactory> CreateAudioCodec(AudioCodec codec);
};

}
