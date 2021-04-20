#include "../shared.h"

int main()
{
    int rc = 0;
    WSADATA client_data;
    rc = WSAStartup(MAKEWORD(2, 2), &client_data);
    if (rc)
    {
        WS_ERROR("start up failed with error code:", WSAGetLastError());
        return -1;
    }

    std::promise<std::string> p_server_ip;
    std::future<std::string> f_server_ip = p_server_ip.get_future();
    std::thread udp_client(ClientServer_AsyncSelectModel::UDP_Client, std::ref(p_server_ip));
    std::string s_server_ip = f_server_ip.get();
    if (udp_client.joinable())
    {
        udp_client.join();
    }
    
    std::thread tcp_client(ClientServer_AsyncSelectModel::TCP_Client, s_server_ip);
    if (tcp_client.joinable())
    {
        tcp_client.join();
    }

    WSACleanup();

    return 0;
}