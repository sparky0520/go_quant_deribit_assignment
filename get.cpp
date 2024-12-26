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
        std::string readBuffer;

        // Set the callback function to write the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://test.deribit.com/api/v2/public/get_order_book?depth=10&instrument_name=BNB_USDC");

        // Pass the string to store the data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            json jsonData = json::parse(readBuffer);
            std::cout << "jsonrpc: " << jsonData["jsonrpc"] << std::endl;
            std::cout << "Top Bid Price: " << jsonData["result"]["bids"][0][0] << std::endl;
            std::cout << "Top Bid Contestants: " << jsonData["result"]["bids"][0][1] << std::endl;
            std::cout << "timetaken: " << ((size_t)jsonData["usOut"] - (size_t)jsonData["usIn"]) << " microseconds" << std::endl;
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }

    return 0;
}