project (navigation)


# Tools for using NK parser.
include (platform/nk)

# Set compile flags.
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -Wno-deprecated-declarations")


find_package(Threads REQUIRED)
add_executable (NavigationController "src/controller.cpp" "src/navigation.cpp" "src/pwm_controller.cpp")
add_dependencies (NavigationController echo_nav_idl_files echo_nav_edl_files)
target_link_libraries(NavigationController 
    Threads::Threads ${gpio_CLIENT_LIB} ${bsp_CLIENT_LIB})
