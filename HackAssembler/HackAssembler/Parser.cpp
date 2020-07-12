#include "Parser.h"
#include <iostream>
#include <algorithm>
#include <functional>

using namespace Hack;

const charset Parser::SYMBOL_CHARS = {'_', '.', '$', ':'};
const stringMap Parser::DEST_MNEMONICS = {
    {"M",   "001"}, 
    {"D",   "010"},
    {"MD",  "011"},
    {"A",   "100"},
    {"AM",  "101"},
    {"AD",  "110"},
    {"AMD",  "111"}
};
const stringMap Parser::JUMP_MNEMONICS = {
    {"",    "000"},
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"}
};
const stringMap Parser::COMP_MNEMONICS = {
    {"0",   "0101010"},
    {"1",   "0111111"},
    {"-1",  "0111010"},
    {"D",   "0001100"},
    {"A",   "0110000"}, {"M", "1110000"},
    {"!D",  "0001101"},
    {"!A",  "0110001"}, {"!M", "1110001"},
    {"-D",  "0001111"},
    {"-A",  "0101010"}, {"-M", "1101010"},
    {"D+1", "0011111"},
    {"A+1", "0110111"}, {"M+1", "1110111"},
    {"D-1", "0001110"},
    {"A-1", "0110010"}, {"M-1", "1110010"},
    {"D+A", "0000010"}, {"D+M", "1000010"},
    {"D-A", "0010011"}, {"D-M", "1010011"},
    {"A-D", "0000111"}, {"M-D", "1000111"},
    {"D&A", "0000000"}, {"D&M", "1000000"},
    {"D|A", "0010101"}, {"D|M", "1010101"}
};

std::string Code::getDest(const std::string& destMnemonic) {
    std::string bits = Parser::DEST_MNEMONICS.at(destMnemonic);
    return bits;    
}
std::string Code::getComp(const std::string& compMnemonic) {
    std::string bits = Parser::COMP_MNEMONICS.at(compMnemonic);
    return bits;
}
std::string Code::getJump(const std::string& jumpMnemonic) {
    std::string bits = Parser::JUMP_MNEMONICS.at(jumpMnemonic);
    return bits;
}

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

bool Parser::sanitize(std::string& cmd) {
    // Remove comments
    size_t commentBegin = cmd.find("//");
    if (commentBegin != std::string::npos) {
        cmd = cmd.substr(0, commentBegin);
    }
    // Removes spaces
    auto isspace = [](char c){ return std::isspace(c); };
    cmd.erase(std::remove_if(cmd.begin(), cmd.end(), isspace), cmd.end());
    return !cmd.empty();
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
    if (pos == std::string::npos){ // '=' not found
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
    return ( !s.empty() &&
             DEST_MNEMONICS.find(s) == DEST_MNEMONICS.end()
        );
}

bool Parser::isJumpMnemonic(const std::string& s) {
    return ( !s.empty() &&
             JUMP_MNEMONICS.find(s) == JUMP_MNEMONICS.end()
        );
}

bool Parser::isCompMnemonic(const std::string& s) {
    return ( !s.empty() &&
             COMP_MNEMONICS.find(s) == COMP_MNEMONICS.end()
        );
}

std::string Parser::getCmd() const{ return m_cmd; }
uint64_t Parser::getLineNumber() const { return m_lineno; }
