#ifndef RECEIVER_H
#define RECEIVER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <arpa/inet.h>

class Receiver {
private:
    size_t _size = 2048;
    struct sockaddr_in _serverAddr;
    struct sockaddr_in _newAddr;
    int _sockfd;
public:
    Receiver() = default;

    Receiver(char *ip, int port) {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);

        _serverAddr.sin_family = AF_INET;
        _serverAddr.sin_port = port;
        _serverAddr.sin_addr.s_addr = inet_addr(ip);
        if(bind(_sockfd, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0) { perror("Error in bind"); exit(1); }      
        std::cout << "Receiver C'tor" << std::endl;
        listenForBlock();
    }

    bool listenForBlock() {
        if(listen(_sockfd, 10) == 0) std::cout <<"Listening..." << std::endl;
        else{ perror("Error in listening"); exit(1); }
        socklen_t addrSize = sizeof(_newAddr);
        int newSock;
        newSock = accept(_sockfd, (struct sockaddr*)&_newAddr, &addrSize);
        return saveBlock(newSock);
    }


    bool saveBlock(int sockfd){
        int bytes;
        FILE *fp = fopen("receivedBlock.json", "w");
        char buffer[_size];
        
        while (true) {
            bytes = recv(sockfd, buffer, _size, 0);
            if (bytes <= 0){ break; return false; }
            fprintf(fp, "%s", buffer);
            bzero(buffer, _size);
        }
        return true;
    }

    ~Receiver() { std::cout << "Receiver D'tor" << std::endl; }
};

#endif
