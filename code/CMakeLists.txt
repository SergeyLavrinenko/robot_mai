cmake_minimum_required(VERSION 2.8)
project(main)
find_package( OpenCV REQUIRED )

find_package(PkgConfig REQUIRED)
pkg_check_modules(Mosquitto REQUIRED IMPORTED_TARGET libmosquitto)


include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable(main 
    "main.cpp"
    "detector.cpp"
    "robot.cpp"
    "robot_control.cpp"
    "drink_control.cpp"
    "order_manager.cpp")

target_link_libraries( main ${OpenCV_LIBS}
    PkgConfig::Mosquitto )

