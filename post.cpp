#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

// Posting client id and secret and using it to get response from a protected route.

// Callback function to write received data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *out)
{
    size_t totalSize = size * nmemb;
    out->append((char *)contents, totalSize);
    return totalSize;
}

int main()
{
    CURL *curl;

    // Initialize curl
    curl = curl_easy_init();
    if (curl)
    {
        std::string readBuffer;
        std::string payload = R"({
            "method": "public/auth",
            "params": {
                "grant_type": "client_credentials",
                "client_id": "_dgkV8if",
                "client_secret": "uQz3bW4weUKnsFiveUO1d85qIy6QJriVyMbbfkoNFaM"
            },
            "jsonrpc": "2.0",
            "id": 1
        })";

        // Set the URL for the public auth endpoint
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2");

        // Set callback function to write response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Set variable to store response in
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Set headers
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Flag the request as POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set payload
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Parse the response
            json jsonData = json::parse(readBuffer);
            std::string token = jsonData["result"]["access_token"];
            std::cout << "Access Token: " << token << std::endl;

            // Cleanup curl_slist and curl
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            // Now let's make the protected request using the token

            CURL *getCurl = curl_easy_init(); // Initialize a new cURL handle for the second request
            if (getCurl)
            {
                std::string getReadBuffer;

                // Set the URL for the protected route
                curl_easy_setopt(getCurl, CURLOPT_URL, "https://test.deribit.com/api/v2/private/buy?amount=100&instrument_name=BNB_USDC&type=market");

                // Set callback function to write the response
                curl_easy_setopt(getCurl, CURLOPT_WRITEFUNCTION, WriteCallback);

                // Set variable to store response in
                curl_easy_setopt(getCurl, CURLOPT_WRITEDATA, &getReadBuffer);

                // Set the Authorization header with the token
                struct curl_slist *getHeaders = nullptr;
                getHeaders = curl_slist_append(getHeaders, ("Authorization: Bearer " + token).c_str());
                getHeaders = curl_slist_append(getHeaders, "Content-Type: application/json");
                curl_easy_setopt(getCurl, CURLOPT_HTTPHEADER, getHeaders);

                // Perform the protected request
                res = curl_easy_perform(getCurl);
                if (res != CURLE_OK)
                {
                    std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
                }
                else
                {
                    std::cout << "Protected Route Response: " << getReadBuffer << std::endl;
                }

                // Cleanup getCurl and getHeaders
                curl_easy_cleanup(getCurl);
                curl_slist_free_all(getHeaders);
            }
            else
            {
                std::cerr << "Failed to initialize getCurl" << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }

    return 0;
}
