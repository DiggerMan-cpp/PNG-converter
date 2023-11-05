#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>
#include <algorithm>

#pragma pack(push, 2)
struct BitmapFileHeader {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

struct BitmapInfoHeader {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t sizeImage;
    int32_t xPelsPerMeter;
    int32_t yPelsPerMeter;
    uint32_t clrUsed;
    uint32_t clrImportant;
};

struct IconHeader {
    uint16_t reserved;
    uint16_t type;
    uint16_t count;
};

struct IconDirectoryEntry {
    uint8_t width;
    uint8_t height;
    uint8_t colorCount;
    uint8_t reserved;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t sizeInBytes;
    uint32_t offset;
};

static const char ppm_header[] = "P6\n32 32\n255\n";
#pragma pack(pop)

class ImageConverter {
public:
    void ConvertPNGtoBMP(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream inputFile(inputFileName, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error writing to output file." << std::endl;
            return;
        }

        std::ofstream outputFile(outputFileName, std::ios::binary);
        if (!outputFile) {
            std::cerr << "Error writing to output file." << std::endl;
            return;
        }

        BitmapFileHeader fileHeader;
        fileHeader.type = 0x4D42;
        fileHeader.reserved1 = 0;
        fileHeader.reserved2 = 0;

        BitmapInfoHeader infoHeader;
        infoHeader.size = sizeof(BitmapInfoHeader);
        infoHeader.width = 32;
        infoHeader.height = 32;
        infoHeader.planes = 1;
        infoHeader.bitCount = 24;
        infoHeader.compression = 0;
        infoHeader.sizeImage = 0;
        infoHeader.xPelsPerMeter = 0;
        infoHeader.yPelsPerMeter = 0;
        infoHeader.clrUsed = 0;
        infoHeader.clrImportant = 0;

        outputFile.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        outputFile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));


        std::vector<uint8_t> imageData(32 * 32 * 3, 0);

        outputFile.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());
    }
    void ConvertPNGtoPPM(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream inputFile(inputFileName, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error writing to output file." << std::endl;
            return;
        }

        std::vector<uint8_t> imageData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());


        std::ofstream outputFile(outputFileName, std::ios::binary);
        if (!outputFile) {
            std::cerr << "Error writing to output file." << std::endl;
            return;
        }

        outputFile.write(ppm_header, sizeof(ppm_header) - 1);
        outputFile.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());

        if (!outputFile) {
            std::cerr << "Error writing to output file." << std::endl;
            return;
        }
    }
    void ConvertPNGtoICO(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream inputFile(inputFileName, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error writing to output file." << std::endl;
            return;
        }

        std::vector<uint8_t> imageData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

        IconHeader iconHeader = { 0, 1, 1 };
        IconDirectoryEntry directoryEntry = { 32, 32, 0, 0, 1, 32, static_cast<uint32_t>(imageData.size()), 22 };

        std::ofstream outputFile(outputFileName, std::ios::binary);
        if (!outputFile) {
            std::cerr << "Error writing to output file." << std::endl;
            return;
        }


        outputFile.write(reinterpret_cast<const char*>(&iconHeader), sizeof(iconHeader));
        outputFile.write(reinterpret_cast<const char*>(&directoryEntry), sizeof(directoryEntry));
        outputFile.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());

        if (!outputFile) {
            std::cerr << "Error writing to output file." << std::endl;
            return;
        }
    }
};

void PrintUsage() {
    std::cout << "usage: ./image_converter --input <input.png> --output <output> --format <ppm/bmp/ico>" << std::endl;
    std::cout << "arguments:" << std::endl;
    std::cout << "  --input, -i   : Input(PNG)." << std::endl;
    std::cout << "  --output, -o  : output(Any name)." << std::endl;
    std::cout << "  --format, -f  : Format (ppm, bmp, ico)." << std::endl;
    std::cout << "  --help, -h    : Help." << std::endl;
}


int main(int argc, char* argv[]) {
    ImageConverter convert;
    std::string inputFileName;
    std::string outputFileName;
    std::string format;
    if (argc < 2) {
        PrintUsage();
        return 0;
    }

    if (argc != 7) {
        std::cerr << "usage: ./image_converter --input <input.png> --output <output> --format <ppm/bmp/ico>" << std::endl;
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            PrintUsage();
            return 0;
        }

        if (arg == "--input" || arg == "-i") {
            if (i + 1 < argc) {
                inputFileName = argv[i + 1];
                i++;
            }
            else {
                std::cerr << "undefined after argument --input." << std::endl;
                return 1;
            }
        }
        else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                outputFileName = argv[i + 1];
                i++;
            }
            else {
                std::cerr << "undefined after argument --output." << std::endl;
                return 1;
            }
        }
        else if (arg == "--format" || arg == "-f") {
            if (i + 1 < argc) {
                format = argv[i + 1];
                i++;
            }
            else {
                std::cerr << "undefined after argument --format." << std::endl;
                return 1;
            }
        }
        else {
            std::cerr << "undefined argument: " << arg << std::endl;
            return 1;
        }
    }

    if (inputFileName.empty() || outputFileName.empty() || format.empty()) {
        std::cerr << "avalible param --input, --output и --format." << std::endl;
        return 1;
    }
     if (format == "bmp") {

        convert.ConvertPNGtoBMP(inputFileName, outputFileName);
    }
     else if(format == "ppm") {
         convert.ConvertPNGtoPPM(inputFileName, outputFileName);
     }
    else if (format == "ico") {
        convert.ConvertPNGtoICO(inputFileName, outputFileName);
    }
    else {
        std::cerr << "Undefined. Avalible:  ppm, bmp, ico." << std::endl;
        return 1;
    }

    std::cout << "Succesfull." << std::endl;
    return 0;
}
