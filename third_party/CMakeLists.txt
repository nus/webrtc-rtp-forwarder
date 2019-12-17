set(depot_tools_path ${CMAKE_CURRENT_SOURCE_DIR}/depot_tools)
set(gclient ${depot_tools_path}/gclient)
set(fetch ${depot_tools_path}/fetch)
set(gn ${depot_tools_path}/gn)
set(ninja ${depot_tools_path}/ninja)
set(webrt_src_path ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(UNIX AND NOT APPLE)
    # For Linux
    # https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_getting_the_code
    set(install_build_deps "./build/install-build-deps.sh")
else()
    set(install_build_deps "")
endif()

if(EXISTS ${depot_tools_path})
    set(cmd_git_clone_depot_tools "")
else()
    set(cmd_git_clone_depot_tools git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git ${depot_tools_path})
endif()

add_custom_target(setup_depot_tools
    COMMENT "Setup depot_tools"
    COMMAND ${cmd_git_clone_depot_tools})

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src)
    set(cmd_fetch_webrtc "")
else()
    set(cmd_fetch_webrtc ${fetch} --nohooks webrtc)
endif()

add_custom_target(fetch_webrtc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${cmd_fetch_webrtc}
    )

# https://webrtc.org/native-code/development/#working-with-release-branches
add_custom_target(setup_webrtc
    COMMENT "Setup for building libwebrtc"
    WORKING_DIRECTORY ${webrt_src_path}
    COMMAND ${install_build_deps}
    COMMAND ${gclient} sync --nohooks
    COMMAND git checkout -b ${webrtc_repo_branch} refs/remotes/${webrtc_repo_branch}
    COMMAND ${gclient} sync 
    )
add_dependencies(setup_webrtc setup_depot_tools fetch_webrtc)

# https://mediasoup.discourse.group/t/undefined-reference-when-building-libmediasoupclient/160/5
set (gn_gen_args_common is_component_build=false is_clang=true rtc_include_tests=false rtc_use_h264=false rtc_enable_protobuf=false use_rtti=true use_custom_libcxx=false)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(gn_gen_args --args='${gn_gen_args_common}')
else()
    set(gn_gen_args --args='${gn_gen_args_common} is_debug=false')
endif()

add_custom_target(patch_webrtc
    COMMENT "Patch for webrtc"
    WORKING_DIRECTORY ${webrt_src_path}
    COMMAND cat ../../patch/webrtc.diff | patch -p1)

add_custom_target(build_webrtc
    COMMENT "Build webrtc"
    WORKING_DIRECTORY ${webrt_src_path}
    COMMAND ${gn} gen out/Default ${gn_gen_args}
    COMMAND ${ninja} -C out/Default)

add_library(webrtc INTERFACE)
target_include_directories(webrtc INTERFACE
    ${webrt_src_path} ${webrt_src_path}/third_party/abseil-cpp)
target_link_options(webrtc INTERFACE
    -L${webrt_src_path}/out/Default/obj
    -L${webrt_src_path}/out/Default/obj/api)
target_compile_options(webrtc INTERFACE
    -std=c++14
    -fvisibility=hidden)

if(APPLE)
    # For macOS
    target_compile_definitions(webrtc INTERFACE
        WEBRTC_MAC WEBRTC_POSIX)
    target_link_libraries(webrtc INTERFACE
        -lwebrtc
        -ljingle_peerconnection_api
        "-framework Foundation"
        "-framework AVFoundation"
        "-framework CoreServices"
        "-framework CoreFoundation"
        "-framework AudioUnit"
        "-framework AudioToolbox"
        "-framework CoreAudio"
        "-framework CoreGraphics"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework VideoToolbox"
        -std=c++14)
elseif(UNIX AND NOT APPLE)
    # For Linux
    target_compile_definitions(webrtc INTERFACE
        WEBRTC_POSIX)
    target_link_libraries(webrtc INTERFACE
        -lwebrtc
        -ljingle_peerconnection_api
        -lX11
        -std=c++14)
endif()

################## gRPC ########################
add_subdirectory(grpc)
