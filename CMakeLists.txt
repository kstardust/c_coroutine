project(c_coroutine)

include_directories("/home/kevin/usr/include")
link_directories("/home/kevin/usr/lib")
add_executable(main main.c coroutine.c coroutine.h)
target_link_libraries(main)
set_target_properties(main  PROPERTIES LINK_FLAGS "-Wl,-rpath,/home/kevin/usr/lib")

