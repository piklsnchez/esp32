#include <sys/socket.h>
#include "socket.h"

const int SOCKET_PORT        = 1234;
const int SOCKET_BACKLOG_LEN = 4;
const int SOCKET_BUFFER_LEN  = 1024;

struct socket* socket_new(){
  int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  return socket_new1(fd);
}

struct socket* socket_new1(int fd){
  struct socket* this = (struct socket*)malloc(sizeof(struct socket));
  this->fd       = fd;
  this->serverAddress.sin_family      = AF_INET;
  this->serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  this->serverAddress.sin_port        = htons(SOCKET_PORT);
  this->_buffer  = (char*)calloc(SOCKET_BUFFER_LEN+1, sizeof(char));
  this->sBind    = socket_bind;
  this->sListen  = socket_listen;
  this->sAccept  = socket_accept;
  this->sRead    = socket_read;
  this->delete   = socket_delete;
  return this;
}

int socket_bind(struct socket* this){
  return bind(this->fd, (struct sockaddr*)&this->serverAddress, sizeof(this->serverAddress));
}

int socket_listen(struct socket* this){
  return listen(this->fd, SOCKET_BACKLOG_LEN);
}

int socket_accept(struct socket* this, struct sockaddr_in* address){
  socklen_t len = sizeof(*address);
  return accept(this->fd, (struct sockaddr*)address, &len);
}

char* socket_read(struct socket* this){
  int r = read(this->fd, this->_buffer, SOCKET_BUFFER_LEN);
  this->_buffer[r] = '\0';
  return this->_buffer;
}

void socket_delete(struct socket* this){
  free(this->_buffer);
  close(this->fd);
  free(this);
}
