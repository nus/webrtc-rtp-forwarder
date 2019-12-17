#pragma once

#include <string>

namespace r5 {

class R5SdpObserverInterface {
public:
    virtual void OnOfferSdpSuccess(std::string offer_sdp, std::string rtp_sdp) = 0;
    virtual void OnOfferSdpFailure() = 0;
    ~R5SdpObserverInterface() {}
};

}
