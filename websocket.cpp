// References:
// https://docs.deribit.com/#json-rpc-over-websocket
// https://docs.deribit.com/#subscription-management
// https://docs.deribit.com/#private-enable_cancel_on_disconnect

#include <string>
#include <boost/json.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <iostream>
#include <thread>

namespace beast = boost::beast;
namespace json = boost::json;
namespace net = boost::asio;
namespace ssl = net::ssl;
namespace websocket = beast::websocket;
namespace http = beast::http;
using tcp = net::ip::tcp;

std::string DOMAIN_NAME = "test.deribit.com";
std::string PORT = "443";
std::string TARGET_ROUTE = "/ws/api/v2";

enum method
{
    // Possible methodEnum args in get_subscription_handle_request function. Replace _ with / except in unsubscribe_all
    public_subscribe,
    public_unsubscribe,
    private_subscribe,
    private_unsubscribe,
    public_unsubscribe_all,
    private_unsubscribe_all,
};

std::string
get_auth_request()
{
    json::object request;
    request["jsonrpc"] = "2.0";
    request["id"] = 9929;
    request["method"] = "public/auth";

    json::object params;
    params["grant_type"] = "client_credentials";
    params["client_id"] = "_dgkV8if";
    params["client_secret"] = "uQz3bW4weUKnsFiveUO1d85qIy6QJriVyMbbfkoNFaM";

    request["params"] = params;
    return json::serialize(request);
}

std::string subscription_handle_request(const int methodEnum, const char *channel_str, websocket::stream<beast::ssl_stream<tcp::socket>> &ws)
{
    const char *method_str;
    switch (methodEnum)
    {
    case 0:
        method_str = "public/subscribe";
        break;
    case 1:
        method_str = "public/unsubscribe";
        break;
    case 2:
        method_str = "public/unsubscribe_all";
        break;
    case 3:
        method_str = "private/subscribe";
        break;
    case 4:
        method_str = "private/unsubscribe";
        break;
    case 5:
        method_str = "private/unsubscribe_all";
        break;
    default:
        std::cout << "Invalid input\n";
        break;
    }

    json::object request;
    request["method"] = method_str;
    request["jsonrpc"] = "2.0";
    request["id"] = 2;

    json::object params;
    json::array channels;
    channels.push_back(channel_str);
    params["channels"] = channels;
    request["params"] = params;

    std::string subreq = json::serialize(request);
    ws.write(net::buffer(subreq));
    std::cout << "Sent request: " << subreq << '\n';

    // receiving response
    beast::flat_buffer buffer;
    while (1)
    {
        ws.read(buffer);
        std::cout << "Received: " << beast::make_printable(buffer.data()) << "\n\n\n";
    }
}

int main()
{
    try
    {
        net::io_context ioc;
        ssl::context ctx{ssl::context::tlsv12_client};
        ctx.set_verify_mode(ssl::verify_none);

        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(DOMAIN_NAME, PORT);

        websocket::stream<beast::ssl_stream<tcp::socket>> ws{ioc, ctx};

        if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), DOMAIN_NAME.c_str()))
            throw beast::system_error(
                beast::error_code(
                    static_cast<int>(::ERR_get_error()),
                    net::error::get_ssl_category()),
                "Failed to set SNI Hostname");

        net::connect(ws.next_layer().next_layer(), results.begin(), results.end());
        ws.next_layer().handshake(ssl::stream_base::client); // ssl handshake

        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type &req)
            {
                req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " jsonrpc-websocket-client");
            }));

        ws.handshake(DOMAIN_NAME, TARGET_ROUTE); // ws handshake
        std::cout << "Connected to websocket server\n";

        // sending auth request
        std::string auth_string = get_auth_request();
        ws.write(net::buffer(auth_string));

        // receiving response
        beast::flat_buffer buffer;
        ws.read(buffer);
        std::cout << "Received: " << beast::make_printable(buffer.data()) << '\n';

        // event loop - public/subscribe and public/unsubscribe for broadcasting
        int flag;
        std::string channel_str;

        // receiving response
        beast::flat_buffer subBuffer;

        // for (int i = 0; i < 1000000; i++)
        // {
        //     ws.read(subBuffer);
        //     std::cout << "Received: " << beast::make_printable(subBuffer.data()) << '\n';
        // }

        while (1)
        {
            std::cout << "Websocket subscription menu: \n\n1. Subscibe\n2. Unsubscribe\n0. Exit program.\n\n";
            std::cin >> flag;
            switch (flag)
            {
            case 1:
            {
                std::cout << "Enter full channel string to subscribe to: ";
                std::cin >> channel_str;
                std::thread subscribeThread([channel_str, &ws]() // Spawning a new thread to handle the subscription
                                            { subscription_handle_request(method::public_subscribe, channel_str.c_str(), ws); });
                subscribeThread.detach(); // Detach the thread to run independently
                break;
            }
            case 2:
            {
                std::cout << "Enter full channel string to unsubscribe from: ";
                std::cin >> channel_str;
                std::thread unsubscribeThread([channel_str, &ws]() // Spawn a new thread to handle the unsubscription
                                              { subscription_handle_request(method::public_unsubscribe, channel_str.c_str(), ws); });
                unsubscribeThread.detach(); // Detach the thread to run independently
                break;
            }
            case 0:
                std::cout << "Gracefully closing connection\n";
                ws.close(websocket::close_code::normal);
                std::cout << "Exiting program\n";
                return 0;
                break;
            default:
                std::cout << "Invalid input\n";
                break;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// {
//   "method": "public/subscribe",
//   "params": {
//     "channels": [
//       "ticker.BNB_USDC.raw"
//     ]
//   },
//   "jsonrpc": "2.0",
//   "id": 2
// }

// {
//   "method": "private/subscribe",
//   "params": {
//     "channels": [
//       "deribit_price_index.ada_usd"
//     ]
//   },
//   "jsonrpc": "2.0",
//   "id": 6
// }