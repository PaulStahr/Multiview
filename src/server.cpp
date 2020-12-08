/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


#include <iostream>
//#include <ncurses.h>
#include <sched.h>
#include <string.h>
#include <stdint.h>
#include <sstream>
#include <fstream>

#include "server.h"


void CommandServer::error(const char *msg)
{
    perror(msg);
}


void *server_thread_fkt(void *x_void_ptr)
{
   CommandServer *obj = (CommandServer *)x_void_ptr;
   obj->run();
   return NULL;
}

void CommandServer::start(){
    running = true;
    pthread_t inc_x_thread;
    if(pthread_create(&inc_x_thread, NULL, server_thread_fkt, this)) {
        std::cerr << "Error creating thread" << std::endl;
    }
}

void CommandServer::stop(){
    running = false;
}

void CommandServer::setCommandExecutor (std::function<void(std::string, std::ostream &)> ce){
    _command_execute = ce;
}

void CommandServer::run(){
     
     int sockfd, newsockfd;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = 2223;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (running) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         
          //close(sockfd);
          dostuff(newsockfd);
          close(newsockfd);
     } /* end of while */
     close(sockfd);
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/

void CommandServer::dostuff (int sock)
{
   int n;
   char buffer[256];
      
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   std::ostringstream stream;
   std::string command = buffer;
   _command_execute(command, stream);
   std::string answer = stream.str();
   n = write(sock,answer.c_str(),answer.size());
   if (n < 0) error("ERROR writing to socket");
}
