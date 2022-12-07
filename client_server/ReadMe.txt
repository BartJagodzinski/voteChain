Compile client and server 
g++ server.cpp -o server -lboost_system -lboost_thread -pthread -std=c++17
g++ client.cpp -o client -lboost_system -lboost_thread -pthread -std=c++17

Run ./server <port>
Run first  ./client <host> <port>
Run second ./client <host> <port>

Type in console name of file to send e.g.
block1.json

Now block1.json has been sent to other clients and saved as "savedFile.json"
