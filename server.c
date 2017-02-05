#include "server.h"
#include "conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

tcpserver_ptr setup_tcpserver(const char *addr, unsigned int port)
{
  tcpserver_ptr srv = NULL;
  malloc_tcpserver(&srv);
  if ((srv->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    handle_error("socket");
  int ov = 4;
  if (setsockopt(srv->sockfd, SOL_SOCKET, SO_ACCEPTCONN & SO_REUSEADDR & SO_PEEK_OFF, &ov, sizeof ov) < 0)
    handle_error("setsockopt");
  struct sockaddr_in *serv_addr_p = srv->serv_addr;
  serv_addr_p->sin_family = AF_INET;
  serv_addr_p->sin_port = htons(port);
  serv_addr_p->sin_addr.s_addr = inet_addr(addr);
  if (bind(srv->sockfd, (struct sockaddr *)serv_addr_p, sizeof((*serv_addr_p))) < 0)
    handle_error("bind");
  if (listen(srv->sockfd,5) < 0)
    handle_error("listen");
  return srv;
}

void malloc_tcpserver(tcpserver_ptr *addr)
{
  tcpserver_ptr tmp = NULL;
  tmp = malloc( sizeof(tcpserver) );
  tmp->serv_addr = malloc( sizeof(struct sockaddr_in) );
  *addr = tmp;
}

void release_tcpserver(const tcpserver_ptr *addr)
{
  free((*addr)->serv_addr);
  free(*addr);
}

void launch(const tcpserver_ptr *serv_addr, bool cluster, const pid_t **pids) {
  if (cluster) {
    prefork(serv_addr, pids);
  } else {
    connection_loop(serv_addr);
  }
}

void prefork(const tcpserver_ptr *serv_addr, const pid_t **pids) {
  pid_t *p = *pids;
  while(!*p) {
    if ((*p = fork()) < 0)
      handle_error("fork child process");
    else if (*p == 0)
      connection_loop(serv_addr);
    p++;
  }
  wait(&(int) {0});
}

void connection_loop(const tcpserver_ptr *serv_addr)
{
  tcpserver_ptr server = *serv_addr;
  struct sockaddr_in* s_addr = server->serv_addr;
  int accept_fd;
  char buf[BUFFERSIZE];
  while(true) {
    if ((accept_fd = accept(server->sockfd, (struct sockaddr *) &s_addr, &(socklen_t) { sizeof s_addr })) < 0)
      handle_error("accept");
    bzero(buf, BUFFERSIZE);
    if (recv(accept_fd, buf, BUFFERSIZE, 0) < 0)
      handle_error("recv");
    if (write(accept_fd, buf, BUFFERSIZE) < 0)
      handle_error("write");
    close(accept_fd);
  }
}


void cleanup(const tcpserver_ptr *serv_addr)
{
  close((*serv_addr)->sockfd);
  release_tcpserver(serv_addr);
}
