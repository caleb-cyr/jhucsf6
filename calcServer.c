#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"
#include <pthread.h>
#include "thread.h"

void *worker(void * args){
  struct connInfo *myInfo = args;
  pthread_detach(pthread_self());
  int keep_going = 1;
  while(keep_going){
  keep_going = chat_with_client(myInfo->myCalc,myInfo->clientfd);
  }
  close(myInfo->clientfd);
  free(myInfo);
  return NULL;
}
int chat_with_client(struct Calc * myCalc,int client){
	
   rio_t rio;
  int result = 0;
  rio_readinitb(&rio,client); 
  char line[RIO_BUFSIZE];
  int done = 0;
  while(!done){
    ssize_t n = rio_readlineb(&rio,line,RIO_BUFSIZE);
    line[n] = '\0';
    if(n <= 0)
      done = 1;
    else if(strcmp(line,"quit\n") == 0 || strcmp(line,"quit\r\n") == 0){
     done = 1;
     return 1;
    }
    else if(strcmp(line,"shutdown\n") == 0 || strcmp(line,"shutdown\r\n") == 0){ 
      done = 1;
      return 0;
    }
    else{
      if(calc_eval(myCalc,line,&result) == 0)
	rio_writen(client,"ERROR\n",6);
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
   if (argc < 2)
    return 1;
   char * port = argv[1];
   char * webroot = argv[2];
   struct Calc * myCalc = calc_create();
   int client;
   int server = open_listenfd(port);
   while(1){
     client = Accept(server,NULL,NULL);
     if(client > 0){
     struct connInfo * info = malloc(sizeof(struct connInfo));
     info->clientfd = client;
     info->webroot = webroot;
     info->myCalc = myCalc;
     pthread_t newThread;
     if(pthread_create(&newThread,NULL,worker,info) != 0)
       return 0;
     }
   }
   return 0;
}


    
  
  
