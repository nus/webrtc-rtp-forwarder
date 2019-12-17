#pragma once

#include <p2p/base/basic_packet_socket_factory.h>
#include <modules/rtp_rtcp/source/rtp_packet_received.h>
#include <memory>
#include "r5_rtp_payload_type.h"

namespace r5 {

class R5RtpPacketSenderInterface {
public:
    virtual void SetAudioPayloadType(R5RtpPaylodType paylod_type, const std::string &hostname, int port) = 0;
    virtual void SetVideoPayloadType(R5RtpPaylodType paylod_type, const std::string &hostname, int port) = 0;
    virtual void SendProcess(const webrtc::RtpPacketReceived &packet) = 0;
    virtual ~R5RtpPacketSenderInterface() {}
};

class R5RtpPacketSender : public R5RtpPacketSenderInterface {
public:
    static std::unique_ptr<R5RtpPacketSender> Create(rtc::Thread *network_thread, const std::string &hostname, int port);
    void SetAudioPayloadType(R5RtpPaylodType paylod_type, const std::string &hostname, int port) override;
    void SetVideoPayloadType(R5RtpPaylodType paylod_type, const std::string &hostname, int port) override;
    void SendProcess(const webrtc::RtpPacketReceived &packet) override;
    ~R5RtpPacketSender();

private:
    R5RtpPacketSender(rtc::Thread *network_thread, const std::string &hostname, int port);

private:
    std::unique_ptr<rtc::AsyncPacketSocket> socket;
    struct {
        R5RtpPaylodType paylod_type;
        std::string hostname;
        int port;
    } audio, video;
};

}
