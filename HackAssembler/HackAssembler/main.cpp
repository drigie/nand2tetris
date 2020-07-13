#include <iostream>
#include "Parser.h"



int main(int argc, char** argv) {
    std::string inFile, outFile;
    if ((argc != 2) && (argc != 3)) {
        std::cout << "Usage: " << std::endl 
                  << "hasm <inputFile> [outputFile]" << std::endl;
        return 1;
    }
    inFile = argv[1];
    std::unique_ptr<Hack::Parser> parser;
    if( argc > 2 ) { 
        outFile = argv[2]; 
        parser = std::unique_ptr<Hack::Parser>(new Hack::Parser(inFile, outFile));
    } else {
        parser = std::unique_ptr<Hack::Parser>(new Hack::Parser(inFile));
    }
    try {
        parser->run();
    } catch (std::exception& e) {
        LOG_ERR << "Encountered exception on line (" 
                << parser->getLineNumber() << "): " 
                << parser->getCmd() << std::endl
                << e.what();
        LOG_ERR << "Failed to process file: " << inFile << std::endl;
        return 1;
    }
    return 0;
}
