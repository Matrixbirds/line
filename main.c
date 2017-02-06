#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <strings.h>
#include <stdbool.h>

#include "http.h"
#include "server.h"

int main(int argc, char *argv[])
{
  printf("[蛤]>");
  printf("I'm the master process %d\n", getpid());
  tcpserver_ptr server = setup_tcpserver("127.0.0.1", 3000);
  launch(&server, true); // true option represent cluster mode;
  cleanup(&server);
  return 0;
}
