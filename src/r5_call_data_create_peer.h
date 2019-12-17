#pragma once

#include <protos/r5.grpc.pb.h>
#include <protos/r5.pb.h>
#include <grpcpp/grpcpp.h>
#include "r5_call_data_interface.h"
#include "r5_grpc_service_proxy.h"
#include "r5_peer_factory.h"
#include "call_data_status.h"
#include "r5_peer_container.h"

namespace r5 {

class CallDataCreatePeer : public r5::CallDataInterface, public r5::R5SdpObserverInterface {
public:
    CallDataCreatePeer(r5::R5GrpcServiceProxy<r5::CreatePeerResponse> *p, r5::R5PeerFactoryInterface *f, r5::R5PeerContainer *c);
    ~CallDataCreatePeer();

    void Proceed() override;

    void OnOfferSdpSuccess(std::string offer_sdp, std::string rtp_sdp) override;

    void OnOfferSdpFailure() override;

private:
    r5::R5GrpcServiceProxy<r5::CreatePeerResponse> *create_peer_proxy;
    grpc::ServerContext context;
    r5::CreatePeerRequest request;
    r5::CreatePeerResponse response;
    grpc::ServerAsyncResponseWriter<r5::CreatePeerResponse> responder;
    r5::R5PeerFactoryInterface *r5_peer_factory;
    r5::R5PeerContainer *r5_peer_container;
    std::unique_ptr<r5::R5PeerInterface> r5_peer;

    r5::CallDataStatus status;
};

}
