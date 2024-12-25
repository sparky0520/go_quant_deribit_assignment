#include <iostream>
#include <string>
#include <curl/curl.h>

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

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://jsonplaceholder.typicode.com/posts/1");

        // Set the callback function to write the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

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
            std::cout << "Response: " << readBuffer << std::endl;
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
