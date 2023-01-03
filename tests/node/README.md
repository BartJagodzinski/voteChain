Node work as server and client.

g++ node.cpp -o node -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ node.cpp -o node -lboost_system-mt -lboost_thread-mt -pthread -std=c++17

Run ./node <listen_port_for_server> <(optional)_host_of_peer_to_connect> <(optional)_port_of_peer_to_connect>

By typing "1" Node will send "block_1.json" to connected peer.
