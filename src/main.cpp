#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "net/socket_handle.hpp"

auto clientHandler(Socket &&client) {
    std::string buffer(100, '\0');
    uint32_t total = 0;
    while (true) {
        client.recvBytes(buffer.data() + total, 1);

        if (buffer.starts_with("*1\r\n$4\r\nPING\r\n")) {
            std::println("sending pong from request {}", buffer);
            std::fflush(stdout);

            client.sendString("+PONG\r\n");
            total = 0;
            buffer = std::string(100, '\0');
            continue;
        }

        if (buffer.starts_with("*2\r\n$4\r\nECHO\r\n")) {

            std::string arg(100, '\0');
            uint32_t t = 0;
            while (true) {
                client.recvBytes(arg.data() + t, 1);
                t += 1;

                std::println("arg {}", arg);
                std::fflush(stdout);

                if (std::count(arg.begin(), arg.begin() + t, '\n') == 2) {
                    client.sendBytes(arg.c_str(), t);
                    break;
                }
            }

            total = 0;
            buffer = std::string(100, '\0');
            continue;
        }

        total += 1;
    }
}

int main(int argc, char **argv) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    //
    uint32_t port = 6379;
    //
    // int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // if (server_fd < 0) {
    //     std::cerr << "Failed to create server socket\n";
    //     return 1;
    // }
    //
    // // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // // ensures that we don't run into 'Address already in use' errors
    // int reuse = 1;
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    //     std::cerr << "setsockopt failed\n";
    //     return 1;
    // }
    //
    // struct sockaddr_in server_addr;
    // server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    // server_addr.sin_port = htons(6379);
    //
    // if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
    //     std::cerr << "Failed to bind to port 6379\n";
    //     return 1;
    // }
    //
    // int connection_backlog = 5;
    // if (listen(server_fd, connection_backlog) != 0) {
    //     std::cerr << "listen failed\n";
    //     return 1;
    // }
    //
    // struct sockaddr_in client_addr;
    // int client_addr_len = sizeof(client_addr);
    // std::cout << "Waiting for a client to connect...\n";
    //
    // // You can use print statements as follows for debugging, they'll be visible when running tests.
    // std::cout << "Logs from your program will appear here!\n";
    //
    // // Uncomment the code below to pass the first stage
    // //
    // accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    // std::cout << "Client connected\n";
    // //
    // close(server_fd);
    //
    auto server = Socket::createServer(port);

    while (true) {
        auto client = server.acceptClient();
        std::cout << "Client connected\n";
        std::thread t(clientHandler, std::move(client));
        t.detach();
    }

    server.close();

    return 0;
}
