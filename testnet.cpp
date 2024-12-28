#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
namespace json = boost::json;
using tcp = net::ip::tcp;

std::string create_jsonrpc_request()
{
    json::object request;
    request["jsonrpc"] = "2.0";
    request["id"] = 8666;
    request["method"] = "public/ticker";

    json::object params;
    params["instrument"] = "ETH-PERPETUAL";
    request["params"] = params;

    return json::serialize(request);
}

int main()
{
    try
    {
        net::io_context ioc;
        ssl::context ctx{ssl::context::tlsv12_client};
        ctx.set_verify_mode(ssl::verify_none);

        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("test.deribit.com", "443");

        websocket::stream<beast::ssl_stream<tcp::socket>> ws{ioc, ctx};

        if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), "test.deribit.com"))
            throw beast::system_error(
                beast::error_code(
                    static_cast<int>(::ERR_get_error()),
                    net::error::get_ssl_category()),
                "Failed to set SNI Hostname");

        net::connect(ws.next_layer().next_layer(), results.begin(), results.end());
        ws.next_layer().handshake(ssl::stream_base::client);

        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type &req)
            {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) + " jsonrpc-websocket-client");
                // req.set(http::field::content_type, "application/json");
            }));

        ws.handshake("test.deribit.com", "/ws/api/v2");
        std::cout << "Connected to websocket server\n";

        // Create and send JSON-RPC request
        std::string request = create_jsonrpc_request();
        int flag;
        while (1)
        {
            std::cout << "Enter 0 to send empty message\nEnter 1 to send test message\nEnter 2 to send custom message\nDefault - Exit Program ";
            std::cin >> flag;
            switch (flag)
            {
            case 0:
                request = "";
                break;

            case 1:
                break;

            case 2:
                std::cin >> request;
                break;

            default:
                return 0;
            }
            ws.write(net::buffer(request));
            std::cout << "Sent request: " << request << std::endl;

            // Receive response
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::cout << "Received: " << beast::make_printable(buffer.data()) << '\n';
        }

        ws.close(websocket::close_code::normal);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// {"method": "public/get_order_book","params": {"instrument_name": "BNB_USDC"},"jsonrpc": "2.0","id": 6}