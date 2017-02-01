#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>

#include "http.h"
#include "server.h"

#define BUFFERSIZE 4096
#define PROCESS_SIZE 5

int main()
{
  printf("I'm the master process %d\n", getpid());
  srv_meta_ptr meta = create_meta("localhost", 3000);
  server_ptr server = create_server(&meta);
  struct sockaddr_in serv_addr;
  int accept_fd;
  char buf[BUFFERSIZE];
  bzero(buf, BUFFERSIZE);

  pid_t pids[PROCESS_SIZE];
  for (int i = 0; i < PROCESS_SIZE; i++) {
    if ((pids[0] = fork()) < 0) {
      perror("fork child process failed\n");
      exit(1);
    }
    else if (pids[0] == 0) {
      while(1) {
        accept_fd = accept(server->sockfd, (struct sockaddr *) &serv_addr, &(int *) { sizeof serv_addr });
        if (accept_fd < 0) {
          perror("accept failed\n");
          exit(1);
        }
        bzero(buf, BUFFERSIZE);
        if (recv(accept_fd, buf, BUFFERSIZE, 0) < 0) {
          perror("recv failed\n");
          exit(1);
        }
        printf("get from socket accept: %s %d\n", buf, getpid());
        if (write(accept_fd, buf, BUFFERSIZE) < 0) {
          perror("write accept file failed\n");
          exit(1);
        }
        close(accept_fd);
      }
    }
  }

  int status;
  wait(&status);
  close(server->sockfd);
  return 0;
}
