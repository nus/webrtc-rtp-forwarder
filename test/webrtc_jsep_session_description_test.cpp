#include <gtest/gtest.h>
#include <pc/webrtc_sdp.h>
#include <api/jsep_session_description.h>
#include <pc/session_description.h>
#include "log.h"

TEST(Try, JsepSessionDescription) {
    // webrtc::JsepSessionDescription s(webrtc::SdpType::kOffer);
    auto jsep_desc = std::make_unique<webrtc::JsepSessionDescription>(webrtc::SdpType::kOffer);
    webrtc::SdpDeserialize("v=0\r\no=- 6624391104903821994 2 IN IP4 127.0.0.1\r\ns=-\r\nt=0 0\r\na=group:BUNDLE 0 1\r\na=msid-semantic: WMS\r\nm=audio 61686 UDP/TLS/RTP/SAVPF 111 110\r\nc=IN IP4 240.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=candidate:3439111480 1 udp 2122262783 2408:210:28ab:7a00:ac9b:7b8d:cf96:b417 49509 typ host generation 0 network-id 2 network-cost 50\r\na=candidate:1226325006 1 udp 2122197247 2408:210:28ab:7a00:9950:1c6c:70a5:f7f0 49510 typ host generation 0 network-id 4 network-cost 50\r\na=candidate:1266632889 1 udp 2122129151 192.168.10.100 51965 typ host generation 0 network-id 1 network-cost 50\r\na=candidate:2090973323 1 udp 2122063615 192.168.10.105 61686 typ host generation 0 network-id 3 network-cost 50\r\na=candidate:2955708184 1 udp 1685855999 240.0.0.0 61686 typ srflx raddr 192.168.10.105 rport 61686 generation 0 network-id 3 network-cost 50\r\na=candidate:2280814378 1 udp 1685921535 240.0.0.0 51965 typ srflx raddr 192.168.10.100 rport 51965 generation 0 network-id 1 network-cost 50\r\na=candidate:2189382088 1 tcp 1518283007 2408:210:28ab:7a00:ac9b:7b8d:cf96:b417 51737 typ host tcptype passive generation 0 network-id 2 network-cost 50\r\na=candidate:127647998 1 tcp 1518217471 2408:210:28ab:7a00:9950:1c6c:70a5:f7f0 51738 typ host tcptype passive generation 0 network-id 4 network-cost 50\r\na=candidate:100447305 1 tcp 1518149375 192.168.10.100 51739 typ host tcptype passive generation 0 network-id 1 network-cost 50\r\na=candidate:841106555 1 tcp 1518083839 192.168.10.105 51740 typ host tcptype passive generation 0 network-id 3 network-cost 50\r\na=ice-ufrag:UKNJ\r\na=ice-pwd:0MqQyYjuTe0haXQF8lObOtDf\r\na=ice-options:trickle\r\na=fingerprint:sha-256 03:A7:44:E2:7C:2A:8B:E8:0F:7C:F4:1A:4B:49:84:AA:2C:C0:D7:4C:B4:5E:3B:EB:F8:CC:FA:F3:9B:1A:64:E8\r\na=setup:actpass\r\na=mid:0\r\na=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\na=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:5 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\r\na=extmap:6 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\r\na=recvonly\r\na=rtcp-mux\r\na=rtpmap:111 opus/48000/2\r\na=rtcp-fb:111 transport-cc\r\na=fmtp:111 minptime=10;useinbandfec=1\r\na=rtpmap:110 telephone-event/48000\r\nm=video 63138 UDP/TLS/RTP/SAVPF 96 97 98 99 100 101 127 124 125\r\nc=IN IP4 240.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=candidate:3439111480 1 udp 2122262783 2408:210:28ab:7a00:ac9b:7b8d:cf96:b417 61687 typ host generation 0 network-id 2 network-cost 50\r\na=candidate:1226325006 1 udp 2122197247 2408:210:28ab:7a00:9950:1c6c:70a5:f7f0 61688 typ host generation 0 network-id 4 network-cost 50\r\na=candidate:1266632889 1 udp 2122129151 192.168.10.100 54070 typ host generation 0 network-id 1 network-cost 50\r\na=candidate:2090973323 1 udp 2122063615 192.168.10.105 63138 typ host generation 0 network-id 3 network-cost 50\r\na=candidate:2955708184 1 udp 1685855999 240.0.0.0 63138 typ srflx raddr 192.168.10.105 rport 63138 generation 0 network-id 3 network-cost 50\r\na=candidate:2280814378 1 udp 1685921535 240.0.0.0 54070 typ srflx raddr 192.168.10.100 rport 54070 generation 0 network-id 1 network-cost 50\r\na=candidate:2189382088 1 tcp 1518283007 2408:210:28ab:7a00:ac9b:7b8d:cf96:b417 51741 typ host tcptype passive generation 0 network-id 2 network-cost 50\r\na=candidate:127647998 1 tcp 1518217471 2408:210:28ab:7a00:9950:1c6c:70a5:f7f0 51742 typ host tcptype passive generation 0 network-id 4 network-cost 50\r\na=candidate:100447305 1 tcp 1518149375 192.168.10.100 51743 typ host tcptype passive generation 0 network-id 1 network-cost 50\r\na=candidate:841106555 1 tcp 1518083839 192.168.10.105 51744 typ host tcptype passive generation 0 network-id 3 network-cost 50\r\na=ice-ufrag:UKNJ\r\na=ice-pwd:0MqQyYjuTe0haXQF8lObOtDf\r\na=ice-options:trickle\r\na=fingerprint:sha-256 03:A7:44:E2:7C:2A:8B:E8:0F:7C:F4:1A:4B:49:84:AA:2C:C0:D7:4C:B4:5E:3B:EB:F8:CC:FA:F3:9B:1A:64:E8\r\na=setup:actpass\r\na=mid:1\r\na=extmap:14 urn:ietf:params:rtp-hdrext:toffset\r\na=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:13 urn:3gpp:video-orientation\r\na=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:12 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\r\na=extmap:11 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type\r\na=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing\r\na=extmap:8 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07\r\na=extmap:9 http://www.webrtc.org/experiments/rtp-hdrext/color-space\r\na=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:5 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\r\na=extmap:6 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\r\na=recvonly\r\na=rtcp-mux\r\na=rtcp-rsize\r\na=rtpmap:96 VP8/90000\r\na=rtcp-fb:96 goog-remb\r\na=rtcp-fb:96 transport-cc\r\na=rtcp-fb:96 ccm fir\r\na=rtcp-fb:96 nack\r\na=rtcp-fb:96 nack pli\r\na=rtpmap:97 rtx/90000\r\na=fmtp:97 apt=96\r\na=rtpmap:98 VP9/90000\r\na=rtcp-fb:98 goog-remb\r\na=rtcp-fb:98 transport-cc\r\na=rtcp-fb:98 ccm fir\r\na=rtcp-fb:98 nack\r\na=rtcp-fb:98 nack pli\r\na=fmtp:98 profile-id=0\r\na=rtpmap:99 rtx/90000\r\na=fmtp:99 apt=98\r\na=rtpmap:100 VP9/90000\r\na=rtcp-fb:100 goog-remb\r\na=rtcp-fb:100 transport-cc\r\na=rtcp-fb:100 ccm fir\r\na=rtcp-fb:100 nack\r\na=rtcp-fb:100 nack pli\r\na=fmtp:100 profile-id=2\r\na=rtpmap:101 rtx/90000\r\na=fmtp:101 apt=100\r\na=rtpmap:127 red/90000\r\na=rtpmap:124 rtx/90000\r\na=fmtp:124 apt=127\r\na=rtpmap:125 ulpfec/90000\r\n", jsep_desc.get(), nullptr);
    std::unique_ptr<cricket::SessionDescription> d = jsep_desc->description()->Clone();
    for (auto c : d->contents()) {
        LOG_DEBUG("c.name: %s", c.name.c_str());
        if (c.description->as_audio()) {
            LOG_DEBUG("audio");
            for (auto codec: c.description->as_audio()->codecs()) {
                LOG_DEBUG("condec.name %s", codec.name.c_str());
            }
        }
        auto v = c.description->as_video();
        if (v) {
            LOG_DEBUG("video");
            auto codecs = v->codecs();
            codecs.erase(codecs.begin() + 8);
            codecs.erase(codecs.begin() + 7);
            codecs.erase(codecs.begin() + 6);
            codecs.erase(codecs.begin() + 5);
            codecs.erase(codecs.begin() + 4);
            codecs.erase(codecs.begin() + 3);
            codecs.erase(codecs.begin() + 2);
            v->set_codecs(codecs);
            for (auto codec: v->codecs()) {
                LOG_DEBUG("condec.name %s", codec.name.c_str());
            }
        }
    }

    webrtc::JsepSessionDescription a(webrtc::SdpType::kOffer);
    a.Initialize(std::move(d), "", "");
    LOG_DEBUG("%s", webrtc::SdpSerialize(a).c_str());
}