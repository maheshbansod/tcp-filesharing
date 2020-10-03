
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 

#include<unistd.h>
#include<arpa/inet.h>

#define BSIZE 200 
#define PORT 8081 
  

void cleanFileName(char*);

int main(int argc, char **argv) 
{ 
    int sockfd, connfd; 
    char buf[BSIZE]; 
    struct sockaddr_in servaddr, cli; 
    
    if(argc < 2) {
      printf("Usage: %s filename\n where filename is the filename of the file to retrieve from the server.\n", argv[0]);
      return -1;
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Failed to create socket.\nAborting.\n"); 
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr)); 
  
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("Failed to connnect to the server.\n");
        return -1;
    }
    
//    printf("Enter the name of the file to look for: ");
//    fgets(buf, BSIZE, stdin);
    
    write(sockfd, argv[1], sizeof(argv[1]));
    
    read(sockfd, buf, sizeof(buf));
    if(strcmp("(error)", buf)==0) {
      printf("There was an error trying to retrieve '%s':\n", argv[1]);
      read(sockfd, buf, sizeof(buf));
      printf("ERROR(from server): %s\n", buf);
    } else {
      printf("Permissions of the file: %s\nCreating the file.\n", buf);
      cleanFileName(argv[1]);
      FILE *nf = fopen(argv[1], "w");
      if(nf == NULL) {
        printf("Unable to create file.\nQuitting\n");
        strcpy(buf, "(err-create)");
        write(sockfd, buf, sizeof(buf));
      } else {
        strcpy(buf, "(success)"); //notify server that it's ready for transfer.
        write(sockfd, buf, sizeof(buf));
        
        printf("Retrieving file from the server");
        while(1) {
          read(sockfd, buf, sizeof(buf));
          if(strcmp(buf, "(data)")==0) { //write the data to the file
            read(sockfd, buf, sizeof(buf));
            printf("writing: '%s'\n", buf);
            fwrite(buf, 1, sizeof(buf), nf);
          } else if(strcmp(buf, "(done)")==0) {
            printf("Done.\nFile transferred.\n");
            break;
          } else {
            printf("Unknown error occurred.\n");
            break;
          }
        }
        printf("Quitting.\n");
        fclose(nf);
      }
    }
    
    close(sockfd);
    
    return 0;
} 

/*Replaces the slash character(/) with underscore(_) to prevent error while creating the file*/
void cleanFileName(char *fname) {
  int i=0;
  while(fname[i]!='\0') {
    if(fname[i]=='/')
      fname[i]='_';
     
    i++;
  } 
}
