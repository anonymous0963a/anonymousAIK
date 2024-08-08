#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <spdlog/spdlog.h>

class MusicGenerator {
public:
    MusicGenerator(const std::string& outputDirectory, const std::string& filename);
    void generateTone(int frequency, int duration);
    void logMessage(const std::string &msg);

private:
    std::string outputPath;
};

MusicGenerator::MusicGenerator(const std::string& outputDirectory, const std::string& filename) {
    if (outputDirectory.empty() || filename.empty()) {
        throw std::invalid_argument("Output directory and filename cannot be empty");
    }
    outputPath = outputDirectory + "/" + filename;

    spdlog::info("Music will be generated at: {}", outputPath);
}

void MusicGenerator::generateTone(int frequency, int duration) {
    const int sampleRate = 44100; // 44.1 kHz
    const int totalSamples = sampleRate * duration;
    std::vector<char> buffer(totalSamples * 2); // 16-bit PCM

    // Create a simple sine wave tone
    for (int i = 0; i < totalSamples; ++i) {
        float amplitude = 32767.0f; // Maximum amplitude for 16-bit PCM
        float sample = amplitude * sin((2.0 * M_PI * frequency * i) / sampleRate);
        int16_t intSample = static_cast<int16_t>(sample);
        buffer[i * 2] = intSample & 0xff; // Low byte
        buffer[i * 2 + 1] = (intSample >> 8) & 0xff; // High byte
    }

    // Write WAV file header
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open output file");
    }

    outFile << "RIFF"; // Chunk ID
    outFile.write(reinterpret_cast<const char*>(&totalSamples), sizeof(totalSamples) + 36);
    outFile << "WAVE"; // Format

    // Subchunk 1 (fmt chunk)
    outFile << "fmt "; // Subchunk1 ID
    int32_t subchunk1Size = 16; // + 2 bytes for fmt size
    outFile.write(reinterpret_cast<const char*>(&subchunk1Size), sizeof(subchunk1Size));
    int16_t audioFormat = 1; // PCM format
    outFile.write(reinterpret_cast<const char*>(&audioFormat), sizeof(audioFormat));
    int16_t numChannels = 1; // Mono
    outFile.write(reinterpret_cast<const char*>(&numChannels), sizeof(numChannels));
    outFile.write(reinterpret_cast<const char*>(&sampleRate), sizeof(sampleRate));
    int32_t byteRate = sampleRate * numChannels * 2; // 16-bit, so 2 bytes
    outFile.write(reinterpret_cast<const char*>(&byteRate), sizeof(byteRate));
    int16_t blockAlign = numChannels * 2; // 16 bits means 2 bytes
    outFile.write(reinterpret_cast<const char*>(&blockAlign), sizeof(blockAlign));
    int16_t bitsPerSample = 16; // 16 bits
    outFile.write(reinterpret_cast<const char*>(&bitsPerSample), sizeof(bitsPerSample));

    // Subchunk 2 (data chunk)
    outFile << "data"; // Subchunk2 ID
    int32_t subchunk2Size = totalSamples * 2; // 2 bytes per sample
    outFile.write(reinterpret_cast<const char*>(&subchunk2Size), sizeof(subchunk2Size));
    outFile.write(buffer.data(), buffer.size());

    logMessage("Music generated successfully.");
}

void MusicGenerator::logMessage(const std::string &msg) {
    spdlog::info(msg);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <output_directory> <filename> <frequency>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string outputDirectory = argv[1];
    std::string filename = argv[2];
    int frequency = std::stoi(argv[3]);

    try {
        MusicGenerator generator(outputDirectory, filename);
        generator.generateTone(frequency, 5); // 5 seconds tone
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
