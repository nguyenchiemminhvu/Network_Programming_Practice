#include <curl/curl.h>
#include <iostream>

using namespace std;

int main()
{
    CURL* curl = NULL;
    curl = curl_easy_init();



    curl_easy_cleanup(curl);

    return 0;
}
