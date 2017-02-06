#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

pid_t pids[PROCESS_NUM] = {};

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

void launch(const tcpserver_ptr *serv_addr, bool cluster) {
  if (cluster) {
    prefork(serv_addr);
  } else {
    connection_loop(serv_addr);
  }
  process_signal_handler();
}

void prefork(const tcpserver_ptr *serv_addr) {
  for (int i = 0; i < PROCESS_NUM; i++) {
    if ((pids[i] = fork()) < 0)
      handle_error("spawn process");
    else if (pids[i] == 0) {
      set_parent_dead_signal();
      connection_loop(serv_addr);
    }
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

void handle_signal(int signo)
{
  const char *signal_name;
  switch(signo) {
    case SIGHUP:
      signal_name = "SIGHUP";
      break;
    case SIGUSR1:
      signal_name = "SIGUSR1";
      break;
    case SIGINT:
      signal_name = "SIGINT";
      break;
    case SIGKILL:
      signal_name = "SIGKILL";
      break;
    default:
      fprintf(stderr, "Caught wrong signal: %d", signo);
      return;
  }
  for (int i = 0; i < PROCESS_NUM; i++) {
    if (pids[i]) {
      kill(pids[i], signo);
    }
  }
  printf("Sending signal...");
  sleep(2);
  printf("Done");
  if (signo == SIGKILL || signo == SIGINT) exit(0);
}

void process_signal_handler()
{
  if (signal(SERVER_TERMINATE, handle_signal) == SIG_ERR)
    handle_error("signal handle");
  if (signal(SIGUSR1, handle_signal) == SIG_ERR)
    handle_error("signal handle");
}

void set_parent_dead_signal()
{
  if (prctl(PR_SET_PDEATHSIG, SIGKILL) < 0)
    handle_error("set deadth signal failed");
}
