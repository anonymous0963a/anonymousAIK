#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>
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

// 文件删除类
class FileDeleter {
public:
    FileDeleter(const std::string &logFile) : logger(logFile) {}

    void deletePath(const std::string &path) {
        if (fs::exists(path)) {
            try {
                if (fs::is_regular_file(path)) {
                    fs::remove(path);
                    std::cout << "File " << path << " has been deleted." << std::endl;
                    logger.write("Deleted file: " + path);
                } else if (fs::is_directory(path)) {
                    fs::remove_all(path);
                    std::cout << "Folder " << path << " and its contents have been deleted." << std::endl;
                    logger.write("Deleted folder: " + path);
                } else {
                    throw std::runtime_error("Unknown path type: " + path);
                }
            } catch (const std::exception &e) {
                throw std::runtime_error("An error occurred while deleting " + path + ": " + e.what());
            }
        } else {
            std::cout << "Path " << path << " does not exist." << std::endl;
        }
    }

private:
    Logger logger;
};

// 主函数
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <path_to_delete>" << std::endl;
        return 1;
    }

    std::string pathToDelete = argv[1];
    std::string logFile = "deletion_log.txt";
    
    // 创建文件删除器对象
    FileDeleter deleter(logFile);

    // 确认删除
    std::string confirmDeletion;
    std::cout << "Are you sure you want to delete " << pathToDelete << "? (yes/no): ";
    std::cin >> confirmDeletion;

    if (confirmDeletion == "yes") {
        try {
            deleter.deletePath(pathToDelete);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Deletion canceled." << std::endl;
        deleter.logger.write("Cancellation of deletion for: " + pathToDelete);
    }

    return 0;
}
