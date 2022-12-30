#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../ast.h"
typedef struct {
  int sockfd; // Socket file descriptor
  struct sockaddr_in addr; // Client address
  socklen_t addr_len; // Length of client address
} client_t;

// Struct to store information about the server
typedef struct {
  int sockfd; // Socket file descriptor
  struct sockaddr_in addr; // Server address
} server_t;

ast_T* socket_socket_create(List* args);
ast_T* socket_socket_bind(List* args);
ast_T* socket_socket_listen(List* args);
ast_T* socket_socket_accept(List* args);
ast_T* socket_socket_read(List* args);
ast_T* socket_socket_write(List* args);
ast_T* socket_socket_close(List* args);

void start_server(int PORT);
