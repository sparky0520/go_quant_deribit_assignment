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
    params["instrument"] = "BTC-24AUG18-6500-P";
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
            }));

        ws.handshake("test.deribit.com", "/ws/api/v2");
        std::cout << "Connected to websocket server\n";

               // Create and send JSON-RPC request
        std::string request = create_jsonrpc_request();
        ws.write(net::buffer(request));
        std::cout << "Sent request: " << request << std::endl;

        // Receive response
        beast::flat_buffer buffer;
        ws.read(buffer);
        std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;

        ws.close(websocket::close_code::normal);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}