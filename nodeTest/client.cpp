#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <arpa/inet.h>
#define SIZE 2048

void sendBlock(FILE *fp, int sockfd) {
  char data[SIZE] = {0};

  while(fgets(data, SIZE, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) { perror("Error in sending block."); exit(1); }
    bzero(data, SIZE);
  }
}

int main(int argc, char* argv[]){
  if (argc != 3) { std::cout << "Usage: ./client <host> <port>" << std::endl; return EXIT_FAILURE; }
  char *ip = argv[1];
  int port = atoi(argv[2]), connection = -1, sockfd = -1;

  struct sockaddr_in server_addr;
  FILE *fp;
  std::string filename = "send.json";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) { perror("Error in socket"); exit(1); }
  std::cout << "Server socket created successfully."<< std::endl;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  connection = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(connection == -1) { perror("Error in socket"); exit(1); }
	std::cout <<"Connected to Server."<< std::endl;

  fp = fopen(filename.c_str(), "r");
  if (fp == NULL) { perror("Error in reading file."); exit(1); }

  sendBlock(fp, sockfd);
  std::cout <<"File data sent successfully."<< std::endl;
	std::cout <<"Closing the connection."<< std::endl;
  close(sockfd);

  return EXIT_SUCCESS;
}
