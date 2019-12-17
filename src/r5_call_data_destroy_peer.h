#pragma once

#include <protos/r5.grpc.pb.h>
#include <protos/r5.pb.h>
#include <grpcpp/grpcpp.h>
#include "r5_call_data_interface.h"
#include "r5_grpc_service_proxy.h"
#include "call_data_status.h"
#include "r5_peer_container.h"

namespace r5 {

class CallDataDestroyPeer : public CallDataInterface {
public:
    CallDataDestroyPeer(R5GrpcServiceProxy<DestroyPeerResponse> *destroy_peer_proxy, R5PeerContainer *c);
    ~CallDataDestroyPeer();
    void Proceed() override;

private:
    R5GrpcServiceProxy<DestroyPeerResponse> *destroy_peer_proxy;
    grpc::ServerContext context;
    DestroyPeerRequest request;
    DestroyPeerResponse response;
    grpc::ServerAsyncResponseWriter<DestroyPeerResponse> responder;
    R5PeerContainer *r5_peer_container;

    CallDataStatus status;
};

}