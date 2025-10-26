set(sources
    src/tmp.cpp
    src/sys_utils.cpp
    src/tap_device.cpp
    src/ethernet_frame.cpp
    src/udp_socket.cpp
    src/vport.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
    include/project/tmp.hpp
    include/project/expected.hpp
    include/project/joining_thread.hpp
    include/project/sys_utils.hpp
    include/project/tap_device.hpp
    include/project/ethernet_frame.hpp
    include/project/udp_socket.hpp
    include/project/vport.hpp
)

set(test_sources
  src/tmp_test.cpp
  src/expected_test.cpp
  src/joining_thread_test.cpp
  src/sys_utils_test.cpp
  src/tap_device_test.cpp
  src/ethernet_frame_test.cpp
  src/udp_socket_test.cpp
)
