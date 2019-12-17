#include "r5_call_data_set_remote_description.h"
#include "log.h"

namespace r5 {

CallDataSetRemoteDescription::CallDataSetRemoteDescription(R5GrpcServiceProxy<SetRemoteDescriptionResponse> *p, R5PeerContainer *c)
    : set_remote_description_proxy(p)
    , responder(&context)
    , r5_peer_container(c)
    , status(CREATE) {
    Proceed();
}

CallDataSetRemoteDescription::~CallDataSetRemoteDescription() {
}

void CallDataSetRemoteDescription::Proceed() {
    LOG_DEBUG("CallDataSetRemoteDescription 0x%p status: %s", this, call_status_to_string(status));
    if (status == CREATE) {
        status = PROCESS;
        set_remote_description_proxy->RequestSetRemoteDescription(&context, &request, &responder, this);
    } else if (status == PROCESS) {
        new CallDataSetRemoteDescription(set_remote_description_proxy, r5_peer_container);
        LOG_DEBUG("CallDataSetRemoteDescription, peer_id: %s", request.peer_id().c_str());

        status = FINISH;
        R5PeerInterface *peer = r5_peer_container->Get(request.peer_id());
        if (peer) {
            peer->SetRemoteDescription(request.answer_sdp());
            set_remote_description_proxy->Finish(&responder, response, grpc::Status::OK, this);
        } else {
            grpc::Status ng(grpc::StatusCode::INVALID_ARGUMENT, "Invalid peer_id: " + request.peer_id());
            set_remote_description_proxy->FinishWithError(&responder, ng, this);
        }
    } else if (status == FINISH) {
        delete this;
    } else {
        delete this;
    }
}

}
