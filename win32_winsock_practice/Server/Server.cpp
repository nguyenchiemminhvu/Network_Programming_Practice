#include "../shared.h"

int main()
{
    int rc = 0;
    WSADATA server_data;
    rc = WSAStartup(MAKEWORD(2, 2), &server_data);
    if (rc)
    {
        WS_ERROR("start up failed with error code:", WSAGetLastError());
        return -1;
    }

    char hostname[128];
    memset(hostname, '\0', 128);
    gethostname(hostname, 128);
    hostent * h = gethostbyname(hostname);
    int i = 0;
    while (h && h->h_addr_list && h->h_addr_list[i])
    {
        in_addr add;
        add.S_un.S_addr = *(u_long*)h->h_addr_list[i++];
        std::cout << "Server's IP Address: " << inet_ntoa(add) << std::endl;
    }

    std::thread udp_server(ClientServer_IOCP_Model::UDP_Server);
    std::thread tcp_server(ClientServer_IOCP_Model::TCP_Server);

    if (udp_server.joinable())
    {
        udp_server.join();
    }

    if (tcp_server.joinable())
    {
        tcp_server.join();
    }

    WSACleanup();
    return 0;
}
