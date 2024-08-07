#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdexcept>

class ImageProcessor {
public:
    ImageProcessor(const std::string& log_file) : log_file_(log_file) {
        // 初始化日志文件
        std::ofstream ofs(log_file_, std::ios::out | std::ios::app);
        if (!ofs) {
            throw std::runtime_error("Failed to open log file");
        }
        ofs << "=== Image Processing Log ===" << std::endl;
        ofs.close();
    }

    void capture_image() {
        cv::VideoCapture cap(0); // 打开默认摄像头
        if (!cap.isOpened()) {
            throw std::runtime_error("Failed to open camera");
        }

        cv::Mat frame;
        cap >> frame; // 抓取一帧图像

        if (frame.empty()) {
            throw std::runtime_error("Captured empty frame");
        }

        process_image(frame);
    }

private:
    std::string log_file_;

    void process_image(const cv::Mat& image) {
        cv::Mat gray_image;
        cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY); // 转换为灰度图像

        std::string window_name = "Processed Image";
        cv::imshow(window_name, gray_image); // 显示处理后的图像
        log_info("Image processed and displayed");

        cv::waitKey(0);
    }

    void log_info(const std::string& message) {
        std::ofstream ofs(log_file_, std::ios::out | std::ios::app);
        if (ofs) {
            auto current_time = boost::posix_time::second_clock::local_time();
            ofs << boost::posix_time::to_simple_string(current_time) << ": " 
                << message << std::endl;
            ofs.close();
        } else {
            std::cerr << "ERROR: Unable to write to log file." << std::endl;
        }
    }
};

int main() {
    const std::string log_file = "image_processing.log";

    try {
        ImageProcessor processor(log_file);
        processor.capture_image();
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
