#pragma once

enum R5RtpPaylodType {
    RTP_PAYLOD_TYPE_NONE = -1,
    RTP_PAYLOD_TYPE_VP8 = 96,
    RTP_PAYLOD_TYPE_VP9_P0 = 98,    // profile-id=0
    RTP_PAYLOD_TYPE_VP9_P2 = 100,   // profile-id=2
    RTP_PAYLOD_TYPE_OPUS = 111,
    RTP_PAYLOD_TYPE_RED = 127,
};
