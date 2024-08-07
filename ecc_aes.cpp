#include <iostream>
#include <fstream>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define AES_KEY_SIZE 256
#define AES_BLOCK_SIZE 128

class SecureECCAESFileEncryptor {
private:
    const char* input_file_;
    const char* output_file_;
    unsigned char aes_key_[AES_KEY_SIZE / 8];

    void generateAESKey() {
        if (RAND_bytes(aes_key_, sizeof(aes_key_)) != 1) {
            throw std::runtime_error("Error generating AES key.");
        }
    }

public:
    SecureECCAESFileEncryptor(const char* input_file, const char* output_file)
        : input_file_(input_file), output_file_(output_file) {}

    void encrypt_decrypt_file(bool encrypt) {
    // 生成 AES 密钥
    generateAESKey();

    // 打开输入文件和输出文件
    std::ifstream input_file(input_file_, std::ios::binary);
    std::ofstream output_file(output_file_, std::ios::binary);

    if (!input_file.is_open() || !output_file.is_open()) {
        throw std::runtime_error("Error opening files.");
    }

    // 初始化 AES 加密/解密上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = EVP_aes_256_cbc(); // 使用 AES-256-CBC 模式
    unsigned char iv[EVP_MAX_IV_LENGTH]; // 初始化向量
    RAND_bytes(iv, sizeof(iv)); // 生成随机 IV
    output_file.write(reinterpret_cast<char*>(iv), sizeof(iv)); // 将 IV 写入输出文件

    if (encrypt) {
        EVP_EncryptInit_ex(ctx, cipher, nullptr, aes_key_, iv);
        unsigned char buffer[1024];
        unsigned char ciphertext[1024 + EVP_MAX_BLOCK_LENGTH];
        int len;

        while (input_file.read(reinterpret_cast<char*>(buffer), sizeof(buffer))) {
            EVP_EncryptUpdate(ctx, ciphertext, &len, buffer, input_file.gcount());
            output_file.write(reinterpret_cast<char*>(ciphertext), len);
        }

        EVP_EncryptFinal_ex(ctx, ciphertext, &len);
        output_file.write(reinterpret_cast<char*>(ciphertext), len);
    } else {
        EVP_DecryptInit_ex(ctx, cipher, nullptr, aes_key_, iv);
        unsigned char buffer[1024];
        unsigned char plaintext[1024 + EVP_MAX_BLOCK_LENGTH];
        int len;

        // 读取 IV
        input_file.read(reinterpret_cast<char*>(iv), sizeof(iv));

        while (input_file.read(reinterpret_cast<char*>(buffer), sizeof(buffer))) {
            EVP_DecryptUpdate(ctx, plaintext, &len, buffer, input_file.gcount());
            output_file.write(reinterpret_cast<char*>(plaintext), len);
        }

        EVP_DecryptFinal_ex(ctx, plaintext, &len);
        output_file.write(reinterpret_cast<char*>(plaintext), len);
    }

    // 清理上下文和文件
    EVP_CIPHER_CTX_free(ctx);
    input_file.close();
    output_file.close();

    std::cout << (encrypt ? "Encryption" : "Decryption") << " completed successfully." << std::endl;
}


    void runEncryption() {
        encrypt_decrypt_file(true);
    }

    void runDecryption() {
        encrypt_decrypt_file(false);
    }

};

int main(int argc, char* argv[]) {
    OpenSSL_add_all_algorithms();

    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <operation(0 - decrypt, 1 - encrypt)>" << std::endl;
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];

    SecureECCAESFileEncryptor file_encryptor(input_file, output_file);
    int operation = std::stoi(argv[3]);

    try {
        if (operation == 1) {
            file_encryptor.runEncryption();
        } else if (operation == 0) {
            file_encryptor.runDecryption();
        } else {
            std::cerr << "Invalid operation. Use 0 for decryption or 1 for encryption." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
