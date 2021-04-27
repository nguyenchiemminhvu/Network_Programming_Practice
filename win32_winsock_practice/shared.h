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

#define SAFE_DELETE(p) do { if (p) delete p; p = NULL; } while(p)

enum ApplicationType
{
    SERVER,
    CLIENT,

    UNKNOWN
};

enum PORTS
{
    TCP_SERVER = 27015,
    UDP_SERVER = 27017,
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

namespace ClientServer_BlockingModel
{
    auto f_prompt = [](const std::string& prompt) -> bool { std::cout << prompt; return true; };

    void TCP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::TCP_SERVER);
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
        soc_listen_info.sin_port = htons(PORTS::UDP_SERVER);
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
                server_info.sin_port = htons(PORTS::TCP_SERVER);
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
        server_info.sin_port = htons(PORTS::UDP_SERVER);
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

namespace ClientServer_SelectModel
{
    auto f_prompt = [](const std::string& prompt) -> bool { std::cout << prompt; return true; };

    class SOCKET_INFO
    {
    public:
        SOCKET socket;
        char buffer[1024];
        DWORD byte_recv;

        SOCKET_INFO(SOCKET s)
        {
            socket = s;
            byte_recv = 0;
            memset(buffer, 0, 1024);
        }

        void ResetData()
        {
            memset(buffer, 0, 1024);
            byte_recv = 0;
        }
    };

    void TCP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::TCP_SERVER);
        soc_listen_info.sin_addr.s_addr = inet_addr(serverIP);

        SOCKET soc_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (soc_listen == INVALID_SOCKET)
        {
            WS_ERROR("socket failed with error code:", WSAGetLastError());
            return;
        }

        unsigned long io_non_block_mode = 1;
        rc = ioctlsocket(soc_listen, FIONBIO, &io_non_block_mode);
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

        std::vector<SOCKET_INFO*> connected_sockets;

        fd_set fds_read;
        fd_set fds_write;

        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        while (true)
        {
            FD_ZERO(&fds_read);
            FD_ZERO(&fds_write);

            FD_SET(soc_listen, &fds_read);

            for (int i = 0; i < FD_SETSIZE && i < connected_sockets.size(); i++)
            {
                SOCKET_INFO* socket_info = connected_sockets[i];
                if (!socket_info)
                    continue;

                if (socket_info->byte_recv == 0)
                {
                    FD_SET(socket_info->socket, &fds_read);
                }
                else
                {
                    FD_SET(socket_info->socket, &fds_write);
                }
            }

            rc = select(0, &fds_read, NULL, NULL, &timeout);
            if (rc == SOCKET_ERROR)
            {
                WS_ERROR("select failed with code:", WSAGetLastError());
                break;
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
                    SOCKET soc_client = accept(soc_listen, NULL, NULL);
                    if (soc_client == INVALID_SOCKET)
                    {
                        WS_ERROR("accept failed with code:", WSAGetLastError());
                        break;
                    }

                    rc = ioctlsocket(soc_client, FIONBIO, &io_non_block_mode);
                    if (rc == SOCKET_ERROR)
                    {
                        WS_ERROR("set socket non block mode failed with code:", WSAGetLastError());
                        break;
                    }

                    connected_sockets.push_back(new SOCKET_INFO(soc_client));
                }
            }

            for (int i = 0; i < FD_SETSIZE && i < connected_sockets.size(); i++)
            {
                SOCKET_INFO* socket_info = connected_sockets[i];
                if (!socket_info)
                    continue;

                if (FD_ISSET(socket_info->socket, &fds_read))
                {
                    rc = recv(socket_info->socket, socket_info->buffer, 1024, 0);
                    if (rc == SOCKET_ERROR)
                    {
                        if (WSAGetLastError() == WSAEWOULDBLOCK)
                        {
                            WS_ERROR("recv failed with code:", WSAGetLastError());
                            SAFE_DELETE(connected_sockets[i]);
                        }
                        continue;
                    }
                    else
                    {
                        socket_info->byte_recv = rc;
                        if (rc == 0)
                        {
                            SAFE_DELETE(connected_sockets[i]);
                            continue;
                        }
                    }
                }
                
                if (FD_ISSET(socket_info->socket, &fds_write))
                {
                    rc = send(socket_info->socket, socket_info->buffer, 1024, 0);
                    if (rc == SOCKET_ERROR)
                    {
                        if (WSAGetLastError() == WSAEWOULDBLOCK)
                        {
                            WS_ERROR("send failed with code:", WSAGetLastError());
                            SAFE_DELETE(connected_sockets[i]);
                        }
                        continue;
                    }
                    else
                    {
                        socket_info->ResetData();
                    }
                }
            }

            for (int i = 0; i < FD_SETSIZE && i < connected_sockets.size(); )
            {
                if (!connected_sockets[i])
                {
                    connected_sockets.erase(connected_sockets.begin() + i);
                }
                else
                {
                    i++;
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
        soc_listen_info.sin_port = htons(PORTS::UDP_SERVER);
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
                server_info.sin_port = htons(PORTS::TCP_SERVER);
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
        server_info.sin_port = htons(PORTS::UDP_SERVER);
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

namespace ClientServer_AsyncSelectModel
{
#ifndef WM_SOCKET
#define WM_SOCKET (WM_USER + 1)
#endif

    auto f_prompt = [](const std::string& prompt) -> bool { std::cout << prompt; return true; };

    class SOCKET_INFO
    {
    public:
        SOCKET socket;
        char buffer[1024];
        DWORD byte_recv;

        SOCKET_INFO(SOCKET s)
        {
            socket = s;
            byte_recv = 0;
            memset(buffer, 0, 1024);
        }

        void ResetData()
        {
            memset(buffer, 0, 1024);
            byte_recv = 0;
        }
    };

    std::vector<SOCKET_INFO*> connected_sockets;

    LRESULT CALLBACK TCP_Socket_Event_Handler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        int rc = 0;
        int i;

        if (WSAGETSELECTERROR(lParam))
        {
            if (WSAGetLastError())
            {
                WS_ERROR("Error in socket event handler:", WSAGetLastError());
            }
            else
            {
                WS_LOG("Client disconnected:", wParam);
            }
            closesocket((SOCKET)wParam);
            return -1;
        }

        SOCKET_INFO * connected = NULL;

        switch (WSAGETSELECTEVENT(lParam))
        {
        case FD_ACCEPT:
            connected = new SOCKET_INFO(accept((SOCKET)wParam, NULL, NULL));
            if (connected->socket == INVALID_SOCKET)
            {
                WS_ERROR("accept failed with code:", WSAGetLastError());
                return -1;
            }
            WSAAsyncSelect(connected->socket, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
            connected_sockets.push_back(connected);
            break;

        case FD_READ:
            for (i = 0; i < connected_sockets.size(); i++)
            {
                if (wParam == connected_sockets[i]->socket)
                {
                    connected = connected_sockets[i];
                    break;
                }
            }
            
            if (connected->byte_recv)
            {
                return 0;
            }
            
            rc = recv(connected->socket, connected->buffer, 1024, 0);
            if (rc == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    WS_ERROR("recv failed with code:", WSAGetLastError());
                    closesocket((SOCKET)wParam);
                    SAFE_DELETE(connected_sockets[i]);
                    connected_sockets.erase(connected_sockets.begin() + i);
                    return 0;
                }
            }
            else
            {
                connected->byte_recv = rc;
                PostMessage(hwnd, WM_SOCKET, wParam, FD_WRITE);
            }

            break;

        case FD_WRITE:
            for (i = 0; i < connected_sockets.size(); i++)
            {
                if (wParam == connected_sockets[i]->socket)
                {
                    connected = connected_sockets[i];
                    break;
                }
            }

            if (connected->byte_recv == 0)
                return 0;
            
            rc = send(connected->socket, connected->buffer, 1024, 0);
            if (rc == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    closesocket((SOCKET)wParam);
                    SAFE_DELETE(connected_sockets[i]);
                    connected_sockets.erase(connected_sockets.begin() + i);
                    return 0;
                }
            }
            else
            {
                connected_sockets[i]->ResetData();
                PostMessage(hwnd, WM_SOCKET, wParam, FD_READ);
            }

            break;

        case FD_CLOSE:
            WS_LOG("close socket", wParam);
            for (i = 0; i < connected_sockets.size(); )
            {
                if (connected_sockets[i]->socket == wParam)
                {
                    closesocket(connected_sockets[i]->socket);
                    SAFE_DELETE(connected_sockets[i]);
                    connected_sockets.erase(connected_sockets.begin() + i);
                }
                else
                {
                    i++;
                }
            }
            break;

        default:
            break;
        }

        return 0;
    }

    LRESULT CALLBACK TCP_Server_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_SOCKET: 
            return TCP_Socket_Event_Handler(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    HWND CreateServerWindow(void)
    {
        WNDCLASS cls;
        cls.style = CS_HREDRAW | CS_VREDRAW;
        cls.lpfnWndProc = (WNDPROC)TCP_Server_Proc;
        cls.cbClsExtra = 0;
        cls.cbWndExtra = 0;
        cls.hInstance = NULL;
        cls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        cls.hCursor = LoadCursor(NULL, IDC_ARROW);
        cls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        cls.lpszMenuName = NULL;
        cls.lpszClassName = L"Async Select Model";

        if (RegisterClass(&cls) == 0)
        {
            WS_ERROR("register wnd class failed with code:", GetLastError());
            return NULL;
        }

        HWND window = CreateWindow(
            L"Async Select Model", 
            L"", 
            WS_OVERLAPPEDWINDOW, 
            CW_USEDEFAULT, 
            CW_USEDEFAULT, 
            CW_USEDEFAULT, 
            CW_USEDEFAULT, 
            NULL, 
            NULL, 
            NULL, 
            NULL
        );

        if (window == NULL)
        {
            WS_ERROR("Create HWND failed with code:", GetLastError());
        }

        return window;
    }

    void TCP_Server()
    {
        HWND soc_window = CreateServerWindow();
        if (!soc_window)
        {
            return;
        }

        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::TCP_SERVER);
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

        rc = WSAAsyncSelect(soc_listen, soc_window, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
        if (rc)
        {
            WS_ERROR("Async select failed with code:", WSAGetLastError());
            return;
        }

        rc = listen(soc_listen, 8);
        if (rc == SOCKET_ERROR)
        {
            WS_ERROR("listen failed with code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }

        MSG msg;
        while (rc = GetMessage(&msg, NULL, 0, 0))
        {
            if (rc == -1)
            {
                WS_ERROR("Get message failed with code:", GetLastError());
                return;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void UDP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::UDP_SERVER);
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
                server_info.sin_port = htons(PORTS::TCP_SERVER);
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
        server_info.sin_port = htons(PORTS::UDP_SERVER);
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

namespace ClientServer_EventSelectModel
{
    auto f_prompt = [](const std::string& prompt) -> bool { std::cout << prompt; return true; };

    class SOCKET_INFO
    {
    public:
        SOCKET socket;
        char buffer[1024];
        DWORD byte_recv;

        SOCKET_INFO(SOCKET s)
        {
            socket = s;
            byte_recv = 0;
            memset(buffer, 0, 1024);
        }

        void ResetData()
        {
            memset(buffer, 0, 1024);
            byte_recv = 0;
        }
    };

    void TCP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::TCP_SERVER);
        soc_listen_info.sin_addr.s_addr = inet_addr(serverIP);

        SOCKET soc_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (soc_listen == INVALID_SOCKET)
        {
            WS_ERROR("socket failed with error code:", WSAGetLastError());
            return;
        }

        unsigned long io_non_block_mode = 1;
        rc = ioctlsocket(soc_listen, FIONBIO, &io_non_block_mode);
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

        WSANETWORKEVENTS network_event;
        std::vector<WSAEVENT> socket_events;
        std::vector<SOCKET_INFO*> connected_sockets;

        WSAEVENT event_listen = WSACreateEvent();
        WSAEventSelect(soc_listen, event_listen, FD_ACCEPT | FD_CLOSE);
        
        socket_events.push_back(event_listen);
        connected_sockets.push_back(new SOCKET_INFO(soc_listen));

        rc = listen(soc_listen, 8);
        if (rc == SOCKET_ERROR)
        {
            WS_ERROR("listen failed with code:", WSAGetLastError());
            closesocket(soc_listen);
            return;
        }
        
        while (true)
        {
            int rc = WSAWaitForMultipleEvents(socket_events.size(), (HANDLE*)&socket_events[0], FALSE, WSA_INFINITE, FALSE);
            if (rc == WSA_WAIT_FAILED)
            {
                WS_ERROR("Wait for socket events failed with code:", WSAGetLastError());
                return;
            }

            int idx = rc - WSA_WAIT_EVENT_0;

            rc = WSAEnumNetworkEvents(connected_sockets[idx]->socket, socket_events[idx], &network_event);
            if (rc == SOCKET_ERROR)
            {
                WS_ERROR("WSAEnumNetworkEvents failed with code:", WSAGetLastError());
                return;
            }

            if (network_event.lNetworkEvents & FD_ACCEPT)
            {
                if (network_event.iErrorCode[FD_ACCEPT_BIT])
                {
                    WS_ERROR("FD_ACCEPT failed with code:", network_event.iErrorCode[FD_ACCEPT_BIT]);
                    break;
                }

                SOCKET s = accept(connected_sockets[idx]->socket, NULL, NULL);
                if (connected_sockets.size() >= WSA_MAXIMUM_WAIT_EVENTS)
                {
                    WS_ERROR("Too many connections");
                    closesocket(s);
                    continue;
                }

                WSAEVENT e = WSACreateEvent();
                if (e == WSA_INVALID_EVENT)
                {
                    WS_ERROR("WSACreateEvent failed with code:", WSAGetLastError());
                    WSACloseEvent((HANDLE)e);
                    closesocket(s);
                    continue;
                }

                socket_events.push_back(e);
                connected_sockets.push_back(new SOCKET_INFO(s));

                WSAEventSelect(connected_sockets.back()->socket, socket_events.back(), FD_READ | FD_WRITE | FD_CLOSE);
            }

            if (network_event.lNetworkEvents & FD_READ || network_event.lNetworkEvents & FD_WRITE)
            {
                if (network_event.lNetworkEvents & FD_READ && network_event.iErrorCode[FD_READ_BIT])
                {
                    WS_ERROR("FD_READ failed with code:", network_event.iErrorCode[FD_READ_BIT]);
                    closesocket(connected_sockets[idx]->socket);
                    SAFE_DELETE(connected_sockets[idx]);
                }

                if (network_event.lNetworkEvents & FD_WRITE && network_event.iErrorCode[FD_WRITE_BIT])
                {
                    WS_ERROR("FD_WRITE failed with code:", network_event.iErrorCode[FD_WRITE_BIT]);
                    closesocket(connected_sockets[idx]->socket);
                    SAFE_DELETE(connected_sockets[idx]);
                }

                SOCKET_INFO* soc_info = connected_sockets[idx];
                if (soc_info)
                {
                    if (soc_info->byte_recv == 0)
                    {
                        rc = recv(soc_info->socket, soc_info->buffer, 1024, 0);
                        if (rc == SOCKET_ERROR)
                        {
                            if (WSAGetLastError() != WSAEWOULDBLOCK)
                            {
                                WS_ERROR("send failed with code:", WSAGetLastError());
                            }
                            continue;
                        }

                        soc_info->byte_recv = rc;
                    }

                    if (soc_info->byte_recv > 0)
                    {
                        rc = send(soc_info->socket, soc_info->buffer, 1024, 0);
                        if (rc == SOCKET_ERROR)
                        {
                            WS_ERROR("send failed with code:", WSAGetLastError());
                            continue;
                        }

                        soc_info->ResetData();
                    }
                }
            }

            if (network_event.lNetworkEvents & FD_CLOSE)
            {
                WS_LOG("Close connection on socket:", connected_sockets[idx]->socket);
                if (network_event.iErrorCode[FD_CLOSE_BIT])
                {
                    WS_ERROR("FD_CLOSE failed with code:", network_event.iErrorCode[FD_CLOSE_BIT]);
                    continue;
                }

                closesocket(connected_sockets[idx]->socket);
                WSACloseEvent(socket_events[idx]);
                SAFE_DELETE(connected_sockets[idx]);
                connected_sockets.erase(connected_sockets.begin() + idx);
                socket_events.erase(socket_events.begin() + idx);
            }

            for (int i = 0; i < connected_sockets.size(); )
            {
                if (connected_sockets[i] == NULL)
                {
                    connected_sockets.erase(connected_sockets.begin() + i);
                    WSACloseEvent(socket_events[i]);
                    socket_events.erase(socket_events.begin() + i);
                }
                else
                {
                    i++;
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
        soc_listen_info.sin_port = htons(PORTS::UDP_SERVER);
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
                server_info.sin_port = htons(PORTS::TCP_SERVER);
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
        server_info.sin_port = htons(PORTS::UDP_SERVER);
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

namespace ClientServer_OverlappedModel
{
    auto f_prompt = [](const std::string& prompt) -> bool { std::cout << prompt; return true; };

    class SOCKET_INFO
    {
    public:
        SOCKET socket;
        char buffer[1024];
        DWORD byte_recv;

        SOCKET_INFO(SOCKET s)
        {
            socket = s;
            byte_recv = 0;
            memset(buffer, 0, 1024);
        }

        void ResetData()
        {
            memset(buffer, 0, 1024);
            byte_recv = 0;
        }
    };

    void TCP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::TCP_SERVER);
        soc_listen_info.sin_addr.s_addr = inet_addr(serverIP);

        SOCKET soc_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (soc_listen == INVALID_SOCKET)
        {
            WS_ERROR("socket failed with error code:", WSAGetLastError());
            return;
        }

        unsigned long io_non_block_mode = 1;
        rc = ioctlsocket(soc_listen, FIONBIO, &io_non_block_mode);
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

        
    }

    void UDP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::UDP_SERVER);
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
                server_info.sin_port = htons(PORTS::TCP_SERVER);
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
        server_info.sin_port = htons(PORTS::UDP_SERVER);
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

namespace ClientServer_CompletionPortModel
{
    auto f_prompt = [](const std::string& prompt) -> bool { std::cout << prompt; return true; };

    class SOCKET_INFO
    {
    public:
        SOCKET socket;
        char buffer[1024];
        DWORD byte_recv;

        SOCKET_INFO(SOCKET s)
        {
            socket = s;
            byte_recv = 0;
            memset(buffer, 0, 1024);
        }

        void ResetData()
        {
            memset(buffer, 0, 1024);
            byte_recv = 0;
        }
    };

    void TCP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::TCP_SERVER);
        soc_listen_info.sin_addr.s_addr = inet_addr(serverIP);

        SOCKET soc_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (soc_listen == INVALID_SOCKET)
        {
            WS_ERROR("socket failed with error code:", WSAGetLastError());
            return;
        }

        unsigned long io_non_block_mode = 1;
        rc = ioctlsocket(soc_listen, FIONBIO, &io_non_block_mode);
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

        
    }

    void UDP_Server()
    {
        int rc;

        hostent* he = gethostbyname("");
        char* serverIP = inet_ntoa(*(in_addr*)*he->h_addr_list);

        sockaddr_in soc_listen_info = { 0 };
        soc_listen_info.sin_family = AF_INET;
        soc_listen_info.sin_port = htons(PORTS::UDP_SERVER);
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
                server_info.sin_port = htons(PORTS::TCP_SERVER);
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
        server_info.sin_port = htons(PORTS::UDP_SERVER);
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