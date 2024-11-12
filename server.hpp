#ifndef SERVER_HPP
#define SERVER_HPP


#include <string>
#include <vector>
#include <sys/select.h> // Para select()
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // Para close()

#define BUFFER_SIZE 1024 // Definir buffer size em um único lugar

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();

    void start();
    void handleNewConnection();
    void handleClientMessage(int client_socket);
    void broadcast(const std::string& message, int sender_socket);

private:
    int server_socket;       // Primeiro o socket do servidor
    int port;
    std::string password;    // Depois a senha
    std::vector<int> clients;
    fd_set master_set; // Set de descritores para o select
};

#endif // SERVER_HPP
