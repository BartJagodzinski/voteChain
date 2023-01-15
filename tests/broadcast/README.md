g++ main.cpp -o main -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ main.cpp -o main -lboost_system-mt -lboost_thread-mt -pthread -std=c++17

Set config for node in node_config.json and also list of peers in nodes.json

Run ./main

By typing "1" Node will broadcast "block_1.json" to connected peers.
