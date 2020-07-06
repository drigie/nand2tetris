#include <iostream>
#include "Parser.h"

int main() {
    std::string filename = "test.txt";
    std::string line;
    Hack::Parser parser(filename);
    while(parser.advance()){
        LOG << "Line[" << parser.getLineNumber() << "] = " << parser.getCmd();
    }
    LOG << "EOF" << std::endl;
}
