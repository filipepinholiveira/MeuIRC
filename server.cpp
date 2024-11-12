#include "server.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h> // Para close()
#include <algorithm> // Para std::remove
#include <cstring>   // Para memset()

Server::Server(int port, const std::string& password) 
    : server_socket(-1), port(port), password(password) {}

Server::~Server() {
    close(server_socket);
}

void Server::start() {
    struct sockaddr_in server_addr;

    // Criar o socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Erro ao criar o socket.\n";
        exit(1);
    }

    // Configurar a estrutura sockaddr_in
    memset(&server_addr, 0, sizeof(server_addr)); // Limpar a estrutura
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Associar o socket ao endereço e porta
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Erro ao associar o socket.\n";
        exit(1);
    }

    // Colocar o socket em modo de escuta
    if (listen(server_socket, 10) < 0) {
        std::cerr << "Erro ao colocar o socket em modo de escuta.\n";
        exit(1);
    }

    // Preparar o set de descritores
    FD_ZERO(&master_set);
    FD_SET(server_socket, &master_set);

    std::cout << "Servidor IRC iniciado.\n";

    // Laço principal de execução do servidor
    while (true) {
        fd_set read_fds = master_set; // Copiar o master_set para manipulação

        // Esperar por atividade no socket
        int activity = select(FD_SETSIZE, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            std::cerr << "Erro no select.\n";
            continue;
        }

        // Verificar se há uma nova conexão
        if (FD_ISSET(server_socket, &read_fds)) {
            handleNewConnection();
        }

        // Verificar as mensagens de clientes existentes
        for (size_t i = 0; i < clients.size(); ++i) {
            int client_socket = clients[i];
            if (FD_ISSET(client_socket, &read_fds)) {
                handleClientMessage(client_socket);
            }
        }
    }
}

void Server::handleNewConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

    if (client_socket < 0) {
        std::cerr << "Erro ao aceitar nova conexão.\n";
        return;
    }

    // Enviar prompt para o cliente pedindo a senha
    const std::string password_prompt = "Por favor, insira a senha:\n";
    send(client_socket, password_prompt.c_str(), password_prompt.length(), 0);

    char buffer[BUFFER_SIZE];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    buffer[bytes_read] = '\0';

    // Remover o '\n' da senha
    std::string client_password(buffer);
    client_password.erase(client_password.find_last_not_of("\n") + 1);  // Remove o \n

    // Verifica se a senha fornecida está correta
    if (client_password != password) {
        const std::string error_message = "Senha incorreta. Conexão fechada.\n";
        send(client_socket, error_message.c_str(), error_message.length(), 0);
        close(client_socket);
        return;
    }

    // Adicionar o novo cliente à lista
    FD_SET(client_socket, &master_set);  
    clients.push_back(client_socket);    

    std::cout << "Novo cliente conectado: " << client_socket << "\n";
}

void Server::handleClientMessage(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        // Se o cliente desconectar ou houver erro
        close(client_socket);
        FD_CLR(client_socket, &master_set);
        clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
        std::cout << "Cliente desconectado: " << client_socket << "\n";
        return;
    }
    
    buffer[bytes_read] = '\0'; // Garantir que a string esteja terminada

    // Mensagem de cliente recebida
    std::string message = std::string(buffer);

    // Enviar a mensagem para todos os outros clientes (broadcast)
    broadcast(message, client_socket);
}

void Server::broadcast(const std::string& message, int sender_socket) {
    for (size_t i = 0; i < clients.size(); ++i) {
        int client_socket = clients[i];
        if (client_socket != sender_socket) {
            // Envia a mensagem para todos os outros clientes
            send(client_socket, message.c_str(), message.length(), 0);
        }
    }
}
