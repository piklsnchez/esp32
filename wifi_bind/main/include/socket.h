#ifndef SOCKET_H
#define SOCKET_H
#include <sys/socket.h>

struct socket{
  int fd;
  struct sockaddr_in serverAddress;
  char* _buffer;
  int (*sBind)(struct socket* this);
  int (*sListen)(struct socket* this);
  int (*sAccept)(struct socket* this, struct sockaddr_in* address);
  char* (*sRead)(struct socket* this);
  void (*delete)(struct socket* this);
};
struct socket* socket_new();
struct socket* socket_new1(int fd);
int socket_bind(struct socket* this);
int socket_listen(struct socket* this);
int socket_accept(struct socket* this, struct sockaddr_in* address);
char* socket_read(struct socket* this);
void socket_delete(struct socket* this);
#endif
