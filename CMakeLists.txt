cmake_minimum_required(VERSION 3.0)

project(r5)

set(webrtc_repo_branch branch-heads/m79)

add_subdirectory(third_party)
add_subdirectory(protos)
add_subdirectory("${PROJECT_SOURCE_DIR}/third_party/googletest" "third_party/googletest")


add_library(r5_internal_src
    src/call_data_status.cpp
    src/helpers.cpp
    src/log.cpp
    src/peer.cpp
    src/r5_audio_decoder_factory.cpp
    src/r5_call_data_destroy_peer.cpp
    src/r5_peer_container.cpp
    src/r5_call_data_set_remote_description.cpp
    src/r5_peer_factory.cpp
    src/r5_call_data_create_peer.cpp
    src/r5_rtp_packet_sender_factory.cpp
    src/r5_rtp_packet_sender.cpp
    src/r5_video_decoder_factory.cpp)

target_include_directories(r5_internal_src PUBLIC
    src)
target_link_libraries(r5_internal_src PUBLIC
    webrtc grpc++ gpr r5_proto_and_grpc)
target_compile_options(r5_internal_src PUBLIC
    -fvisibility=hidden)

add_executable(r5
    src/r5.cpp)
set_target_properties(r5 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
target_link_libraries(r5 PRIVATE r5_internal_src)


add_executable(r5client
    src/log.cpp
    src/r5_client.cpp)
set_target_properties(r5client PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
target_link_libraries(r5client PRIVATE grpc++ gpr r5_proto_and_grpc)


# ref. https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html
enable_testing()
macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    set_target_properties(${TESTNAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)
    target_link_libraries(${TESTNAME} r5_internal_src gmock gmock_main)
    target_include_directories(${TESTNAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include"
        "${CMAKE_SOURCE_DIR}/third_party/googletest/googlemock/include"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/test")
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

package_add_test(helpers_test test/helpers_test.cpp)
package_add_test(r5_audio_decoder_factory_test test/r5_audio_decoder_factory_test.cpp)
package_add_test(r5_call_data_create_peer_test test/r5_call_data_create_peer_test.cpp)
package_add_test(r5_call_data_destroy_peer_test test/r5_call_data_destroy_peer_test.cpp)
package_add_test(r5_call_data_set_remote_description_test test/r5_call_data_set_remote_description_test.cpp)
package_add_test(r5_peer_container_test test/r5_peer_container_test.cpp)
package_add_test(r5_peer_factory_test test/r5_peer_factory_test.cpp)
package_add_test(r5_rtp_packet_sender_factory_test test/r5_rtp_packet_sender_factory_test.cpp)
package_add_test(r5_video_decoder_factory_test test/r5_video_decoder_factory_test.cpp)
package_add_test(webrtc_jsep_session_description_test test/webrtc_jsep_session_description_test.cpp)
