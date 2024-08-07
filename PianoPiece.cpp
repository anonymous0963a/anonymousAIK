#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <portaudio.h>
#include <sndfile.h>

// 日志类
class Logger {
public:
    static void logInfo(const std::string &message) {
        std::cout << "[INFO] " << message << std::endl;
    }

    static void logError(const std::string &message) {
        std::cerr << "[ERROR] " << message << std::endl;
    }
};

// 音频生成类
class PianoPiece {
public:
    PianoPiece() {
        // 初始化 PortAudio
        if (Pa_Initialize() != paNoError) {
            Logger::logError("PortAudio initialization failed");
            throw std::runtime_error("PortAudio initialization failed");
        }
    }

    ~PianoPiece() {
        Pa_Terminate();
    }

    static double noteNameToFrequency(const std::string &noteName) {
        std::vector<std::string> noteNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        double baseFrequency = 440.0; // A4 音符的频率为 440 Hz
        int octave = noteName[1] - '0'; // 音符的音阶
        int noteIndex = std::find(noteNames.begin(), noteNames.end(), noteName.substr(0, 2)) - noteNames.begin();
        if (noteIndex == noteNames.size()) { // 如果音符是单音符
            noteIndex = std::find(noteNames.begin(), noteNames.end(), std::string(1, noteName[0])) - noteNames.begin();
        }
        int semitoneDifference = (octave - 4) * 12 + noteIndex - 9;
        return baseFrequency * std::pow(2.0, semitoneDifference / 12.0);
    }

    void generatePianoPiece(const std::vector<std::pair<std::string, int>> &pianoScore, const std::string &filename) {
        int sampleRate = 44100; // 采样率
        int totalDuration = 0;   // 初始化总时长
        for (const auto &note : pianoScore) {
            totalDuration += note.second; // 计算总时长
        }

        // 创建输出文件
        SF_INFO sfinfo;
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
        sfinfo.channels = 1; // 单声道
        sfinfo.samplerate = sampleRate;
        SNDFILE *outfile = sf_open(filename.c_str(), SFM_WRITE, &sfinfo, NULL);
        if (!outfile) {
            Logger::logError("Unable to open output file");
            throw std::runtime_error("Unable to open output file");
        }

        // 生成音符
        std::vector<double> audioBuffer(totalDuration * sampleRate / 1000, 0.0);
        int index = 0;

        for (const auto &note : pianoScore) {
            double frequency = noteNameToFrequency(note.first);
            int duration = note.second;

            for (int t = 0; t < duration * sampleRate / 1000; ++t) {
                audioBuffer[index++] += 0.5 * std::sin(2 * M_PI * frequency * (t / (double)sampleRate));
            }
        }

        // 将生成的数据写入文件
        sf_write_double(outfile, audioBuffer.data(), audioBuffer.size());
        sf_close(outfile);
        Logger::logInfo("Piano piece generated and saved to " + filename);
    }
};

int main() {
    try {
        // 定义音符和它们的时长，构建乐谱
        std::vector<std::pair<std::string, int>> pianoScore = {
            {"C4", 500}, {"D4", 500}, {"E4", 500}, {"F4", 500},
            {"G4", 500}, {"A4", 500}, {"B4", 500},
            {"C5", 500}, {"B4", 500}, {"A4", 500}, {"G4", 500},
            {"F4", 500}, {"E4", 500}, {"D4", 500}, {"C4", 500}
        };

        // 创建 PianoPiece 对象并生成音乐
        PianoPiece piano;
        piano.generatePianoPiece(pianoScore, "piano_piece.wav");

    } catch (const std::exception &e) {
        Logger::logError("An error occurred: " + std::string(e.what()));
    }
    return 0;
}
