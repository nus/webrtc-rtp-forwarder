#pragma once

#include "r5_peer_interface.h"
#include <string>
#include <map>
#include <mutex>
#include <memory>

namespace r5 {

class R5PeerContainer {
public:
    R5PeerContainer();
    ~R5PeerContainer();

    void Add(const std::string &peer_id, std::unique_ptr<R5PeerInterface> peer);
    R5PeerInterface *Get(const std::string &peer_id);
    bool Has(const std::string &peer_id);
    void Remove(const std::string &peer_id);

private:
    std::map<std::string, std::unique_ptr<R5PeerInterface>> peers;
    std::recursive_mutex peers_mutex;
};

}
