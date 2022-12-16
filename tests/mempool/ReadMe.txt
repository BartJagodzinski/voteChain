g++ mempool.cpp -o mempool -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ mempool.cpp -o mempool -lboost_system-mt -lboost_thread-mt -pthread -std=c++17

./mempool <listenPort>
