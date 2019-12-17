# WebRTC-RTP Forwarder

![](https://github.com/nus/webrtc-rtp-forwarder/workflows/CI/badge.svg)

`WebRTC-RTP Forwarder` is a gRPC service for forwarding RTP media packets from WebRTC connections.

## Requirements

* C/C++ Compiler toolchain (e.g. clang, gcc
* [CMake](https://cmake.org/)
* [Git](https://git-scm.com/)
* [Go](https://golang.org/)
* [FFmpeg](https://www.ffmpeg.org/) (Optional for sample)

## Build

```
mkdir build
cd build
cmake ..
make setup_webrtc
make patch_webrtc
make build_webrtc
make
```

## Sample

```
cd build
./r5 0.0.0.0:50001
```

```
cd sample
go run main.go
```

Open http://localhost:8080/

```
cd sample
ffplay -v trace -protocol_whitelist "file,rtp,udp" out.sdp
```


## License

```
Copyright (C) 2019 Yota Ichino

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
