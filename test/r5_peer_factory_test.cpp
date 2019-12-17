#include "mocks.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rtc_base/ref_counted_object.h>
#include "peer.h"

using ::testing::_;
using ::testing::Invoke;

TEST(R5PeerFactory, CreatePeer) {
    rtc::scoped_refptr<DummyPeerConnectionFactory> pc_factory(
        new rtc::RefCountedObject<DummyPeerConnectionFactory>());

    EXPECT_CALL(*pc_factory.get(), CreatePeerConnection(_, _, _, _))
        .WillOnce(Invoke(pc_factory.get(), &DummyPeerConnectionFactory::InvokeCreatePeerConnection));
    r5::R5PeerFactory factory(pc_factory);

    r5::R5PeerParameter param;
    param.audio = true;
    param.audio_rtp_host = "127.0.0.1";
    param.audio_rtp_port = 1;
    param.video = true;
    param.video_rtp_host = "127.0.0.1";
    param.video_rtp_port = 2;
    EXPECT_NE(nullptr, factory.CreatePeer(param));
}

TEST(R5PeerFactory, CreatePeerDisableAudio) {
    rtc::scoped_refptr<DummyPeerConnectionFactory> pc_factory(
        new rtc::RefCountedObject<DummyPeerConnectionFactory>());

    EXPECT_CALL(*pc_factory.get(), CreatePeerConnection(_, _, _, _))
        .WillOnce(Invoke(pc_factory.get(), &DummyPeerConnectionFactory::InvokeCreatePeerConnection));
    r5::R5PeerFactory factory(pc_factory);

    r5::R5PeerParameter param;
    param.audio = false;
    param.video = true;
    param.video_rtp_host = "127.0.0.1";
    param.video_rtp_port = 2;
    EXPECT_NE(nullptr, factory.CreatePeer(param));
}
