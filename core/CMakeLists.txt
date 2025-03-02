# Define source and header files
set(SOURCES
        src/main.c
        src/server.c
        src/command.c

        src/mynetworks.c
        src/create_tcp_socket.c
        src/bind_socket.c
        src/listen_socket.c
        src/accept_tcp_connection_impl.c
        src/close_socket.c
        src/convert_ip_address.c
        src/convert_address_to_string.c
        src/print_socket_error.c
)

set(HEADERS
    include/myftp.h
)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Add cvector library
target_link_libraries(${PROJECT_NAME} PRIVATE cvector)

# Set compile options for Debug and Release modes
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-g -Wall>
        $<$<CONFIG:Release>:-O3 -Wall>
)
