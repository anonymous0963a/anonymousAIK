cmake_minimum_required(VERSION 3.10)
project(SecureECCAESFileEncryptor)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 OpenSSL 库
find_package(OpenSSL REQUIRED)

# 添加可执行文件
add_executable(file_encryptor ecc_aes.cpp)

# 链接 OpenSSL 库
target_link_libraries(file_encryptor OpenSSL::Crypto)

# 可选：设定输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 可选：安装规则
install(TARGETS file_encryptor DESTINATION bin)
