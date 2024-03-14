#include "webServ.hpp"


class Client {
    int clientSocket;
    sockaddr_in clientAddress;
    socklen_t clientAddressLength;
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    char body[BUFFER_SIZE];
    char *FullRequest;
    Client* next;

    public:
        Client(int clientSocket, sockaddr_in clientAddress, socklen_t clientAddressLength) {
            this->clientSocket = clientSocket;
            this->clientAddress = clientAddress;
            this->clientAddressLength = clientAddressLength;
        }
            int getSocketDescriptor() const {
            return clientSocket;
        }

        const sockaddr_in& getClientAddress() const {
            return clientAddress;
        }

        char* getBuffer() {
            return buffer;
        }

        ssize_t getBytesRead() const {
            return bytesRead;
        }

        void setBytesRead(ssize_t bytesRead) {
            this->bytesRead = bytesRead;
        }

        Client* getNext() const {
            return next;
        }

        void setNext(Client* next) {
            this->next = next;
        }
};


class Request {
    char *method; // 4
    char *path; // 1020
    char *version; 
    char *host;
    char *connection;
    char *cache_control;
    char *user_agent;
    char *accept;
    char *accept_encoding;
    char *accept_language;
    char *body;
    char *FullRequest;
    // /n/r
    public:
        Request(char *FullRequest) {
            this->FullRequest = FullRequest;
        }
        void parseRequest() {
            char *token = strtok(FullRequest, "\n");
            while (token != NULL) {
                if (strstr(token, "GET") != NULL) {
                    method = token;
                } else if (strstr(token, "Host") != NULL) {
                    host = token;
                } else if (strstr(token, "Connection") != NULL) {
                    connection = token;
                } else if (strstr(token, "Cache-Control") != NULL) {
                    cache_control = token;
                } else if (strstr(token, "User-Agent") != NULL) {
                    user_agent = token;
                } else if (strstr(token, "Accept") != NULL) {
                    accept = token;
                } else if (strstr(token, "Accept-Encoding") != NULL) {
                    accept_encoding = token;
                } else if (strstr(token, "Accept-Language") != NULL) {
                    accept_language = token;
                } else if (strstr(token, "HTTP/1.1") != NULL) {
                    version = token;
                }
                token = strtok(NULL, "\n");
            }
        }
        void printRequest() {
            std::cout << "Method: " << method << std::endl;
            std::cout << "Host: " << host << std::endl;
            std::cout << "Connection: " << connection << std::endl;
            std::cout << "Cache-Control: " << cache_control << std::endl;
            std::cout << "User-Agent: " << user_agent << std::endl;
            std::cout << "Accept: " << accept << std::endl;
            std::cout << "Accept-Encoding: " << accept_encoding << std::endl;
            std::cout << "Accept-Language: " << accept_language << std::endl;
            std::cout << "Version: " << version << std::endl;
        }
};

class Responce {
    char *status;
    char *server;
    char *date;
    char *content_type;
    char *content_length;
    char *connection;
    char *body;
    char *FullResponce;
    public:
        Responce(char *FullResponce) {
            this->FullResponce = FullResponce;
        }
        void parseResponce() {
            char *token = strtok(FullResponce, "\n");
            while (token != NULL) {
                if (strstr(token, "HTTP/1.1") != NULL) {
                    status = token;
                } else if (strstr(token, "Server") != NULL) {
                    server = token;
                } else if (strstr(token, "Date") != NULL) {
                    date = token;
                } else if (strstr(token, "Content-Type") != NULL) {
                    content_type = token;
                } else if (strstr(token, "Content-Length") != NULL) {
                    content_length = token;
                } else if (strstr(token, "Connection") != NULL) {
                    connection = token;
                }
                token = strtok(NULL, "\n");
            }
        }
        void printResponce() {
            std::cout << "Status: " << status << std::endl;
            std::cout << "Server: " << server << std::endl;
            std::cout << "Date: " << date << std::endl;
            std::cout << "Content-Type: " << content_type << std::endl;
            std::cout << "Content-Length: " << content_length << std::endl;
            std::cout << "Connection: " << connection << std::endl;
        }
};