#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include "socket.h"
#include <arpa/inet.h>

int server;
struct sockaddr_in sockaddr;
ast_T* socket_socket_create(List* args) {
  ast_T* ret_val = init_ast(AST_INT);
  ast_T* a1 =  (ast_T*)args->items[0];
  ast_T* a2 =  (ast_T*)args->items[1];
  ast_T* a3 =  (ast_T*)args->items[2];
  ret_val->int_value = socket(a1->int_value, a2->int_value, a3->int_value);
  server = ret_val->int_value;
  return ret_val;
}

ast_T* socket_socket_bind(List* args) {
  ast_T* ret_val = init_ast(AST_INT);
  ast_T* sock =  (ast_T*)args->items[0];
  ast_T* addr =  (ast_T*)args->items[1];
  ast_T* port =  (ast_T*)args->items[2];
  
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = inet_addr(addr->string_value);
  sockaddr.sin_port = htons(port->int_value);
  ret_val->int_value = bind(sock->int_value, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
  
  return ret_val;
  
}

ast_T* socket_socket_listen(List* args) {
  ast_T* ret_val = init_ast(AST_INT);
  ast_T* sock =  (ast_T*)args->items[0];
  ast_T* backlog =  (ast_T*)args->items[1];
  ret_val->int_value = listen(sock->int_value, backlog->int_value);
  return ret_val;
}

ast_T* socket_socket_accept(List* args) {
  ast_T* ret_val = init_ast(AST_INT);
  ast_T* sock =   (ast_T*)args->items[0];
  ret_val->int_value = accept(sock->int_value, NULL, NULL);
  return ret_val;
}
ast_T* socket_socket_read(List* args) {
  ast_T* ret_val = init_ast(AST_INT);
  ast_T* sock =   (ast_T*)args->items[0];
  ast_T* buf =   (ast_T*)args->items[1];
  ast_T* bytes =   (ast_T*)args->items[2];

  char buffer[bytes->int_value];
  int n = read(sock->int_value, buffer, bytes->int_value);
  buf->string_value = buffer;
  ret_val->int_value = n;
  if(errno != 0) {
    fprintf(stderr, "Read error: %s\n", strerror(errno));
    exit(1);
  }
  return ret_val;
}
ast_T* socket_socket_write(List* args) {
  ast_T* ret_val = init_ast(AST_INT);
  ast_T* sock =   (ast_T*)args->items[0];
  ast_T* buf =   (ast_T*)args->items[1];
  ast_T* bytes =   (ast_T*)args->items[2];
  ret_val->int_value = write(sock->int_value, buf->string_value, bytes->int_value);
  return ret_val;
}

ast_T* socket_socket_close(List* args) {
  ast_T* ret_val = init_ast(AST_INT);
  ast_T* sock =   (ast_T*)args->items[0];
  ret_val->int_value = close(sock->int_value);
  return ret_val;
}
void start_server(int PORT) {
  // Create the server socket
  server_t server;

  server.sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Set the server address and port
  server.addr.sin_family = AF_INET;
  server.addr.sin_addr.s_addr = INADDR_ANY;
  server.addr.sin_port = htons(PORT);

  // Bind the socket to the address and port
  bind(server.sockfd, (struct sockaddr *) &server.addr, sizeof(server.addr));

  // Start listening for connections
  listen(server.sockfd, 10);

  // Accept incoming connections
  while (1) {
    // Create a client struct
    client_t client;
    client.addr_len = sizeof(client.addr);
    client.sockfd = accept(server.sockfd, (struct sockaddr *) &client.addr, &client.addr_len);

    // Read the request from the client
    char request[1024];
    int bytes_read = read(client.sockfd, request, sizeof(request));

    // Parse the request
    // ...

    // Set the response headers
    char headers[1024] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

    // Set the response body
    char body[1024] = "<h1>Hello, World!</h1>";

    // Send the response to the client
    write(client.sockfd, headers, strlen(headers));
    write(client.sockfd, body, strlen(body));

    // Close the client connection
    close(client.sockfd);
  }

  // Close the server socket
  close(server.sockfd);
}
