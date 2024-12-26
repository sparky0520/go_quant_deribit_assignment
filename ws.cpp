#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

int main()
{
    try
    {
        // I/O context for asynchronous operations
        net::io_context ioc;

        // SSL context for secure communications
        ssl::context ctx{ssl::context::tlsv12_client};
        ctx.set_verify_mode(ssl::verify_none); // Note: In production, you should verify certificates

        // TCP resolver for resolving server address
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("ws.postman-echo.com", "443");

        // Websocket stream with SSL
        websocket::stream<beast::ssl_stream<tcp::socket>> ws{ioc, ctx};

        // Set SNI hostname (required for connection)
        if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), "ws.postman-echo.com"))
            throw beast::system_error(
                beast::error_code(
                    static_cast<int>(::ERR_get_error()),
                    net::error::get_ssl_category()),
                "Failed to set SNI Hostname");

        // Connect to the server over TCP
        net::connect(ws.next_layer().next_layer(), results.begin(), results.end());

        // Perform SSL handshake
        ws.next_layer().handshake(ssl::stream_base::client);

        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type &req)
            {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-client-coro");
            }));

        // Perform WebSocket handshake
        ws.handshake("ws.postman-echo.com", "/raw");

        std::cout << "Connected to websocket server\n";

        // Send a message
        std::string message = "Hello! Websocket!";
        ws.write(net::buffer(message));
        std::cout << "Sent: " << message << std::endl;

        // Receive a message
        beast::flat_buffer buffer;
        ws.read(buffer);
        std::cout << "Received: " << beast::make_printable(buffer.data()) << "\n";

        // Close the websocket connection
        ws.close(websocket::close_code::normal);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}