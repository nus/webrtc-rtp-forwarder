#include "r5_rtp_packet_sender.h"
#include "log.h"
#include "r5_rtp_payload_type.h"

namespace r5 {

std::unique_ptr<R5RtpPacketSender> R5RtpPacketSender::Create(rtc::Thread *network_thread, const std::string &hostname, int port) {
    return std::unique_ptr<R5RtpPacketSender>(new R5RtpPacketSender(network_thread, hostname, port));
}

R5RtpPacketSender::R5RtpPacketSender(rtc::Thread *network_thread, const std::string &hostname, int port)
    : audio({ RTP_PAYLOD_TYPE_NONE, "", -1 })
    , video({ RTP_PAYLOD_TYPE_NONE, "", -1 }) {
    rtc::BasicPacketSocketFactory socket_factory(network_thread);
    socket.reset(socket_factory.CreateUdpSocket(rtc::SocketAddress(hostname, port), 0, 0));
}

R5RtpPacketSender::~R5RtpPacketSender() {
}

void R5RtpPacketSender::SetAudioPayloadType(R5RtpPaylodType paylod_type, const std::string &hostname, int port) {
    audio.paylod_type = paylod_type;

    if (paylod_type == RTP_PAYLOD_TYPE_NONE) {
        audio.hostname = "";
        audio.port = -1;
    } else {
        audio.hostname = hostname;
        audio.port = port;
    }
}

void R5RtpPacketSender::SetVideoPayloadType(R5RtpPaylodType paylod_type, const std::string &hostname, int port) {
    video.paylod_type = paylod_type;

    if (paylod_type == RTP_PAYLOD_TYPE_NONE) {
        video.hostname = "";
        video.port = -1;
    } else {
        video.hostname = hostname;
        video.port = port;
    }
}

void R5RtpPacketSender::SendProcess(const webrtc::RtpPacketReceived &packet) {
    // TODO: Parse payload type from SDP.

    uint8_t payload_type = packet.PayloadType();
    if (payload_type == RTP_PAYLOD_TYPE_RED) {
        // https://tools.ietf.org/html/rfc2198

        const uint8_t *payload = packet.payload().data();
        const uint8_t *payload_end = payload + packet.payload_size();

        typedef struct {
            uint8_t payload_type;
            uint32_t timestamp;
        } red_header_t;

        std::vector<red_header_t> rtp_headers;

        while ((payload < payload_end) && (payload[0] & 0x80)) {
            uint8_t pt = payload[0] & 0x7F;
            uint16_t offset = (payload[1] << 6) + (payload[2] >> 2);
            uint32_t ts = packet.Timestamp() - offset;

            rtp_headers.push_back(red_header_t {
                pt,
                ts
            });

            payload += 4;
        }

        uint8_t pt = payload[0] & 0x7F;
        rtp_headers.push_back(red_header_t {
            pt,
        });
        payload++;  // The last is 1 byte.

        if ((rtp_headers.size() == 1) && (pt == video.paylod_type)) {
            std::vector<uint8_t> d(packet.data(),
                packet.data() + packet.size() - ((4 * rtp_headers.size()) - 3));
            uint8_t mpt = d[1]; // marker and payalod type.
            d[1] = (mpt & 0x80) + video.paylod_type;
            std::copy(payload, payload_end, d.begin() + packet.headers_size());

            rtc::PacketOptions opt;
            int s = socket->SendTo(&d[0], d.size(), rtc::SocketAddress(video.hostname, video.port), opt);
            if (s < 0) {
                LOG_ERROR("socket->SendTo() failed: %d, %d", s, socket->GetError());
            }
        } else {
            LOG_ERROR("TODO: Parse RTP payloads.");
        }
    } else if (payload_type == video.paylod_type) {
        rtc::PacketOptions opt;
        int s = socket->SendTo(packet.data(), packet.size(), rtc::SocketAddress(video.hostname, video.port), opt);
        if (s < 0) {
            LOG_ERROR("socket->SendTo() failed: %d, %d", s, socket->GetError());
        }
    } else if (payload_type == audio.paylod_type) {
        rtc::PacketOptions opt;
        int s = socket->SendTo(packet.data(), packet.size(), rtc::SocketAddress(audio.hostname, audio.port), opt);
        if (s < 0) {
            LOG_ERROR("socket->SendTo() failed: %d, %d", s, socket->GetError());
        }
    }
}

}
