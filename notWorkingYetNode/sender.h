#ifndef SENDER_H
#define SENDER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <arpa/inet.h>
class Sender {
private:
    size_t _size = 2048;
    struct sockaddr_in _serverAddr;
    int _sockfd;
public:
    Sender() = default;

    Sender(char *ip, int port) {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_sockfd == -1) { perror("Error in socket"); exit(1); }
        _serverAddr.sin_family = AF_INET;
        _serverAddr.sin_port = port;
        _serverAddr.sin_addr.s_addr = inet_addr(ip);

        int connection = connect(_sockfd, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr));
        if(connection == -1) { perror("Error in socket"); exit(1); }

        std::cout << "Sender C'tor" << std::endl;
    }

    bool sendBlock(std::string fileName) {
        char data[_size] = {0};
        FILE *fp = fopen(fileName.c_str(), "r");
        if (fp == NULL) { perror("Error in reading file."); exit(1); return false; }

        while(fgets(data, _size, fp) != NULL) {
            if (send(_sockfd, data, sizeof(data), 0) == -1) { perror("Error in sending block."); exit(1); return false; }
            bzero(data, _size);
        }
        std::cout <<"File data sent successfully."<< std::endl;
	    std::cout <<"Closing the connection."<< std::endl;
        close(_sockfd);
        return true;
    }

    ~Sender() { std::cout << "Sender D'tor" << std::endl; }
};

#endif
