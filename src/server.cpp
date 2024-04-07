#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char **argv) {
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage
  
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  
  std::cout << "Waiting for a client to connect...\n";
  
  // accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  std::cout << "Client connected\n";
  
  if (client_fd < 0){
    std::cerr << "Socket connection failed\n";
    return 1;
  }

  char buffer[1024] = {0};
  recv(client_fd, buffer, sizeof(buffer), 0);

  std::string_view buffer_content = buffer;

  std::cout << "Message from client: " << std::endl << buffer << std::endl;

  std::string response_data;
  size_t path_pos = buffer_content.find("/");
  // std::cout << path_pos << std::endl;
  // std::cout << "buffer_content:" << buffer_content.at(path_pos) << std::endl;

  if (buffer_content.at(path_pos + 1) == ' ')
  {
    response_data = "HTTP/1.1 200 OK\r\n\r\n";
  }else{
    path_pos = buffer_content.find("/echo/");

    if (path_pos == buffer_content.npos)
    {
      response_data = "HTTP/1.1 404 Not Found\r\n\r\n";
    }
    else
    {
      size_t protocol_version_pos = buffer_content.find("HTTP");
      size_t echo_start_pos = path_pos + 6;
      std::string_view substr = buffer_content.substr(echo_start_pos, protocol_version_pos - echo_start_pos - 1);
      response_data = "HTTP/1.1 200 OK\r\n";
      response_data.append("Content-Type: text/plain\r\nContent-Length: ");
      response_data.append(std::to_string(substr.size()));
      response_data.append("\r\n\r\n");
      response_data.append(substr);
    }
  }

  std::cout << "Response data:" << response_data << std::endl;

  

  
  send(client_fd, response_data.c_str(), response_data.size(), 0); 
  std::cerr << "Message sent\n";
  close(client_fd);
  close(server_fd);

  return 0;
}
