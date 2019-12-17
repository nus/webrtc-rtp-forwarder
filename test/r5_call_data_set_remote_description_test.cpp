#include "r5_call_data_set_remote_description.h"
#include <gtest/gtest.h>
#include <string>
#include "mocks.h"

using namespace r5;
using ::testing::_;

typedef ::r5::R5GrpcServiceProxy<r5::SetRemoteDescriptionResponse> SetRemoteDescriptionProxy;

namespace {

class DummyProxy : public SetRemoteDescriptionProxy {
public:
    DummyProxy(const std::string &peer_id_, const std::string &sdp_)
        : SetRemoteDescriptionProxy(nullptr, nullptr)
        , peer_id(peer_id_)
        , sdp(sdp_) {
    }

    void RequestSetRemoteDescription(grpc::ServerContext *context,
                           SetRemoteDescriptionRequest *request,
                           grpc::ServerAsyncResponseWriter<SetRemoteDescriptionResponse> *responder,
                           void *tag) override {
        request->set_peer_id(peer_id);
        request->set_answer_sdp(sdp);
    }

    void Finish(grpc::ServerAsyncResponseWriter<r5::SetRemoteDescriptionResponse> *responder, const r5::SetRemoteDescriptionResponse& msg, const grpc::Status& status, void* tag) override {
        response = msg;
    }

    void FinishWithError(grpc::ServerAsyncResponseWriter<r5::SetRemoteDescriptionResponse> *responder, const grpc::Status& s, void* tag) override {
        status = s;
    }

    std::string peer_id;
    std::string sdp;
    r5::SetRemoteDescriptionResponse response;
    grpc::Status status;
};

}

TEST(CallDataSetRemoteDescription, Proceed) {
    R5PeerContainer peer_container;
    const std::string peer_id1 = "peer_id1";
    const std::string sdp = "sdp";
    std::unique_ptr<DummyPeer> dummy_peer(new DummyPeer());
    EXPECT_CALL(*dummy_peer.get(), SetRemoteDescription(_));
    DummyProxy dummy_proxy(peer_id1, sdp);
    peer_container.Add(peer_id1, std::move(dummy_peer));

    auto cd = new CallDataSetRemoteDescription(static_cast<SetRemoteDescriptionProxy*>(&dummy_proxy), &peer_container);
    cd->Proceed();

    EXPECT_EQ(dummy_proxy.status.error_code(), grpc::StatusCode::OK);
    EXPECT_EQ(dummy_proxy.sdp, sdp);
}

TEST(CallDataSetRemoteDescription, FailedResponse) {
    R5PeerContainer peer_container;

    const std::string peer_id1 = "peer_id1";
    const std::string sdp = "sdp";
    DummyProxy dummy_proxy(peer_id1, sdp);

    auto cd = new CallDataSetRemoteDescription(static_cast<SetRemoteDescriptionProxy*>(&dummy_proxy), &peer_container);
    cd->Proceed();

    EXPECT_EQ(dummy_proxy.status.error_code(), grpc::StatusCode::INVALID_ARGUMENT);
}
