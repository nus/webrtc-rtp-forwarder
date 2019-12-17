#pragma once

#include "r5_rtp_packet_sender.h"
#include <rtc_base/thread.h>
#include <memory>

namespace r5 {

class R5RtpPacketSenderFactoryInterface {
public:
    virtual std::unique_ptr<R5RtpPacketSenderInterface> CreateRtpPacketSender(rtc::Thread *thread, const std::string host, int port) = 0;
    virtual ~R5RtpPacketSenderFactoryInterface() {}
};

class R5RtpPacketSenderFactory : public R5RtpPacketSenderFactoryInterface {
public:
    R5RtpPacketSenderFactory();
    ~R5RtpPacketSenderFactory() override;
    std::unique_ptr<R5RtpPacketSenderInterface> CreateRtpPacketSender(rtc::Thread *thread, const std::string host, int port) override;
};

}
