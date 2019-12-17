#pragma once

#include <protos/r5.grpc.pb.h>
#include <protos/r5.pb.h>
#include <grpcpp/grpcpp.h>
#include "r5_call_data_interface.h"
#include "r5_grpc_service_proxy.h"
#include "call_data_status.h"
#include "r5_peer_container.h"

namespace r5 {

class CallDataSetRemoteDescription : public CallDataInterface {
public:
    CallDataSetRemoteDescription(R5GrpcServiceProxy<SetRemoteDescriptionResponse> *set_remote_description_proxy, R5PeerContainer *c);
    ~CallDataSetRemoteDescription();
    void Proceed() override;

private:
    R5GrpcServiceProxy<SetRemoteDescriptionResponse> *set_remote_description_proxy;
    grpc::ServerContext context;
    SetRemoteDescriptionRequest request;
    SetRemoteDescriptionResponse response;
    grpc::ServerAsyncResponseWriter<SetRemoteDescriptionResponse> responder;
    R5PeerContainer *r5_peer_container;

    CallDataStatus status;
};

}
