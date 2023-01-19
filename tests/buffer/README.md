Test of dividing block with 10.000 votes into smaller chunks to send via TCP.

g++ node.cpp -o node -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ node.cpp -o node -lboost_system-mt -lboost_thread-mt -pthread -std=c++17

First instance
./node 9999

Second instance
./node 8888 127.0.0.1 9999

Second node will send block_1.json to first node.
