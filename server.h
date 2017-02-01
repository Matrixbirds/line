#ifndef __SERVER__
#define __SERVER__

#include <sys/types.h>

typedef struct {
  char *hostname;
  ssize_t port;
} srv_meta, * srv_meta_ptr;

srv_meta_ptr create_meta(const char *hostname, ssize_t port);
void inspect_meta(srv_meta_ptr *meta);

typedef struct {
  int sockfd;
} server, * server_ptr;

server_ptr create_server(srv_meta_ptr *meta);
#endif
