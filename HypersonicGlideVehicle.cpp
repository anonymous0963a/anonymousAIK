#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <random>
#include <thread> // 追加：スレッドライブラリ

class HypersonicGlideVehicle {
public:
    HypersonicGlideVehicle(double initialAltitude, double initialSpeed)
        : altitude(initialAltitude), speed(initialSpeed), targetAltitude(0), targetSpeed(0) {
        if (initialAltitude < 0 || initialSpeed <= 0) {
            throw std::invalid_argument("Initial altitude must be non-negative and speed must be positive.");
        }
    }

    void navigate(double targetAltitude, double targetSpeed) {
        this->targetAltitude = targetAltitude;
        this->targetSpeed = targetSpeed;

        try {
            log("Starting navigation.");
            while (!hasReachedTarget()) {
                calculateTrajectory();
                adjustAltitude();
                adjustSpeed();
                sensorDataFeedback();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            log("Navigation completed.");
            postProcessing();
        } catch (const std::exception& e) {
            log("Error during navigation: " + std::string(e.what()));
        }
    }

private:
    double altitude;
    double speed;
    double targetAltitude;
    double targetSpeed;

    static constexpr double gravity = 9.81; // m/s^2
    static constexpr double airDragCoefficient = 0.05; // 空気抵抗係数

    void calculateTrajectory() {
        double airResistance = airDragCoefficient * speed * speed;
        altitude -= (gravity + airResistance) * 0.1;
        speed -= airResistance * 0.1;
        if (speed < 0) speed = 0;
        log("Trajectory calculated - Altitude: " + std::to_string(altitude) + ", Speed: " + std::to_string(speed));
    }

    void adjustAltitude() {
        if (altitude > targetAltitude) {
            altitude -= std::min(10.0, altitude - targetAltitude);
            log("Adjusting altitude to: " + std::to_string(altitude));
        }
    }

    void adjustSpeed() {
        if (speed < targetSpeed) {
            speed += std::min(5.0, targetSpeed - speed);
            log("Adjusting speed to: " + std::to_string(speed));
        }
    }

    void sensorDataFeedback() {
        double sensorAdjustment = generateRandomAdjustment();
        altitude += sensorAdjustment;
        log("Adjusting based on sensor feedback: " + std::to_string(sensorAdjustment));
    }

    bool hasReachedTarget() {
        return altitude <= targetAltitude && speed >= targetSpeed;
    }

    void log(const std::string& message) {
        std::ofstream logFile("hgv_log.txt", std::ios::app);
        if (!logFile) {
            throw std::runtime_error("Could not open log file.");
        }
        logFile << message << std::endl;
    }

    void postProcessing() {
        log("Processing post-navigation data.");

        bool missionSuccess = checkMissionSuccess();
        collectPerformanceData();

        log("Mission Success: " + std::string(missionSuccess ? "Yes" : "No"));
    }

    bool checkMissionSuccess() {
        return (altitude <= targetAltitude && speed >= targetSpeed);
    }

    void collectPerformanceData() {
        std::ofstream performanceFile("performance_data.txt", std::ios::app);
        if (!performanceFile) {
            throw std::runtime_error("Could not open performance data file.");
        }

        performanceFile << "Final Altitude: " + std::to_string(altitude) + " m\n";
        performanceFile << "Final Speed: " + std::to_string(speed) + " m/s\n";
        performanceFile << "Target Altitude: " + std::to_string(targetAltitude) + " m\n";
        performanceFile << "Target Speed: " + std::to_string(targetSpeed) + " m/s\n";
        performanceFile << "--------------------------------\n";
    }

    double generateRandomAdjustment() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> d(0, 1);
        return d(gen);
    }
};

int main() {
    try {
        HypersonicGlideVehicle hgv(10000, 3000);
        hgv.navigate(5000, 4000);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
