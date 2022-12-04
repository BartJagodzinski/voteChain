#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <arpa/inet.h>
#define SIZE 2048

void saveBlock(int sockfd, std::string fileName){
  int bytes;
  FILE *fp;
  char buffer[SIZE];

  fp = fopen(fileName.c_str(), "w");
  while (true) {
    bytes = recv(sockfd, buffer, SIZE, 0);
    if (bytes <= 0) { break; return; }
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  return;
}

int main(int argc, char* argv[]){
  if (argc != 3) { std::cout << "Usage: ./client <host> <port>" << std::endl; return EXIT_FAILURE; }
  char *ip = argv[1];
  int port = atoi(argv[2]), bnd = -1, sockfd = -1, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) { perror("Error in socket"); exit(1); }
  std::cout << "Server socket created successfully." << std::endl;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  bnd = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(bnd < 0) { perror("Error in bind"); exit(1); }
  std::cout << "Binding successfull." << std::endl;

  if(listen(sockfd, 10) == 0) std::cout << "Listening..." << std::endl;
  else { perror("Error in listening"); exit(1); }

  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
  saveBlock(new_sock, "block.json");
  std::cout << "Data written in the file successfully." << std::endl;

  return EXIT_SUCCESS;
}
