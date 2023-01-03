g++ blockchain.cpp -o blockchain -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ blockchain.cpp -o blockchain -lboost_system-mt -lboost_thread-mt -pthread -std=c++17


g++ client.cpp -o client -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ client.cpp -o client -lboost_system-mt -lboost_thread-mt -pthread -std=c++17


./blockchain

./client 127.0.0.1 9876

On client side type 1 character which will be last char of sender address, vote will be sent to mempool.
