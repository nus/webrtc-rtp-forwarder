#include "r5_rtp_packet_sender_factory.h"

namespace r5 {

R5RtpPacketSenderFactory::R5RtpPacketSenderFactory() {
}

R5RtpPacketSenderFactory::~R5RtpPacketSenderFactory() {
}

std::unique_ptr<R5RtpPacketSenderInterface> R5RtpPacketSenderFactory::CreateRtpPacketSender(rtc::Thread *thread, const std::string host, int port) {
    return R5RtpPacketSender::Create(thread, host, port);
}

}
