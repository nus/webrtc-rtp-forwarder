#include "r5_call_data_destroy_peer.h"
#include "r5_peer_container.h"
#include <gtest/gtest.h>
#include <string>
#include "mocks.h"

using namespace r5;

typedef ::r5::R5GrpcServiceProxy<r5::DestroyPeerResponse> DestroyPeerProxy;

namespace {

class DummyProxy : public DestroyPeerProxy {
public:
    DummyProxy(const std::string &peer_id_)
        : DestroyPeerProxy(nullptr, nullptr)
        , peer_id(peer_id_) {
    }

    void RequestDestroyPeer(grpc::ServerContext *context,
                           DestroyPeerRequest *request,
                           grpc::ServerAsyncResponseWriter<DestroyPeerResponse> *responder,
                           void *tag) override {
        request->set_peer_id(peer_id);
    }

    void Finish(grpc::ServerAsyncResponseWriter<r5::DestroyPeerResponse> *responder, const r5::DestroyPeerResponse& msg, const grpc::Status& status, void* tag) override {
        response = msg;
    }

    void FinishWithError(grpc::ServerAsyncResponseWriter<r5::DestroyPeerResponse> *responder, const grpc::Status& s, void* tag) override {
        status = s;
    }

    std::string peer_id;
    r5::DestroyPeerResponse response;
    grpc::Status status;
};

}

TEST(CallDataDestroyPeer, Proceed) {
    R5PeerContainer peer_container;

    const std::string peer_id1 = "peer_id1";
    std::unique_ptr<DummyPeer> dummy_peer(new DummyPeer());
    EXPECT_CALL(*dummy_peer.get(), Close());
    DummyProxy dummy_proxy(peer_id1);

    peer_container.Add(peer_id1, std::move(dummy_peer));

    auto cd = new CallDataDestroyPeer((DestroyPeerProxy*)&dummy_proxy, &peer_container);
    cd->Proceed();

    EXPECT_EQ(dummy_proxy.status.error_code(), grpc::StatusCode::OK);
    EXPECT_FALSE(peer_container.Has(peer_id1));
}

TEST(CallDataDestroyPeer, FailedResponse) {
    R5PeerContainer peer_container;

    const std::string peer_id1 = "peer_id1";
    DummyProxy dummy_proxy(peer_id1);

    auto cd = new CallDataDestroyPeer(static_cast<DestroyPeerProxy*>(&dummy_proxy), &peer_container);
    cd->Proceed();

    EXPECT_EQ(dummy_proxy.status.error_code(), grpc::StatusCode::INVALID_ARGUMENT);
}
