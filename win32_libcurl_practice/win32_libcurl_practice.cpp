#define CURL_STATICLIB

#include <curl/curl.h>
#include <iostream>

#if _WIN64
    #ifdef _DEBUG
        #pragma comment(lib, "libcurl/lib_x64_debug/libcurl_a_debug.lib")
    #else
        #pragma comment(lib, "libcurl/lib_x64_release/libcurl_a.lib")
    #endif
#elif _WIN32
    #ifdef _DEBUG
        #pragma comment(lib, "libcurl/lib_x32_debug/libcurl_a_debug.lib")
    #else
        #pragma comment(lib, "libcurl/lib_x32_release/libcurl_a.lib")
    #endif
#endif

#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "advapi32.lib")

using namespace std;

int main()
{
    CURL* curl = NULL;
    curl = curl_easy_init();



    curl_easy_cleanup(curl);

    return 0;
}
