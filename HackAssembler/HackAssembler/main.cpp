#include <iostream>
#include "Parser.h"

void printUsage() {
    std::cout << "Usage: " << std::endl 
              << "hasm <inputFile> [outputFile]" << std::endl;
}

std::string replaceExt(std::string& filepath, const std::string& ext1, const std::string& ext2) {
    return filepath.substr(0, filepath.find(ext1)) + ext2;
}

bool parseArgs(int argc, char** argv, std::string& file1, std::string& file2) {
    if ((argc != 2) && (argc != 3)) {
        printUsage();
        return false;
    }
    file1 = argv[1];
    file2 = (argc > 2) ? argv[2] : replaceExt(file1, ".asm", ".hack");
    return true;
}

void printError(std::unique_ptr<Hack::Parser>& parser, std::exception e) {
    LOG_ERR << "Encountered exception on line (" 
                << parser->getLineNumber() << "): " 
                << parser->getCmd() << std::endl
                << e.what();
}

int main(int argc, char** argv) {
    std::string inFile, outFile;
    if (!parseArgs(argc, argv, inFile, outFile)) {
        return 1;
    }
    std::unique_ptr<Hack::Parser> parser = std::unique_ptr<Hack::Parser>(new Hack::Parser(inFile, outFile));
    try {
        LOG << "Processing file " << inFile;
        parser->run();
        LOG << "Wrote output file " << outFile << std::endl;
        return 0;
    } catch (std::exception& e) {
        printError(parser, e);
        LOG_ERR << "Failed to process file: " << inFile << std::endl;
        return 1;
    }
}
