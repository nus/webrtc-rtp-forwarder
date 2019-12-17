#include <protos/r5.grpc.pb.h>
#include <protos/r5.pb.h>
#include <grpcpp/grpcpp.h>

#include "log.h"

namespace {

class R5Client {
public:
    R5Client(std::shared_ptr<grpc::Channel> channel)
        :stub(r5::R5Service::NewStub(channel)) {
    }

    void CreatePeer() {
        r5::CreatePeerRequest request;
        request.set_audio(true);
        request.set_video(true);
        request.set_ice_server_uri("stun:stun.l.google.com:19302");

        grpc::ClientContext context;
        r5::CreatePeerResponse response;

        grpc::Status status = stub->CreatePeer(&context, request, &response);
        if (status.ok()) {
            LOG_INFO("offer_sdp: %s", response.offer_sdp().c_str());
            LOG_INFO("peer_id: %s", response.peer_id().c_str());
        } else {
            LOG_ERROR("CreatePeer() failed. %d, %s", status.error_code(), status.error_message().c_str());
        }
    }

    void DestroyPeer() {
        r5::DestroyPeerRequest request;
    }
private:
    std::unique_ptr<r5::R5Service::Stub> stub;
};

}

int main(int argc, char *argv[]) {
    set_log_level(LOG_LEVEL_INFO);

    if (argc < 2) {
        LOG_ERROR("You need to specify server address. (e.g. localhost:50001");
        return 1;
    }

    std::string address(argv[1]);

    R5Client cli(grpc::CreateChannel(
      address, grpc::InsecureChannelCredentials()));

    cli.CreatePeer();

    return 0;
}
