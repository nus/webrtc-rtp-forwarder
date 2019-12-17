#include "r5_peer_container.h"
#include "r5_peer_interface.h"
#include "mocks.h"

#include <gtest/gtest.h>


using namespace std;


TEST(R5PeerContainer, GetAndHas) {
    r5::R5PeerContainer peers;

    DummyPeer *peer_ptr = new DummyPeer;
    std::unique_ptr<DummyPeer> peer(peer_ptr);
    peers.Add("peer_id1", move(peer));
    EXPECT_EQ(peers.Get("peer_id1"), peer_ptr);
    EXPECT_TRUE(peers.Has("peer_id1"));
}

TEST(R5PeerContainer, GetEmpty) {
    r5::R5PeerContainer peers;
    r5::R5PeerInterface *peer;

    peer = peers.Get("peer_id1");
    EXPECT_EQ(nullptr, peer);
    EXPECT_FALSE(peers.Has("peer_id1"));
}
