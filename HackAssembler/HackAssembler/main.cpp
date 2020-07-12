#include <iostream>
#include "Parser.h"

int main() {
    std::string filename = "/Users/drigie/Projects/nand2tetris/projects/06/rect/RectL.asm";
    std::string line;
    Hack::Parser parser(filename);
    parser.run();
}
