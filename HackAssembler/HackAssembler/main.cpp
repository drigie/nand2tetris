#include <iostream>
#include "Parser.h"

int main() {
    std::string filename = "test.txt";
    std::string line;
    Hack::Parser parser(filename);
    LOG << "hasmore = " << parser.hasMoreCommands();
    while(parser.advance()){
        LOG << "Line[" << parser.getLineNumber() << "] = " << parser.getCmd();
    }
    LOG << "hasmore = " << parser.hasMoreCommands();
    LOG << "Check numerics";
    LOG << Hack::Parser::isNumeric("abc");
    LOG << Hack::Parser::isNumeric("9.01");
    LOG << Hack::Parser::isNumeric("9.abc");
    LOG << Hack::Parser::isNumeric("123");
    LOG << "Check symbols";
    LOG << Hack::Parser::isSymbol("123");
    LOG << Hack::Parser::isSymbol("abc");
    LOG << Hack::Parser::isSymbol("a$.1");
    LOG << Hack::Parser::isSymbol("1abc");
    LOG << Hack::Parser::isSymbol("1a bc");
    LOG << Hack::Parser::isSymbol("");
    LOG << "Check dest";
    LOG << Hack::Parser::isDestMnemonic("A");
    LOG << Hack::Parser::isDestMnemonic("A");
    LOG << Hack::Parser::isDestMnemonic("A");
    LOG << Hack::Parser::isDestMnemonic("A");
}
