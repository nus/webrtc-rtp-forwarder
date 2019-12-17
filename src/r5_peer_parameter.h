#pragma once

#include <string>

namespace r5 {

class R5PeerParameter {
public:
    bool video = false;
    bool audio = false;
    std::string audio_rtp_host = "";
    uint32_t audio_rtp_port = 0;
    std::string video_rtp_host = "";
    uint32_t video_rtp_port = 0;
    std::string ice_server_uri = "";
};

}
