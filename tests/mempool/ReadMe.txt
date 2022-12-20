g++ mempool.cpp -o mempool -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ mempool.cpp -o mempool -lboost_system-mt -lboost_thread-mt -pthread -std=c++17

./mempool <listenPort>

g++ client.cpp -o client -lboost_system -lboost_thread -pthread -std=c++17

./client 127.0.0.1 <mempoolListenPort>

On client side type 1 character which will be last char of sender address, vote will be sent to mempool.
