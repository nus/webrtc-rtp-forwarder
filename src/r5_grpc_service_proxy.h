#pragma once

#include <protos/r5.grpc.pb.h>
#include <protos/r5.pb.h>
#include <grpcpp/grpcpp.h>

#include "log.h"

namespace r5 {

template<class ResponseType>
class R5GrpcServiceProxy {
public:
    R5GrpcServiceProxy(R5Service::AsyncService *s, grpc::ServerCompletionQueue *cq)
        : service(s) 
        , completion_queue(cq) {}
    ~R5GrpcServiceProxy() {}

    virtual void RequestCreatePeer(grpc::ServerContext *context,
                           CreatePeerRequest *request,
                           grpc::ServerAsyncResponseWriter<CreatePeerResponse> *responder,
                           void *tag) {
        service->RequestCreatePeer(context, request, responder, completion_queue, completion_queue, tag);
    }

    virtual void RequestDestroyPeer(grpc::ServerContext *context,
                           DestroyPeerRequest *request,
                           grpc::ServerAsyncResponseWriter<DestroyPeerResponse> *responder,
                           void *tag) {
        service->RequestDestroyPeer(context, request, responder, completion_queue, completion_queue, tag);
    }

    virtual void RequestSetRemoteDescription(grpc::ServerContext *context,
                           SetRemoteDescriptionRequest *request,
                           grpc::ServerAsyncResponseWriter<SetRemoteDescriptionResponse> *responder,
                           void *tag) {
        service->RequestSetRemoteDescription(context, request, responder, completion_queue, completion_queue, tag);
    }

    virtual void Finish(grpc::ServerAsyncResponseWriter<ResponseType> *responder, const ResponseType& msg, const grpc::Status& status, void* tag) {
        responder->Finish(msg, status, tag);
    }

    virtual void FinishWithError(grpc::ServerAsyncResponseWriter<ResponseType> *responder, const grpc::Status& status, void* tag) {
        responder->FinishWithError(status, tag);
    }

private:
    R5Service::AsyncService *service;
    grpc::ServerCompletionQueue *completion_queue;
};

}
