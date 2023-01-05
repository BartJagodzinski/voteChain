Test of dividing block of 10k votes into smaller chunks to send via TCP

g++ main.cpp -o main -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ main.cpp -o main -lboost_system-mt -lboost_thread-mt -pthread -std=c++17

./main
