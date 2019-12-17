package main

import (
	"context"
	"fmt"
	"os"
	"net/http"
	"github.com/gorilla/websocket"
	"google.golang.org/grpc"
	r5 "./r5" // protoc -I ../protos/ --go_out=plugins=grpc:./r5 r5.proto
)

const GRPC_R5_URL = "localhost:50001"

const indexHtml = `
<html>
<head>
<title>WebRTC-RTP Forwarder Sample - Video Sender</title>
<script>
'use strict';

let peer = null;
let conn = null;
let local_stream = null;

function startConnect() {
	conn = new WebSocket('ws://localhost:8080/ws');
	conn.onmessage = (evt) => {
		let d =JSON.parse(evt.data);
		let type = d['type'];
		let payload = d['payload'];

		if (type === 'offer') {
			let config = null;
			peer = new RTCPeerConnection(config);
			local_stream.getTracks().forEach(track => peer.addTrack(track, local_stream));
			peer.setRemoteDescription(new RTCSessionDescription({
				type: 'offer',
				sdp: payload,
			})).then(() => {
				return peer.createAnswer();
			}).then((desc) => {
				return peer.setLocalDescription(desc);
			}).then(() => {
				conn.send(JSON.stringify({
					type: 'answer',
					payload: peer.localDescription.sdp,
				}))
			}).catch((e) => {
				console.error(e);
			});
		} else {
			console.error('unexpected message', d);
		}
	};
	conn.onclose = (evt) => {
		console.log('Closed connection.');
		conn = null;
	};
}

window.onload = () => {
	navigator.mediaDevices.getUserMedia({
		video: true,
		audio: true,
	}).then((stream) => {
		local_video.srcObject = stream;
		local_video.volume = 0;

		local_stream = stream;

		startConnect();
	}).catch(err => {
		console.log(JSON.stringify(err));
	});
};
</script>
</head>
<body>
	<video id="local_video" autoplay>
</body>
</html>
`

type message struct {
	Type		string	`json:"type"`
	Payload		string	`json:"payload"`
}

func main() {
	upgrader := websocket.Upgrader{
		CheckOrigin: func(r *http.Request) bool {
			return true
		},
	}

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprintf(w, indexHtml)
	})
	http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			return
		}
		defer conn.Close()

		// TODO: PING/PONG

		msgch := make(chan message)
		go func() {
			m := message{}
			for {
				if err := websocket.ReadJSON(conn, &m); err != nil {
					fmt.Fprintf(os.Stderr, "websocket.ReadJSON() returns ClosedError %v\n", err)
					close(msgch)
					return
				} else {
					fmt.Printf("websocket.ReadJSON() returns %v\n", m)
					msgch <- m
				}
			}
		}()

		grpcConn, err := grpc.Dial(GRPC_R5_URL, grpc.WithInsecure())
		if err != nil {
			panic(fmt.Sprintf("grpc.Dial() failed: %v\n", err))
		}
		defer grpcConn.Close()
		r5Client := r5.NewR5ServiceClient(grpcConn)

		res, err := r5Client.CreatePeer(context.TODO(), &r5.CreatePeerRequest{
			Audio: true,
			Video: true,
			AudioRtp: &r5.CreatePeerRequest_RtpTarget{
				Host: "127.0.0.01",
				Port: 30002,
			},
			VideoRtp: &r5.CreatePeerRequest_RtpTarget{
				Host: "127.0.0.01",
				Port: 30004,
			},
			IceServerUri: "stun:stun.l.google.com:19302",
		})
		if err != nil {
			return;
		}

		peer_id := res.PeerId;
		websocket.WriteJSON(conn, &message{
			Type: "offer",
			Payload: res.OfferSdp,
		})

		fmt.Printf("CreatePeerResponse: peer_id: %s \n", res.PeerId)
		fmt.Printf("CreatePeerResponse: offer_sdp: %s \n", res.OfferSdp)
		fmt.Printf("CreatePeerResponse: rpt_sdp: %s \n", res.RtpSdp)

		go func () {
			file, err := os.Create("out.sdp")
			if err != nil {
				fmt.Fprintf(os.Stderr, "os.Create(out.sdp) failed: %v", err)
				return
			}
			defer file.Close()
			file.Write(([]byte)(res.RtpSdp))
		}()

		for {
			select {
			case m, ok := <- msgch:
				if ok {
					if m.Type == "answer" {
						fmt.Printf("type is answer\n")
						r5Client.SetRemoteDescription(context.TODO(), &r5.SetRemoteDescriptionRequest{
							AnswerSdp: m.Payload,
							PeerId: peer_id,
						})
					}
				} else {
					goto closed
				}
			}
		}
	closed:
		fmt.Printf("closed.\n")
		_, err = r5Client.DestroyPeer(context.TODO(), &r5.DestroyPeerRequest{
			PeerId: peer_id,
		});
		if err != nil {
			fmt.Fprintf(os.Stderr, "DestroyPeer() failed.\n")
		} else {
			fmt.Printf("DestroyPeer() succeeded.\n")
		}
	})
	if err := http.ListenAndServe(":8080", nil); err != nil {
		fmt.Fprintf(os.Stderr, "httpListenAndServe() failed: %v\n", err)
	}
}
