#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

srv_meta_ptr create_meta(const char *hostname, ssize_t port)
{
  srv_meta_ptr meta = malloc(sizeof(srv_meta));
  meta->hostname = hostname;
  meta->port = port;
  return meta;
}

void inspect_meta(srv_meta_ptr *meta)
{
  printf("[hostname]: %s\n", (*meta)->hostname);
  printf("[port]: %d\n", (*meta)->port);
}

server_ptr create_server(srv_meta_ptr *meta)
{
  server_ptr srv = malloc(sizeof(server));
  if ((srv->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket create failed\n");
    exit(1);
  }
  int ov = 4;
  if (setsockopt(srv->sockfd, SOL_SOCKET, SO_ACCEPTCONN & SO_REUSEADDR & SO_PEEK_OFF, &ov, sizeof ov) < 0)
  {
    perror("set socket option failed\n");
    exit(1);
  }
  struct sockaddr_in serv_addr;
  bzero((char *) &serv_addr, sizeof serv_addr);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons((*meta)->port);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  if (bind(srv->sockfd, (struct sockaddr *) &serv_addr, sizeof serv_addr) < 0) {
    perror("socket bind failed\n");
    exit(1);
  }
  if (listen(srv->sockfd,5) < 0) {
    perror("socket listen failed\n");
    exit(1);
  }
  return srv;
}
