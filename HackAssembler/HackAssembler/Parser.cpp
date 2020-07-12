#include "Parser.h"
#include <iostream>
#include <algorithm>
#include <functional>

using namespace Hack;

const charset Parser::SYMBOL_CHARS = {'_', '.', '$', ':'};
const stringset Parser::DEST_MNEMONICS = {
    "M", "D", "MD", "A", "AM", "AD", "AMD"
};
const stringset Parser::JUMP_MNEMONICS = {
    "JGT",
    "JEQ",
    "JGE",
    "JLT",
    "JNE",
    "JLE",
    "JMP"
};
const stringset Parser::COMP_MNEMONICS = {
    "0","1","D","A","!D","!A","-D","-A","D+1","A+1",
    "D-1", "A-1","D+A", "D-A", "A-D", "D&A", "D|A",
    "M", "!M", "-M", "M+1", "M-1", "D+M", "D-M", "M-D",
    "D&M", "D|M"
};

Parser::Parser() : m_lineno(0) {}

Parser::Parser(const std::string& filepath) : m_lineno(0) {
    loadFile(filepath);
}

std::string Parser::commandTypeString(const CommandType t) {
    switch (t){
        case CommandType::A_COMMAND : 
            return "A_COMMAND"; break;
        case CommandType::C_COMMAND :
            return "C_COMMAND"; break;
        case CommandType::L_COMMAND :
            return "L_COMMAND"; break;
        default :
            return "BAD_COMMAND";
    }
}

void Parser::loadFile(const std::string& filepath){
    m_file = std::ifstream(filepath);
}

bool Parser::advance(){
    if (readLine(m_cmd)){
        ++m_lineno;
        return true;
    }
    return false;
}

bool Parser::hasMoreCommands() const{
    return (m_file.is_open() && !m_file.eof());
}

bool Parser::readLine(std::string& line){
    if (!m_file.is_open()){
        LOG << "Could not open file!";
        return false;
    }
    if (!std::getline(m_file, line)) {
        return false;
    }
    return true;
}

bool Parser::isSymbolChar(const char x){
    return (
        std::isdigit(x)
    ||  std::isalpha(x)
    ||  Parser::any_of(x, SYMBOL_CHARS)
    );
}

bool Parser::isDigit(const char c) {
    return std::isdigit(c);
}

bool Parser::isNumeric(const std::string& s){
    if (s.empty()){
        return false;
    }
    return std::all_of(s.begin(), s.end(), Parser::isDigit);
}

bool Parser::isSymbol(const std::string& s){
    if (s.empty()){ return false; }
    if ( std::isdigit(s.at(0)) ) { 
        return false;
    }
    return std::all_of(s.begin(), s.end(), Parser::isSymbolChar);
}

bool Parser::isCommandA(const std::string& s, std::string& sym) {
    sym = "";
    if (s.empty()){
        return false;
    }
    if (s.at(0) != '@') {
        return false;
    }
    sym = s.substr(1);
    return ( 
            Parser::isSymbol(sym) 
        ||  Parser::isNumeric(sym)
    );
}

bool Parser::isCommandL(const std::string& s, std::string& sym) {
    sym = "";
    if (s.length() < 3 || s.front() != '(' || s.back() != ')') {
        return false;
    }
    sym = s.substr(1, s.length()-2);
    return Parser::isSymbol(sym);
}

bool Parser::getDest(const std::string& s, std::string& dest) {
    auto pos = s.find(s, '=');
    dest = "";
    if ( pos == std::string::npos ){ // '=' not found
        return false;
    }
    dest = s.substr(0, pos);
    return true;
}

bool Parser::getJump(const std::string& s, std::string& jump) {
    auto pos = s.find(s, ';');
    jump = "";
    if ( (pos == std::string::npos) ){ // '=' not found
        return false;
    }
    if (pos < (s.length()-1)) {
        jump = s.substr(pos+1); 
    }
    return true;
}

bool Parser::isCommandC(const std::string& s, 
                        std::string& dest, 
                        std::string& comp, 
                        std::string& jump) {
    bool hasDest = getDest(s, dest);
    bool hasJump = getJump(s, jump);
    size_t compLength = s.length() - dest.length() - jump.length();
    comp = s.substr(dest.length(), compLength);
    return (
            (dest.empty() || Parser::isDestMnemonic(dest))
        &&  Parser::isCompMnemonic(comp)
        &&  (jump.empty() || Parser::isJumpMnemonic(jump))
    );
}

bool Parser::isDestMnemonic(const std::string& s) {
    return Parser::any_of(s, DEST_MNEMONICS);
}

bool Parser::isJumpMnemonic(const std::string& s) {
    return Parser::any_of(s, JUMP_MNEMONICS);
}

bool Parser::isCompMnemonic(const std::string& s) {
    return Parser::any_of(s, COMP_MNEMONICS);
}



std::string Parser::getCmd() const{ return m_cmd; }
uint64_t Parser::getLineNumber() const { return m_lineno; }
