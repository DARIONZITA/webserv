#include "server.hpp"
int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);


    // 1. Criar socket (IPv4, TCP)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // 2. Permitir reutilização da porta (evita "Address already in use")
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Configurar endereço
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    server_addr.sin_port = htons(8080);        // Porta 8080

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 1;
    }

    // 4. Escutar por conexões
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        return 1;
    }

    cout << "Servidor HTTP escutando na porta 8080..." << endl;

    while (true) {
        // 5. Aceitar conexão
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        cout << "Cliente conectado." << endl;

        // 6. Ler requisição
        
        Request *newRequest =  Request::read_request(client_fd);
        
        
        

        cout << "Request received:\n" << request << endl;

        // 7. Responder com "Hello World"
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 12\r\n"
            "\r\n"
            "Dário Nzita\n";

        send(client_fd, response, strlen(response), 0);

        // 8. Fechar conexão
        close(client_fd);
    }

    // 9. Fechar o socket do servidor (nunca atinge aqui no loop infinito)
    close(server_fd);
    return 0;
}
