#include "http_utilities.h"

namespace HTTP
{
    // perform a GET request and return the content's length. 
    int get_length(HTTPClient &httpClient, const char *url)
    {
        httpClient.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        httpClient.begin(url);
        // httpClient.addHeader("Cache-Control", "no-cache");
        // httpClient.addHeader("Cache-Control", "max-age=0, private, must-revalidate");
        httpClient.addHeader("Cache-Control", "no-cache, max-age=5");

        const char *headerKeys[]{"Content-Length"};
        httpClient.collectHeaders(headerKeys, 1);
        log_i("GET %s ...", url);
        int responseCode = httpClient.GET();

        if (responseCode != 200)
        {
            log_i("HTTP Error Code: %d", responseCode);
            return -responseCode;
        }
        else
            return httpClient.header("Content-Length").toInt();
    }

    int get_length(HTTPClient &httpClient, const char *path, const char *ext)
    {
        String url{(char *)0}; url.reserve(256); // Heap De-fragmentation
        url += path;
        url += ext;
        return get_length(httpClient, url.c_str());
    }
}