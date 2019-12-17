set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

get_filename_component(r5_proto "r5.proto" ABSOLUTE)
get_filename_component(r5_proto_path "${r5_proto}" PATH)

set(r5_proto_inc_dir "${CMAKE_CURRENT_BINARY_DIR}")
set(r5_proto_dir "${CMAKE_CURRENT_BINARY_DIR}")
set(r5_proto_srcs "${r5_proto_dir}/r5.pb.cc")
set(r5_proto_hdrs "${r5_proto_dir}/r5.pb.h")
set(r5_grpc_srcs "${r5_proto_dir}/r5.grpc.pb.cc")
set(r5_grpc_hdrs "${r5_proto_dir}/r5.grpc.pb.h")
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

add_custom_command(
      OUTPUT "${r5_proto_srcs}" "${r5_proto_hdrs}" "${r5_grpc_srcs}" "${r5_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${r5_proto_dir}"
        --cpp_out "${r5_proto_dir}"
        -I "${r5_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${r5_proto}"
      MAIN_DEPENDENCY "${r5_proto_path}")

add_library(r5_proto_and_grpc
    ${r5_proto_srcs}
    ${r5_grpc_srcs})
set_target_properties(r5_proto_and_grpc PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
target_include_directories(r5_proto_and_grpc PUBLIC 
    src
    "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/grpc/third_party/protobuf/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/grpc/include"
    "${CMAKE_BINARY_DIR}")
target_link_libraries(r5_proto_and_grpc PUBLIC grpc++ gpr)
target_compile_options(r5_proto_and_grpc PUBLIC
    -fvisibility=hidden)
