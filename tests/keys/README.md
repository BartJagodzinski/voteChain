
g++ key.cpp -o key -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ key.cpp -o key -lboost_system-mt -lboost_thread-mt -pthread -std=c++17

./key

You can check if it works properly by comparing outputs: [privatekeys.pw/calc](https://privatekeys.pw/calc)
