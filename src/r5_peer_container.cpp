#include "r5_peer_container.h"

namespace r5 {

R5PeerContainer::R5PeerContainer() {
}

R5PeerContainer::~R5PeerContainer() {
}

void R5PeerContainer::Add(const std::string &peer_id, std::unique_ptr<R5PeerInterface> peer) {
    std::lock_guard<std::recursive_mutex> lock(peers_mutex);
    peers[peer_id] = std::move(peer);
}

R5PeerInterface *R5PeerContainer::Get(const std::string &peer_id) {
    std::lock_guard<std::recursive_mutex> lock(peers_mutex);
    if (Has(peer_id)) {
        return peers[peer_id].get();
    } else {
        return nullptr;
    }
}

bool R5PeerContainer::Has(const std::string &peer_id) {
    std::lock_guard<std::recursive_mutex> lock(peers_mutex);
    return peers.find(peer_id) != peers.end();
}

void R5PeerContainer::Remove(const std::string &peer_id) {
    std::lock_guard<std::recursive_mutex> lock(peers_mutex);
    peers.erase(peer_id);
}

}
