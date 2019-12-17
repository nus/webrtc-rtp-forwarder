#include "r5_call_data_create_peer.h"

#include "helpers.h"

namespace r5 {

CallDataCreatePeer::CallDataCreatePeer(r5::R5GrpcServiceProxy<r5::CreatePeerResponse> *p, r5::R5PeerFactoryInterface *f, r5::R5PeerContainer *c)
    : create_peer_proxy(p)
    , responder(&context)
    , r5_peer_factory(f)
    , r5_peer_container(c)
    , r5_peer(nullptr)
    , status(r5::CallDataStatus::CREATE) {
    Proceed();
}

CallDataCreatePeer::~CallDataCreatePeer() {
}

void CallDataCreatePeer::Proceed() {
    LOG_DEBUG("CallDataCreatePeer, %p status: %s", this, r5::call_status_to_string(status));
    if (status == r5::CallDataStatus::CREATE) {
        status = r5::CallDataStatus::PROCESS;
        create_peer_proxy->RequestCreatePeer(&context, &request, &responder, this);
    } else if (status == r5::CallDataStatus::PROCESS) {
        new CallDataCreatePeer(create_peer_proxy, r5_peer_factory, r5_peer_container);

        r5::R5PeerParameter param;
        param.audio = request.audio();
        param.video = request.video();
        param.audio_rtp_host = request.audio_rtp().host();
        param.audio_rtp_port = request.audio_rtp().port();
        param.video_rtp_host = request.video_rtp().host();
        param.video_rtp_port = request.video_rtp().port();
        param.ice_server_uri = request.ice_server_uri();

        r5_peer = r5_peer_factory->CreatePeer(param);
        if (r5_peer) {
            r5_peer->CreateOffer(this);
        } else {
            LOG_ERROR("r5_peer_factory->CreatePeer() failed.");
            status = r5::CallDataStatus::FINISH;
            grpc::Status ng(grpc::StatusCode::RESOURCE_EXHAUSTED, "Failed to creat a peer resource");
            create_peer_proxy->FinishWithError(&responder, ng, this);
        }
    } else if (status == r5::CallDataStatus::FINISH) {
        delete this;
    } else {
        LOG_ERROR("CallDataCreatePeer, unexpected status: %d", status);
        delete this;
    }
}

void CallDataCreatePeer::OnOfferSdpSuccess(std::string offer_sdp, std::string rtp_sdp) {
    status = r5::CallDataStatus::FINISH;
    std::string peer_id = r5::CreatePeerId();

    response.set_offer_sdp(std::move(offer_sdp));
    response.set_peer_id(peer_id);
    response.set_rtp_sdp(std::move(rtp_sdp));

    LOG_DEBUG("peer_id: %s 0x%p", peer_id.c_str(), r5_peer.get());
    r5_peer_container->Add(peer_id, std::move(r5_peer));
    create_peer_proxy->Finish(&responder, response, grpc::Status::OK, this);
}

void CallDataCreatePeer::OnOfferSdpFailure()  {
    status = r5::CallDataStatus::FINISH;
    grpc::Status ng(grpc::StatusCode::INTERNAL, "Failed to create an offer.");
    create_peer_proxy->FinishWithError(&responder, ng, this);
}

}
