
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include<unistd.h>
#define BSIZE 200
#define PORT 8080 

char *permissions(char*);

int main() 
{
    int sockfd, connfd, len; 
    char buf[BSIZE];
    struct sockaddr_in servaddr, cli; 

    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Failed to create socket.\nAborting.\n");
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr)); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 

    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Failed to bind to the socket.\nAborting.\n");
        return -1; 
    }

    if ((listen(sockfd, 5)) != 0) { 
        printf("Failed to listen.\nAborting.\n"); 
        return -1;
    } 
    len = sizeof(cli); 

    connfd = accept(sockfd, (struct sockaddr*)&cli, &len); 
    if (connfd < 0) { 
        printf("Failed to accept connection.\nAborting.\n"); 
        return -1;
    }
    
    read(connfd, buf, sizeof(buf));
    
    printf("Recieved filename: %s\nChecking the file.\n", buf);
    
    char stat[BSIZE];
    strcpy(stat, permissions(buf));
    if(strlen(stat)!=9) {
      strcpy(buf,"(error)");
      write(connfd, buf, sizeof(buf));
      printf("%s\nQuitting\n", stat);
      write(connfd, stat, sizeof(stat));
    } else {
      printf("Permissions for the file: %s\n", stat);
      write(connfd, stat, sizeof(stat));
      
      read(connfd, buf, sizeof(buf));
      if(strcmp(buf, "(err-create)")==0) {
        printf("Couldn't create the file on server.\nQuitting.\n");
      } else { //assume success
        printf("Sending file.\n");
      }
    }
    
    
    close(sockfd);
    
    return 0;
}

char* permissions(char *file){
    struct stat st;
    char *modeval = malloc(sizeof(char) * 9 + 1);
    if(stat(file, &st) == 0){
        mode_t perm = st.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';
        return modeval;     
    }
    else{
        return strerror(errno);
    }   
}

