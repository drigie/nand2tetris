#include <iostream>
#include "Parser.h"



int main(int argc, char** argv) {
    std::string inFile, outFile;
    if ((argc != 2) && (argc != 3)) {
        std::cout << "Usage: " << std::endl 
                  << "hasm <inputFile> [outputFile]" << std::endl;
        exit(1);
    }
    inFile = argv[1];
    std::unique_ptr<Hack::Parser> parser;
    if( argc > 2 ) { 
        outFile = argv[2]; 
        parser = std::unique_ptr<Hack::Parser>(new Hack::Parser(inFile, outFile));
    } else {
        parser = std::unique_ptr<Hack::Parser>(new Hack::Parser(inFile));
    }
    parser->run();
}

// int main() {
//     std::string filename = "/Users/drigie/Projects/nand2tetris/projects/06/rect/RectL.asm";
//     std::string line;
//     Hack::Parser parser(filename);
//     parser.run();
// }
