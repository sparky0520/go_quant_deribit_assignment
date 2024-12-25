#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

// Callback function for writing received data to a string
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *out)
{
    size_t totalSize = size * nmemb;
    out->append((char *)contents, totalSize);
    return totalSize;
}

int main()
{
    CURL *curl;
    CURLcode res;

    // Initialize curl
    curl = curl_easy_init();
    if (curl)
    {
        std::string getReadBuffer;

        // Set the callback function to write the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Set the URL
        // curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2/public/get_order_book?depth=10&instrument_name=BNB_USDC");
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2/private/buy?amount=100&instrument_name=BNB_USDC&type=market");

        // Pass the string to store the data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &getReadBuffer);

        std::string token = "1735117023861.1e7-K4d0.8FVJAZFIgfZA8PUEW27WwZUB7YABKB72ZFDDp8XSulzTvWiuV4U-La3y1RawGzvxRNMO2Mmjiup5fmB9-uTg-o5Myvi-Jx-7LdmR2FsLbYuyEVcWS6nUB7O2M51VT9perkES8F-AZNaAxUAg7Gc9x71JyGqbQIDnnxqTuCv8DOk6bXjBfEflPCCr85EqCXTw8h3NMnh_VMFi8ByrxUErkMEWSAtR20B_SKiKuKq81hCdFhVoN9_p50sbo1VVX2TjKt14oQ-W7eFCwn32k37F3YzEjFegfpUqA2zLsLEIqpIhiOfOpkdWP6wJRsVpMUEj1FtnM-g-3nizZd6ZHGcsWxrq-_xR";
        // set getHeaders
        struct curl_slist *getHeaders = nullptr;
        getHeaders = curl_slist_append(getHeaders, ("Access-Token: Bearer " + token).c_str());
        getHeaders = curl_slist_append(getHeaders, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, getHeaders);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << "response:\n"
                      << getReadBuffer << std::endl;
            // json jsonData = json::parse(getReadBuffer);
            // std::cout << "jsonrpc: " << jsonData["jsonrpc"] << std::endl;
            // std::cout << "Top Bid Price: " << jsonData["result"]["bids"][0][0] << std::endl;
            // std::cout << "Top Bid Contestants: " << jsonData["result"]["bids"][0][1] << std::endl;
            // std::cout << "timetaken: " << ((size_t)jsonData["usOut"] - (size_t)jsonData["usIn"]) << " microseconds" << std::endl;
        }

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(getHeaders);
    }
    else
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }

    return 0;
}
