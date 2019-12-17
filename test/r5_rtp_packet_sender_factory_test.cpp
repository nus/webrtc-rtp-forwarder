#include "r5_rtp_packet_sender_factory.h"
#include <gtest/gtest.h>
#include <rtc_base/ssl_adapter.h>
#include <rtc_base/thread.h>

TEST(R5RtpPacketSenderFactory, CreateRtpPacketSender) {
    rtc::InitializeSSL();
    r5::R5RtpPacketSenderFactory factory;
    std::unique_ptr<rtc::Thread> rtp_thread = rtc::Thread::CreateWithSocketServer();
    rtp_thread->Start();

    std::unique_ptr<r5::R5RtpPacketSenderInterface> sender;
    rtp_thread->Invoke<void>(RTC_FROM_HERE, [&factory, &rtp_thread, &sender] {
        sender = std::move(factory.CreateRtpPacketSender(rtp_thread.get(), "127.0.0.1", 0));
    });
    EXPECT_NE(nullptr, sender);
    rtc::CleanupSSL();
}
