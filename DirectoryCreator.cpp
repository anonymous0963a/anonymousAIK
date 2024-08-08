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

// 目录创建类
class DirectoryCreator {
public:
    DirectoryCreator(const std::string &logFile) : logger(logFile) {}

    void createDirectory(const std::string &path) {
        if (!fs::exists(path)) {
            try {
                fs::create_directory(path);
                std::cout << "Directory " << path << " has been created." << std::endl;
                logger.write("Created directory: " + path);
            } catch (const std::exception &e) {
                throw std::runtime_error("An error occurred while creating directory " + path + ": " + e.what());
            }
        } else {
            std::cerr << "Directory " << path << " already exists." << std::endl;
        }
    }

private:
    Logger logger;
};

// 主函数
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <directory_name>" << std::endl;
        return 1;
    }

    std::string dirName = argv[1];
    std::string logFile = "mkdir_log.txt";

    // 创建目录创建器对象
    DirectoryCreator creator(logFile);

    // 确认创建目录
    std::string confirmCreation;
    std::cout << "Are you sure you want to create directory " << dirName << "? (yes/no): ";
    std::cin >> confirmCreation;

    if (confirmCreation == "yes") {
        try {
            creator.createDirectory(dirName);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Directory creation canceled." << std::endl;
    }

    return 0;
}
