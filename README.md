# http.cpp

Making HTTP Requests using C++ on the deribit api to make transactions on the deribit test network.

### dependencies installation - Linux

`sudo apt-get update`
`sudo apt-get install libcurl4-openssl-dev g++`

### compile

`g++ http.cpp -o http -lcurl -std=c++17`

### run

`./http`

# websocket.cpp

Establishing a websocket client to connect to the deribit testnet api and fetch realtime updates.

### dependencies installation - Linux

`sudo apt update`
`sudo apt install libboost-all-dev libssl-dev g++`

### compile

`g++ websocket.cpp -o websocket -std=c++17 -lcurl -lssl -lcrypto -lboost_system -lboost_json -pthread`

### run

`./websocket`
