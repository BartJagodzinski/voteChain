[libsecp256k1-dev](https://github.com/bitcoin-core/secp256k1) is needed    

sudo apt-get install libsecp256k1-dev

     
g++ main.cpp -o main -lboost_system -lboost_thread -lsecp256k1 -pthread -std=c++17

OSX: g++ main.cpp -o main -lboost_system-mt -lboost_thread-mt -lsecp256k1 -pthread -std=c++17

./main

You can check if it works properly by comparing outputs: [privatekeys.pw/calc](https://privatekeys.pw/calc)
