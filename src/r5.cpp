#include <stdio.h>

#include <unistd.h>
#include <memory>
#include <map>
#include <string>

#include "peer.h"
#include "log.h"
#include "helpers.h"
#include "call_data_status.h"
#include "r5_peer_factory.h"
#include "r5_peer_container.h"
#include "r5_call_data_interface.h"
#include "r5_call_data_create_peer.h"
#include "r5_call_data_destroy_peer.h"
#include "r5_call_data_set_remote_description.h"
#include "r5_grpc_service_proxy.h"

#include <protos/r5.grpc.pb.h>
#include <protos/r5.pb.h>
#include <grpcpp/grpcpp.h>

#include <thread>

// r5 (webRtc Rtp foRwaRdeR)

namespace {

void StartAsyncService(const std::string &address) {
    r5::R5Service::AsyncService service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    // grpc::ServerCompletionQueue is thread safe therefore it is not necessary mutex for completion_queue.
    // completion_queue_impl.h
    // https://github.com/grpc/grpc/issues/4486
    std::unique_ptr<grpc::ServerCompletionQueue> completion_queue = builder.AddCompletionQueue();
    std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

    r5::R5PeerFactory peer_factory(nullptr,
                                   std::make_unique<r5::R5AudioDecoderFactory>(),
                                   nullptr);
    r5::R5PeerContainer peer_container;

    r5::R5GrpcServiceProxy<r5::CreatePeerResponse> create_peer_proxy(&service, completion_queue.get());
    r5::R5GrpcServiceProxy<r5::DestroyPeerResponse> destroy_peer_proxy(&service, completion_queue.get());
    r5::R5GrpcServiceProxy<r5::SetRemoteDescriptionResponse> set_remote_description_proxy(&service, completion_queue.get());

    new r5::CallDataCreatePeer(&create_peer_proxy, &peer_factory, &peer_container);
    new r5::CallDataDestroyPeer(&destroy_peer_proxy, &peer_container);
    new r5::CallDataSetRemoteDescription(&set_remote_description_proxy, &peer_container);

    void *tag = nullptr;
    bool ok = false;

    while (true) {
        if (!completion_queue->Next(&tag, &ok)) {
            LOG_ERROR("completion_queue->Next() failed.");
            break;
        } else if (!ok) {
            LOG_ERROR("completion_queue->Next() failed by not ok.");
            break;
        } else {
            static_cast<r5::CallDataInterface*>(tag)->Proceed();
        }
    }

    server->Shutdown();
    completion_queue->Shutdown();
}

}

int main(int argc, char** argv) {
    set_log_level(LOG_LEVEL_DEBUG);

    if (argc < 2) {
        LOG_ERROR("You need to specify listening address. (e.g. 0.0.0.0:50001");
        return 1;
    }

    std::string address(argv[1]);

    r5::Peer::Initialize();

    StartAsyncService(address);

    r5::Peer::Finalize();

    return 0;
}
