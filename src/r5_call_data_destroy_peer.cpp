#include "r5_call_data_destroy_peer.h"
#include "call_data_status.h"
#include "log.h"

namespace r5 {

CallDataDestroyPeer::CallDataDestroyPeer(R5GrpcServiceProxy<DestroyPeerResponse> *p, r5::R5PeerContainer *c)
    : destroy_peer_proxy(p)
    , responder(&context)
    , r5_peer_container(c)
    , status(CREATE) {
    Proceed();
}

CallDataDestroyPeer::~CallDataDestroyPeer() {
}

void CallDataDestroyPeer::Proceed() {
    LOG_DEBUG("CallDataDestroyPeer 0x%p status: %s", this, call_status_to_string(status));
    if (status == CREATE) {
        status = PROCESS;
        destroy_peer_proxy->RequestDestroyPeer(&context, &request, &responder, this);
    } else if (status == PROCESS) {
        new CallDataDestroyPeer(destroy_peer_proxy, r5_peer_container);
        LOG_DEBUG("CallDataDestroyPeer, peer_id: %s", request.peer_id().c_str());
        status = FINISH;

        R5PeerInterface *peer = r5_peer_container->Get(request.peer_id());
        if (peer) {
            peer->Close();
            r5_peer_container->Remove(request.peer_id());
            destroy_peer_proxy->Finish(&responder, response, grpc::Status::OK, this);
        } else {
            grpc::Status ng(grpc::StatusCode::INVALID_ARGUMENT, "Invalid peer_id: " + request.peer_id());
            destroy_peer_proxy->FinishWithError(&responder, ng, this);
        }
    } else if (status == FINISH) {
        delete this;
    } else {
        delete this;
    }
}

}
