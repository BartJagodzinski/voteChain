Test of dividing block with 10.000 votes into smaller chunks to send via TCP, for now it only saves chunks into other file.

g++ main.cpp -o main -lboost_system -lboost_thread -pthread -std=c++17

OSX: g++ main.cpp -o main -lboost_system-mt -lboost_thread-mt -pthread -std=c++17

./main
