project (communication)

# Tools for using NK parser.
include (platform/nk)

# Set compile flags.
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -Wno-deprecated-declarations")

add_executable (Client 
    "src/controller.cpp" 
    "src/subscriber.cpp" 
    "src/message_validator.cpp"
    "src/message_sender.cpp")

add_dependencies (Client echo_idl_files echo_nav_idl_files echo_controller_edl_files)

target_link_libraries(Client 
                      ${mosquitto_CXX_LIB}
                      ${vfs_CLIENT_LIB}                  
)

set_target_properties (Client PROPERTIES ${vfs_ENTITY}_REPLACEMENT "")
set_target_properties (Client PROPERTIES LINK_FLAGS ${VULN_LDFLAGS})