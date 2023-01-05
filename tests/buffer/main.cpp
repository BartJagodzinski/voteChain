#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <deque>
#include <math.h>
#include <chrono>
#include "message.hpp"

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    std::ifstream file("block_1.json", std::ios::in | std::ios::binary);
    Message msg;
    // Get file size
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg);
    // Buffer to store whole file
    char buff[size];
    file.read(buff, size);
    file.close();
    // Copy buff into deque
    std::deque<char> deq(buff, buff + sizeof(buff)/sizeof(*buff));
    memset(buff, 0, sizeof(buff));


    // Delete all spaces to save memory
    // its very inefficient, takes ~2500ms
    /*
    for(auto it = deq.begin(); it != deq.end(); ++it) {
        if(*it == ' ') {
            deq.erase(it);
            //--it;
        }
    }
    */

    char chunk[Message::max_body_length];

    /*
        Send header with first vote = 322 B
        then send 125 votes ( 9375 B) until deq.size = 0
    */
    for(size_t i = 0; i < 322; ++i) {
        chunk[i] = deq[0];
        deq.pop_front();
    }

    std::ofstream blockFile("block_to_verify.json", std::ios::app);
    blockFile << chunk;
    bool loop = true;
    while(loop) {
        memset(chunk, 0, sizeof(chunk));
        for(size_t i = 0; i < 9375; ++i) {
            if(deq.size() == 0) {
                loop = false;
                break;
            }
            chunk[i] = deq[0];
            deq.pop_front();
        }
        blockFile << chunk;
    }
    blockFile.close();

    // Get ending timepoint
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    return EXIT_SUCCESS;
}
