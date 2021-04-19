#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef __SHARED_H__
#define __SHARED_H__

#include <winsock2.h> // always include winsock2.h before windows.h
#include <WS2tcpip.h>
#include <iphlpapi.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include <fstream>
#include <iostream>
#include <windows.h>

#pragma comment (lib, "ws2_32.lib")

enum ApplicationType
{
    SERVER,
    CLIENT,

    UNKNOWN
};

enum PORTS
{
    SYNC_TCP_SERVER = 27015,
    SYNC_UDP_SERVER = 27017,

    ASYNC_TCP_SERVER = 10000,
    ASYNC_UDP_SERVER = 10011
};

inline void WS_ERROR(std::string log, ApplicationType tag = SERVER)
{
	std::cout << ((tag == SERVER) ? "[SERVER]" : "[CLIENT]") << log << std::endl;
}

inline void WS_ERROR(std::string log, int code, ApplicationType tag = SERVER)
{
	std::cout << ((tag == SERVER) ? "[SERVER]" : "[CLIENT]") << log << " " << code << std::endl;
}

inline void WS_LOG(std::string log, ApplicationType tag = SERVER)
{
    std::cout << ((tag == SERVER) ? "[SERVER]" : "[CLIENT]") << log << std::endl;
}

inline void WS_LOG(std::string log, int code, ApplicationType tag = SERVER)
{
    std::cout << ((tag == SERVER) ? "[SERVER]" : "[CLIENT]") << log << " " << code << std::endl;
}

namespace ClientServerApplication_Sync
{
    auto f_prompt = [](const std::string& prompt) -> bool { std::cout << prompt; return true; };

    void TCP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::SYNC_TCP_SERVER);
        soc_listen_info.sin_addr.s_addr = inet_addr(serverIP);

        SOCKET soc_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (soc_listen == INVALID_SOCKET)
        {
            WS_ERROR("socket failed with error code:", WSAGetLastError());
            return;
        }

        rc = bind(soc_listen, (SOCKADDR*)&soc_listen_info, sizeof(soc_listen_info));
        if (rc)
        {
            WS_ERROR("bind failed with error code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }

        rc = listen(soc_listen, 1);
        if (rc)
        {
            WS_ERROR("listen failed with error code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }

        WS_LOG("Start service on server side...");

        SOCKET soc_client = INVALID_SOCKET;
        while (true)
        {
            soc_client = accept(soc_listen, NULL, NULL);
            if (soc_client == INVALID_SOCKET)
            {
                WS_ERROR("accept failed with error code:", WSAGetLastError());
                closesocket(soc_listen);
                return;
            }

            WS_LOG("Client Accepted");

            auto f_handler =
            [](SOCKET client) 
            {
                int rc = 0;
                do
                {
                    char request_from_client[1024];
                    memset(request_from_client, '\0', 1024);
                    rc = recv(client, request_from_client, 1024, 0);
                    if (rc > 0)
                    {
                        if (std::string("disconnect") == std::string(request_from_client)
                            || std::string("d") == std::string(request_from_client))
                        {
                            std::string greeting = "Thanks for using this service!!!...";
                            rc = send(client, greeting.data(), greeting.length(), 0);
                            if (rc == SOCKET_ERROR)
                            {
                                WS_ERROR("send failed with error code:", WSAGetLastError());
                            }
                            break;
                        }
                        else
                        {
                            std::string greeting = "Invalid request!";
                            rc = send(client, greeting.data(), greeting.length(), 0);
                            if (rc == SOCKET_ERROR)
                            {
                                WS_ERROR("send failed with error code:", WSAGetLastError());
                            }
                        }
                    }
                    else if (rc == 0)
                    {
                        WS_LOG("Connection closed!");
                    }
                    else
                    {
                        WS_ERROR("recv failed with error code:", WSAGetLastError());
                    }
                } while (rc > 0);

                WS_LOG("Disconnect the client");
                closesocket(client);
            };
            std::thread t_handler(f_handler, soc_client);
            if (t_handler.joinable())
            {
                t_handler.detach();
            }
        }
        
        closesocket(soc_listen);
    }

    void UDP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::SYNC_UDP_SERVER);
        soc_listen_info.sin_addr.s_addr = inet_addr(serverIP);

        SOCKET soc_listen = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (soc_listen == INVALID_SOCKET)
        {
            WS_ERROR("socket failed with error code:", WSAGetLastError());
            return;
        }

        rc = bind(soc_listen, (SOCKADDR*)&soc_listen_info, sizeof(soc_listen_info));
        if (rc)
        {
            WS_ERROR("bind failed with error code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }

        char buffer[1024];
        int buffer_len = 1024;
        while (true)
        {
            memset(buffer, 0, buffer_len);

            sockaddr_in client_info = { 0 };
            int client_info_len = sizeof(client_info);
            rc = recvfrom(soc_listen, buffer, buffer_len, 0, (SOCKADDR*)&client_info, &client_info_len);
            if (rc == SOCKET_ERROR)
            {
                WS_ERROR("recvfrom failed with code:", WSAGetLastError());
                break;
            }

            if (std::string(buffer) == std::string("server_ip_request"))
            {
                memset(buffer, 0, 1024);
                strcpy(buffer, serverIP);
                rc = sendto(soc_listen, buffer, 1024, 0, (SOCKADDR*)&client_info, client_info_len);
                if (rc == SOCKET_ERROR)
                {
                    WS_ERROR("sendto failed with code:", WSAGetLastError());
                    break;
                }
            }
        }

        closesocket(soc_listen);
    }

    void TCP_Client(const std::string& server_ip)
    {
        std::string command;
        while (f_prompt("Enter command: ") && std::getline(std::cin, command))
        {
            if (command == "exit")
            {
                break;
            }
            else if (command == "connect")
            {
                int rc;

                sockaddr_in server_info = { 0 };
                server_info.sin_family = AF_INET;
                server_info.sin_port = htons(PORTS::SYNC_TCP_SERVER);
                server_info.sin_addr.s_addr = inet_addr(server_ip.data());

                SOCKET soc_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (soc_client == INVALID_SOCKET)
                {
                    WS_ERROR("create socket client failed with code:", WSAGetLastError(), CLIENT);
                    return;
                }

                rc = connect(soc_client, (SOCKADDR*)&server_info, sizeof(server_info));
                if (rc)
                {
                    WS_ERROR("connect failed with code:", WSAGetLastError(), CLIENT);
                    closesocket(soc_client);
                    return;
                }

                auto f_client =
                    [=](SOCKET client)
                {
                    std::string request;
                    while (f_prompt("Enter message: ") && std::getline(std::cin, request))
                    {
                        int rc = send(soc_client, request.data(), request.length(), 0);
                        if (rc == SOCKET_ERROR)
                        {
                            WS_ERROR("send failed with error code:", WSAGetLastError(), CLIENT);
                            closesocket(soc_client);
                            return;
                        }

                        char buffer[1024];
                        memset(buffer, '\0', 1024);
                        rc = recv(soc_client, buffer, 1024, 0);
                        if (rc > 0)
                        {
                            WS_LOG(buffer);
                        }
                        else if (rc == 0)
                        {
                            WS_LOG("Connection closed!", CLIENT);
                        }
                        else
                        {
                            WS_ERROR("Receive failed with error code:", WSAGetLastError(), CLIENT);
                            break;
                        }

                        if (request == "disconnect" || request == "d")
                        {
                            break;
                        }
                    }
                };
                std::thread t_client(f_client, soc_client);
                if (t_client.joinable())
                {
                    t_client.join();
                }

                rc = shutdown(soc_client, SD_BOTH);
                if (rc == SOCKET_ERROR)
                {
                    WS_ERROR("shutdown failed with error code:", WSAGetLastError(), CLIENT);
                    closesocket(soc_client);
                    return;
                }

                closesocket(soc_client);
            }
            else
            {
                WS_LOG("Invalid command", CLIENT);
            }
        }
    }

    void UDP_Client(std::promise<std::string>& p_server_ip)
    {
        int rc;

        sockaddr_in server_info = { 0 };
        server_info.sin_family = AF_INET;
        server_info.sin_port = htons(PORTS::SYNC_UDP_SERVER);
        server_info.sin_addr.s_addr = inet_addr("255.255.255.255");

        SOCKET soc_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (soc_client == INVALID_SOCKET)
        {
            WS_ERROR("create socket failed with code:", WSAGetLastError(), CLIENT);
            return;
        }

        BOOL b_broadcast = TRUE;
        setsockopt(soc_client, SOL_SOCKET, SO_BROADCAST, (char*)&b_broadcast, sizeof(BOOL));

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "server_ip_request");

        rc = sendto(soc_client, buffer, sizeof(buffer), 0, (SOCKADDR*)&server_info, sizeof(server_info));
        if (rc == SOCKET_ERROR)
        {
            WS_ERROR("sendto failed with code:", WSAGetLastError(), CLIENT);
            closesocket(soc_client);
            return;
        }

        memset(buffer, 0, 1024);
        int server_info_len = sizeof(server_info);
        rc = recvfrom(soc_client, buffer, 1024, 0, (SOCKADDR*)&server_info, &server_info_len);
        if (rc == SOCKET_ERROR)
        {
            WS_ERROR("recvfrom failed with code:", WSAGetLastError(), CLIENT);
            closesocket(soc_client);
            return;
        }

        f_prompt("TCP server IP: ");
        f_prompt(buffer);
        f_prompt("\n");

        p_server_ip.set_value(buffer);

        closesocket(soc_client);
    }
}

namespace ClientServerApplication_Async
{
    auto f_prompt = [](const std::string& prompt) -> bool { std::cout << prompt; return true; };

    void TCP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::ASYNC_TCP_SERVER);
        soc_listen_info.sin_addr.s_addr = inet_addr(serverIP);

        SOCKET soc_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (soc_listen == INVALID_SOCKET)
        {
            WS_ERROR("socket failed with error code:", WSAGetLastError());
            return;
        }

        unsigned long io_non_block_mode = 1;
        rc = ioctlsocket(soc_listen, FIONBIO, (unsigned long*)&io_non_block_mode);
        if (rc == SOCKET_ERROR)
        {
            WS_ERROR("set non-block mode failed with code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }

        rc = bind(soc_listen, (SOCKADDR*)&soc_listen_info, sizeof(soc_listen_info));
        if (rc)
        {
            WS_ERROR("bind failed with error code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }

        rc = listen(soc_listen, 8);
        if (rc == SOCKET_ERROR)
        {
            WS_ERROR("listen failed with code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }

        fd_set fds_read;
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        while (true)
        {
            FD_ZERO(&fds_read);
            FD_SET(soc_listen, &fds_read);

            rc = select(0, &fds_read, NULL, NULL, &timeout);
            if (rc == SOCKET_ERROR)
            {
                WS_ERROR("select failed with code:", WSAGetLastError());
            }
            else if (rc == 0)
            {
                //WS_LOG("Timeout while waiting for client connection");
            }
            else if (rc > 0)
            {
                // check to see if the socket is still part of read set
                if (FD_ISSET(soc_listen, &fds_read))
                {
                    WS_LOG("A read event has occurred on client connected socket");
                }
            }
        }
    }

    void UDP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::ASYNC_UDP_SERVER);
        soc_listen_info.sin_addr.s_addr = inet_addr(serverIP);

        SOCKET soc_listen = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (soc_listen == INVALID_SOCKET)
        {
            WS_ERROR("create udp socket failed with code:", WSAGetLastError());
            return;
        }

        rc = bind(soc_listen, (SOCKADDR*)&soc_listen_info, sizeof(soc_listen_info));
        if (rc)
        {
            WS_ERROR("bind failed with error code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }

        char buffer[1024];
        int buffer_len = 1024;
        while (true)
        {
            memset(buffer, 0, buffer_len);

            sockaddr_in client_info = { 0 };
            int client_info_len = sizeof(client_info);
            rc = recvfrom(soc_listen, buffer, buffer_len, 0, (SOCKADDR*)&client_info, &client_info_len);
            if (rc == SOCKET_ERROR)
            {
                WS_ERROR("recvfrom failed with code:", WSAGetLastError());
                break;
            }

            if (std::string(buffer) == std::string("server_ip_request"))
            {
                memset(buffer, 0, 1024);
                strcpy(buffer, serverIP);
                rc = sendto(soc_listen, buffer, 1024, 0, (SOCKADDR*)&client_info, client_info_len);
                if (rc == SOCKET_ERROR)
                {
                    WS_ERROR("sendto failed with code:", WSAGetLastError());
                    break;
                }
            }
        }

        closesocket(soc_listen);
    }

    void TCP_Client(const std::string& server_ip)
    {
        std::string command;
        while (f_prompt("Enter command: ") && std::getline(std::cin, command))
        {
            if (command == "exit")
            {
                break;
            }
            else if (command == "connect")
            {
                int rc;

                sockaddr_in server_info = { 0 };
                server_info.sin_family = AF_INET;
                server_info.sin_port = htons(PORTS::ASYNC_TCP_SERVER);
                server_info.sin_addr.s_addr = inet_addr(server_ip.data());

                int server_info_len = sizeof(server_info);

                SOCKET soc_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (soc_client == INVALID_SOCKET)
                {
                    WS_ERROR("create socket client failed with code:", WSAGetLastError(), CLIENT);
                    return;
                }

                rc = connect(soc_client, (SOCKADDR*)&server_info, server_info_len);
                if (rc == SOCKET_ERROR)
                {
                    WS_ERROR("connect failed with code:", WSAGetLastError(), CLIENT);
                    closesocket(soc_client);
                    return;
                }

                for (int i = 0; i < 10; i++)
                {
                    char buffer[1024];
                    int buffer_len = 1024;
                    memset(buffer, 0, buffer_len);
                    strcpy(buffer, "request_data_from_client");
                    rc = send(soc_client, buffer, buffer_len, 0);
                    if (rc == SOCKET_ERROR)
                    {
                        WS_LOG("send failed with code:", WSAGetLastError(), CLIENT);
                        break;
                    }

                    memset(buffer, 0, buffer_len);
                    rc = recv(soc_client, buffer, buffer_len, 0);
                    if (rc == SOCKET_ERROR)
                    {
                        WS_LOG("recv failed with code:", WSAGetLastError(), CLIENT);
                        break;
                    }

                    buffer[rc] = '\0';
                    f_prompt("Received data from server: ");
                    f_prompt(buffer);
                    f_prompt("\n");
                }

                closesocket(soc_client);
            }
            else
            {
                WS_LOG("Invalid command", CLIENT);
            }
        }
    }

    void UDP_Client(std::promise<std::string>& p_server_ip)
    {
        int rc;

        sockaddr_in server_info = { 0 };
        server_info.sin_family = AF_INET;
        server_info.sin_port = htons(PORTS::ASYNC_UDP_SERVER);
        server_info.sin_addr.s_addr = inet_addr("255.255.255.255");

        SOCKET soc_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (soc_client == INVALID_SOCKET)
        {
            WS_ERROR("create socket failed with code:", WSAGetLastError(), CLIENT);
            return;
        }

        BOOL b_broadcast = TRUE;
        setsockopt(soc_client, SOL_SOCKET, SO_BROADCAST, (char*)&b_broadcast, sizeof(BOOL));

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "server_ip_request");

        rc = sendto(soc_client, buffer, sizeof(buffer), 0, (SOCKADDR*)&server_info, sizeof(server_info));
        if (rc == SOCKET_ERROR)
        {
            WS_ERROR("sendto failed with code:", WSAGetLastError(), CLIENT);
            closesocket(soc_client);
            return;
        }

        memset(buffer, 0, 1024);
        int server_info_len = sizeof(server_info);
        rc = recvfrom(soc_client, buffer, 1024, 0, (SOCKADDR*)&server_info, &server_info_len);
        if (rc == SOCKET_ERROR)
        {
            WS_ERROR("recvfrom failed with code:", WSAGetLastError(), CLIENT);
            closesocket(soc_client);
            return;
        }

        f_prompt("TCP server IP: ");
        f_prompt(buffer);
        f_prompt("\n");

        p_server_ip.set_value(buffer);

        closesocket(soc_client);
    }
}

#endif