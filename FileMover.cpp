#include <iostream>
#include <filesystem>
#include <string>
#include <stdexcept>
#include <fstream>
#include <ctime>

namespace fs = std::filesystem;

// 日志类
class Logger {
public:
    explicit Logger(const std::string &logFile) : logFile(logFile) {}

    void write(const std::string &entry) {
        std::ofstream logStream(logFile, std::ios_base::app);
        if (!logStream.is_open()) {
            throw std::runtime_error("Could not open log file for writing.");
        }
        logStream << currentDateTime() << " - " << entry << std::endl;
    }

private:
    std::string logFile;

    std::string currentDateTime() {
        std::time_t now = std::time(nullptr);
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buffer;
    }
};

// 文件移动类
class FileMover {
public:
    FileMover(const std::string &logFile) : logger(logFile) {}

    void moveFile(const std::string &source, const std::string &destination) {
        if (fs::exists(source)) {
            try {
                fs::rename(source, destination);
                std::cout << "Moved " << source << " to " << destination << std::endl;
                logger.write("Moved " + source + " to " + destination);
            } catch (const std::exception &e) {
                throw std::runtime_error("An error occurred while moving " + source + " to " + destination + ": " + e.what());
            }
        } else {
            std::cerr << "Source path " << source << " does not exist." << std::endl;
        }
    }

private:
    Logger logger;
};

// 主函数
int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <source> <destination>" << std::endl;
        return 1;
    }

    std::string sourcePath = argv[1];
    std::string destinationPath = argv[2];
    std::string logFile = "mv_log.txt";

    // 创建文件移动器对象
    FileMover mover(logFile);

    // 确认移动
    std::string confirmMove;
    std::cout << "Are you sure you want to move " << sourcePath << " to " << destinationPath << "? (yes/no): ";
    std::cin >> confirmMove;

    if (confirmMove == "yes") {
        try {
            mover.moveFile(sourcePath, destinationPath);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Move operation canceled." << std::endl;
    }

    return 0;
}
