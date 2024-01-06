#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

void convertFile(const std::string& inputFilePath, const std::string& outputFilePath);
void convertDirectory(const std::string& inputDirectory, const std::string& outputDirectory);

// 修改成自己的路径
int main() {
    std::string inputDirectory = "/Users/moguw/Github/NuMVC/data/raw/";
    std::string outputDirectory = "/Users/moguw/Github/NuMVC/data/processed/";

    convertDirectory(inputDirectory, outputDirectory);

    return 0;
}


void convertFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    // 打开输入文件
    std::ifstream inputFile(inputFilePath);

    if (!inputFile.is_open()) {
        std::cerr << "无法打开输入文件: " << inputFilePath << std::endl;
        return;
    }

    // 打开输出文件
    std::ofstream outputFile(outputFilePath);

    if (!outputFile.is_open()) {
        std::cerr << "无法打开输出文件: " << outputFilePath << std::endl;
        inputFile.close();
        return;
    }

    // 逐行读取文件
    std::string line;
    while (std::getline(inputFile, line)) {
        // 使用字符串流
        std::istringstream iss(line);

        // 用空格分割字符串
        std::string token;
        std::getline(iss >> std::ws, token, ' ');  // 读取第一个单词（跳过行首空格）

        // 检查第一个单词是否是 "p" 或 "e"
        if (token == "p" || token == "e") {
            // 如果是 "p" 或 "e"，则继续读取整行
            std::getline(iss, line);  // 读取整行（包括 "xx xxx" 部分）

            // 移除 "edge" 部分
            size_t posEdge = line.find("edge");
            if (posEdge != std::string::npos) {
                line.erase(posEdge, 5);  // 移除 "edge" 及其后的空格
            }

            // 移除 "col" 部分
            size_t posCol = line.find("col");
            if (posCol != std::string::npos) {
                line.erase(posCol, 4);  // 移除 "col" 及其后的空格
            }

            // 输出结果到文件
            outputFile << line << std::endl;
        }
    }

    // 关闭文件
    inputFile.close();
    outputFile.close();

    std::cout << "处理完成，结果已保存到 " << outputFilePath << std::endl;
}

void convertDirectory(const std::string& inputDirectory, const std::string& outputDirectory) {
    for (const auto& entry : fs::directory_iterator(inputDirectory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".clq") {
            // 构建输出文件路径
            std::string outputFilePath = outputDirectory + entry.path().stem().string() + ".txt";

            // 调用convertFile函数进行转换
            convertFile(entry.path().string(), outputFilePath);
        }
    }
}

