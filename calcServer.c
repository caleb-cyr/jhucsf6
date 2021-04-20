#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"


int chat_with_client(struct Calc * myCalc,int client){
  rio_t rio;
  int result = 0;
  rio_readinitb(&rio,client);   //set up empty read buffer
  char line[RIO_BUFSIZE];
  int done = 0;
  while(!done) {   //while chatting
    ssize_t n = rio_readlineb(&rio,line,RIO_BUFSIZE);   //read line
    line[n] = '\0';   //null terminate line
    if (n <= 0) { //check if nothing is read, and return from function if so
      done = 1;
    } else if (strcmp(line,"quit\n") == 0 || strcmp(line,"quit\r\n") == 0) {    //return from function if quit command is read
     done = 1;
     return 1;    //return 1 to signal session ending
    } else if (strcmp(line,"shutdown\n") == 0 || strcmp(line,"shutdown\r\n") == 0){    //return from function if shutdown command is read
      done = 1;
      return 0;   //return 0 to signal server process exit
    } else {
      if (calc_eval(myCalc,line,&result) == 0) {    //catch error if thrown from invalid expression
	      rio_writen(client,"ERROR\n",6);
      } else {  //else print result from calc eval function to next line
	      int len = snprintf(line,RIO_BUFSIZE,"%d\n",result);   
        if(len < RIO_BUFSIZE) {
	        rio_writen(client,line,len);
	      }
      }
    }
  }
}

int main(int argc, char **argv) {
   char * port = argv[1];               //get port number argument from command line
   int server = open_listenfd(port);    //open server port
   int keep_going = 1;
   struct Calc * myCalc = calc_create();
   //while client is connected, keep port open
   //close when client disconnects
   while (keep_going) {     //keep_going will be set to zero on shutdown command, set to one on quit command
     int client = Accept(server,NULL,NULL);             //accept user input from server port
     if(client > 0) {
       keep_going = chat_with_client(myCalc,client);    //set while loop indicator based on return value from chat_with_client function
       Close(client);                                   //close current client connection
     }
   }
   return 0;
}

    
  
  
