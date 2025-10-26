set(sources
    src/tmp.cpp
    src/sys_utils.cpp
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
)

set(test_sources
  src/tmp_test.cpp
  src/expected_test.cpp
  src/joining_thread_test.cpp
  src/sys_utils_test.cpp
)
