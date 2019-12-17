#include "r5_call_data_create_peer.h"
#include <string>
#include "mocks.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace r5;
using ::testing::_;
using ::testing::Return;
using ::testing::ByMove;
using ::testing::Invoke;


typedef ::r5::R5GrpcServiceProxy<r5::CreatePeerResponse> CreatePeerProxy;

namespace {

class DummyProxy : public CreatePeerProxy {
public:
    DummyProxy()
        : CreatePeerProxy(nullptr, nullptr) {
    }

    void RequestCreatePeer(grpc::ServerContext *context,
                           CreatePeerRequest *request,
                           grpc::ServerAsyncResponseWriter<CreatePeerResponse> *responder,
                           void *tag) override {
    }

    void Finish(grpc::ServerAsyncResponseWriter<r5::CreatePeerResponse> *responder, const r5::CreatePeerResponse& msg, const grpc::Status& status, void* tag) override {
        response = msg;
    }

    void FinishWithError(grpc::ServerAsyncResponseWriter<r5::CreatePeerResponse> *responder, const grpc::Status& s, void* tag) override {
        status = s;
    }

    r5::CreatePeerResponse response;
    grpc::Status status;
};

}

TEST(CallDataCreatePeer, Proceed) {
    DummyPeer *peer = new DummyPeer;
    EXPECT_CALL(*peer, CreateOffer(_))
        .WillOnce(Invoke(peer, &DummyPeer::InvokeCreateOffer));
    std::unique_ptr<r5::R5PeerInterface> peer_ptr(peer);

    DummyPeerFactory peer_factory;
    EXPECT_CALL(peer_factory, CreatePeer(_))
        .WillOnce(Return(ByMove(std::move(peer_ptr))));
    R5PeerContainer peer_container;
    DummyProxy dummy_proxy;

    auto cd = new CallDataCreatePeer(static_cast<CreatePeerProxy*>(&dummy_proxy), &peer_factory, &peer_container);
    cd->Proceed();

    EXPECT_NE(nullptr, peer->sdp_observer);

    std::string sdp = "v=0\r\ns=test sdp\r\n";
    const char *rtp_sdp = "SDP:\r\n"
              "v=0\r\n"
              "o=- 0 0 IN IP6 ::1\r\n"
              "s=WebRTC-RTP Forwarder\r\n"
              "c=IN IP6 ::1\r\n"
              "t=0 0\r\n"
              "m=video 30002 RTP/AVP 96\r\n"
              "a=rtpmap:96 VP8/90000\r\n";
    peer->sdp_observer->OnOfferSdpSuccess(sdp, std::string(rtp_sdp));
    EXPECT_EQ(dummy_proxy.status.error_code(), grpc::StatusCode::OK);
    EXPECT_EQ(sdp, dummy_proxy.response.offer_sdp());
    EXPECT_EQ(std::string(rtp_sdp), dummy_proxy.response.rtp_sdp());

    std::string peer_id = dummy_proxy.response.peer_id();
    EXPECT_NE(nullptr, peer_container.Get(peer_id));
}

TEST(CallDataCreatePeer, FailedResponse1) {
    DummyPeer *peer = new DummyPeer;
    EXPECT_CALL(*peer, CreateOffer(_))
        .WillOnce(Invoke(peer, &DummyPeer::InvokeCreateOffer));
    std::unique_ptr<r5::R5PeerInterface> peer_ptr(peer);

    DummyPeerFactory peer_factory;
    EXPECT_CALL(peer_factory, CreatePeer(_))
        .WillOnce(Return(ByMove(std::move(peer_ptr))));
    R5PeerContainer peer_container;
    DummyProxy dummy_proxy;

    auto cd = new CallDataCreatePeer(static_cast<CreatePeerProxy*>(&dummy_proxy), &peer_factory, &peer_container);
    cd->Proceed();

    EXPECT_NE(nullptr, peer->sdp_observer);

    peer->sdp_observer->OnOfferSdpFailure();
    EXPECT_EQ(dummy_proxy.status.error_code(), grpc::StatusCode::INTERNAL);

    delete peer;
}

TEST(CallDataCreatePeer, FailedResponse2) {
    DummyPeerFactory peer_factory;
    EXPECT_CALL(peer_factory, CreatePeer(_))
        .WillOnce(Return(ByMove(nullptr)));
    R5PeerContainer peer_container;
    DummyProxy dummy_proxy;

    auto cd = new CallDataCreatePeer(static_cast<CreatePeerProxy*>(&dummy_proxy), &peer_factory, &peer_container);
    cd->Proceed();

    EXPECT_EQ(dummy_proxy.status.error_code(), grpc::StatusCode::RESOURCE_EXHAUSTED);
}

