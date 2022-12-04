#ifndef BLOCKCHAINNODE_H
#define BLOCKCHAINNODE_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <arpa/inet.h>
#include <thread>
#include "sender.h"
#include "receiver.h"

class BlockchainNode {
private:
    Sender _sender;
    Receiver _receiver;
public:
    void createReceiver(char *ip, int port) { _receiver = Receiver(ip, port); }
    void createSender(char *ip, int port){ _sender = Sender(ip, port); }

    BlockchainNode(char *ip, int port) { 
        //std::thread r((&BlockchainNode::createReceiver), this, ip, port);
        //std::thread s((&BlockchainNode::createSender), this, ip, port);
        std::thread r([=]{createReceiver(ip, port);});
        std::thread s([=]{createReceiver(ip, port);});
        r.join();
        s.join();
        std::cout << "BlockchainNode C'tor" << std::endl;
    }

    bool sendBlock(std::string fileName) { if(_sender.sendBlock(fileName)) return true; else return false;}

    bool listenForBlock() { return _receiver.listenForBlock(); }

    ~BlockchainNode() { std::cout << "BlockchainNode D'tor" << std::endl; }
};

#endif
