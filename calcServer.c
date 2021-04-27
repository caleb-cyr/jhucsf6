#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"
#include <pthread.h>
#include "thread.h"
void *worker(void * args){
  //create a client connection struct
  struct connInfo *myInfo = args;
  pthread_detach(pthread_self());//detatch the thread
  int keep_going = 1;
  while(keep_going){ //if shutdown command isn't called keep going
  keep_going = chat_with_client(myInfo->myCalc,myInfo->clientfd);
  //close client connection and free connection struct
  close(myInfo->clientfd);
  free(myInfo);
  }
  return NULL;
}
int chat_with_client(struct Calc * myCalc,int client){
	
   rio_t rio;
  int result = 0;
  rio_readinitb(&rio,client); //set up an empty read buffer 
  char line[RIO_BUFSIZE];
  int done = 0;
  while(!done){
    ssize_t n = rio_readlineb(&rio,line,RIO_BUFSIZE);//read in line
    line[n] = '\0'; //null terminate line
    if(n <= 0)
      done = 1; //if didn't read anything stop loop
    else if(strcmp(line,"quit\n") == 0 || strcmp(line,"quit\r\n") == 0){
      done = 1; //if quit is called, stop chatting
     return 1;
    }
    else if(strcmp(line,"shutdown\n") == 0 || strcmp(line,"shutdown\r\n") == 0){ 
      done = 1;//if shutdown is called, end connection
      return 0;
    }
    else{
      //if eval returns 0, show an error
      if(calc_eval(myCalc,line,&result) == 0)
	rio_writen(client,"ERROR\n",6);
      //print result to buffer
      else{
	int len = snprintf(line,RIO_BUFSIZE,"%d\n",result);
        if(len < RIO_BUFSIZE){
	  rio_writen(client,line,len);
	}
      }
    }
  }
  return 1;
}

int main(int argc, char **argv) {
	/* TODO: implement this program */
  //if incorrect number of arguments, return 1
   if (argc < 2)
    return 1;
   char * port = argv[1];//read in port
   char * webroot = argv[2];//create new calculator
   struct Calc * myCalc = calc_create(); //create new calculator
   int client;
   int server = open_listenfd(port); //open a socket
   while(1){
     client = Accept(server,NULL,NULL); //accept a connection to socket
     if(client > 0){
       //create new connection struct
       struct connInfo * info = malloc(sizeof(struct connInfo));
     info->clientfd = client;
     info->webroot = webroot;
     info->myCalc = myCalc;
     pthread_t newThread;//create thread variable
     //start thread function
     if(pthread_create(&newThread,NULL,worker,info) != 0)
       return 1;
 
     }
   }
   return 0;
}


    
  
  
