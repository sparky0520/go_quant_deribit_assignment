# http.cpp

Making HTTP Requests using C++ on the deribit api to make transactions on the deribit test network.

### compile

``

# websocket.cpp

Establishing a websocket client to connect to the deribit testnet api and fetch realtime updates.

### compile

`g++ websocket.cpp -o websocket -std=c++17 -lcurl -lssl -lcrypto -lboost_system -lboost_json -pthread`

### run

`./websocket`
