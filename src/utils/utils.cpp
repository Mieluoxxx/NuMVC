#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>  // C++17 文件系统库

bool convertFile(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream fin(inputPath);
    if (!fin.is_open()) {
        std::cerr << "Error opening input file: " << inputPath << std::endl;
        return false;
    }

    std::ofstream fout(outputPath);
    if (!fout.is_open()) {
        std::cerr << "Error opening output file: " << outputPath << std::endl;
        fin.close(); // 关闭输入文件
        return false;
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.compare(0, 1, "p") == 0) {
            // Assuming the line format is "p col 125 6963"
            std::istringstream iss(line);
            std::string token;
            iss >> token; // Skip the first token ("p")
            iss >> token; // Skip the second token ("col")
            int num1, num2;
            iss >> num1 >> num2;
            fout << num1 << " " << num2 << std::endl;
        } else if (line.compare(0, 1, "e") == 0) {
            // Assuming the line format is "e 2 1"
            std::istringstream iss(line);
            std::string token;
            iss >> token; // Skip the first token ("e")
            int num1, num2;
            iss >> num1 >> num2;
            fout << num1 << " " << num2 << std::endl;
        }
    }

    // 关闭文件
    fin.close();
    fout.close();

    return true;
}

void processFiles(const std::string& rootPath) {
    std::filesystem::path rawPath = rootPath + "/data/raw/";
    std::filesystem::path processedPath = rootPath + "/data/processed/";

    for (const auto& entry : std::filesystem::directory_iterator(rawPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".clq") {
            std::string inputPath = entry.path().string();
            std::string outputFileName = entry.path().stem().string() + ".txt";
            std::string outputPath = processedPath.string() + "/" + outputFileName;

            if (!convertFile(inputPath, outputPath)) {
                std::cerr << "Failed to convert file: " << inputPath << std::endl;
                exit(0);
            }
        }
    }

}