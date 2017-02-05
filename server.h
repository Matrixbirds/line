#ifndef __SERVER__
#define __SERVER__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>


#define BUFFERSIZE 4096
#define PROCESS_NUM 5
#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)


typedef struct {
  int sockfd;
  struct sockaddr_in *serv_addr;
} tcpserver, * tcpserver_ptr;

tcpserver_ptr setup_tcpserver(const char *addr, unsigned int port);
void malloc_tcpserver(tcpserver_ptr *addr);
void release_tcpserver(const tcpserver_ptr *addr);
void launch(const tcpserver_ptr *serv_addr, bool cluster, const pid_t **pids);
void prefork(const tcpserver_ptr *serv_addr, const pid_t **pids);
void connection_loop(const tcpserver_ptr *serv_addr);
void cleanup(const tcpserver_ptr *serv_addr);
void clean_fork(const pid_t *pids);
#endif
