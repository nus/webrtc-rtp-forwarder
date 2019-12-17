#pragma once

#include "r5_sdp_observer_interface.h"
#include <string>

namespace r5 {

class R5PeerInterface {
public:
    virtual bool CreateOffer(R5SdpObserverInterface *sdp_observer) = 0;
    virtual void SetRemoteDescription(const std::string &sdp) = 0;
    virtual void AddIceCandidate(const std::string &ice) = 0;
    virtual void Close() = 0;
    virtual ~R5PeerInterface() {}
};

}
