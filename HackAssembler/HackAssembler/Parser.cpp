#include "Parser.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <bitset>
#include <exception>

using namespace Hack;

const charset Parser::SYMBOL_CHARS = {'_', '.', '$', ':'};
const stringMap Parser::DEST_MNEMONICS = {
    {"",    "000"},
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
    try {
        std::string bits = Parser::DEST_MNEMONICS.at(destMnemonic);
        return bits;  
    } catch (std::exception& e) {
        LOG << "Key '" << destMnemonic << "' not found in map";
        throw(e);
    }  
}
std::string Code::getComp(const std::string& compMnemonic) {
    try {
        std::string bits = Parser::COMP_MNEMONICS.at(compMnemonic);
        return bits;  
    } catch (std::exception& e) {
        LOG << "Key '" << compMnemonic << "' not found in map";
        throw(e);
    }  
}
std::string Code::getJump(const std::string& jumpMnemonic) {
    try {
        std::string bits = Parser::JUMP_MNEMONICS.at(jumpMnemonic);
        return bits;  
    } catch (std::exception& e) {
        LOG << "Key '" << jumpMnemonic << "' not found in map";
        throw(e);
    }  
}

std::string Code::toBinary(uint16_t n) {
    if (n > MAX_INT) {
        throw std::runtime_error("Input '" + std::to_string(n) + "' is too large! "  
                                           + "Cannot represent numbers larger than " 
                                           + std::to_string(MAX_INT));
    }
    std::bitset<16> b(n);
    return b.to_string();
}

Parser::Parser() : m_lineno(0) {}

Parser::Parser(const std::string& filepath) : m_lineno(0) {
    loadFile(filepath);
    std::string outFile = filepath.substr(0, filepath.find(".asm")) + ".hack";
    m_outFile = std::ofstream(outFile);
    m_outputStream = &m_outFile;
    //m_outputStream = &std::cout; 
}

Parser::Parser(const std::string& inFile, const std::string& outFile) : m_lineno(0) {
    loadFile(inFile);
    m_outFile = std::ofstream(outFile);
    m_outputStream = &m_outFile;
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
    m_inFile = std::ifstream(filepath);
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

void Parser::reset() {
    m_inFile.clear();
    m_inFile.seekg(0, std::ios::beg);
    m_lineno = 0;
}

bool Parser::advance(){
    if (readLine(m_cmd)){
        ++m_lineno;
        return true;
    }
    return false;
}

bool Parser::hasMoreCommands() const{
    return (m_inFile.is_open() && !m_inFile.eof());
}

bool Parser::readLine(std::string& line){
    if (!m_inFile.is_open()){
        LOG << "Could not open file!";
        return false;
    }
    if (!std::getline(m_inFile, line)) {
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
    auto pos = s.find('=');
    dest = "";
    if ( pos == std::string::npos ){ // '=' not found
        return false;
    }
    dest = s.substr(0, pos);
    return true;
}

bool Parser::getJump(const std::string& s, std::string& jump) {
    auto pos = s.find(';');
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
    size_t destLength = (hasDest) ? (dest.length() + 1) : 0; // add one for '='
    size_t jumpLength = (hasJump) ? (jump.length() + 1) : 0; // add one for ';'
    size_t compLength = s.length() - destLength - jumpLength;
    comp = s.substr(destLength, compLength);
    return (
            (dest.empty() || Parser::isDestMnemonic(dest))
        &&  Parser::isCompMnemonic(comp)
        &&  (jump.empty() || Parser::isJumpMnemonic(jump))
    );
}

bool Parser::isDestMnemonic(const std::string& s) {
    return ( !s.empty() &&
             DEST_MNEMONICS.find(s) != DEST_MNEMONICS.end()
        );
}

bool Parser::isJumpMnemonic(const std::string& s) {
    return ( !s.empty() &&
             JUMP_MNEMONICS.find(s) != JUMP_MNEMONICS.end()
        );
}

bool Parser::isCompMnemonic(const std::string& s) {
    return ( !s.empty() &&
             COMP_MNEMONICS.find(s) != COMP_MNEMONICS.end()
        );
}

void Parser::buildSymbolTable() {
    std::string sym, dest, comp, jump, result;
    while (hasMoreCommands()) {
        advance();
        if (!sanitize(m_cmd)){ continue; }
        if ( isCommandA(m_cmd, sym) || isCommandC(m_cmd, dest, comp, jump) ) {
            ++m_romCounter;
        } else if ( isCommandL(m_cmd, sym) ) {
            m_symbolTable.addEntry(sym, m_romCounter);
        } else {
            LOG << "Failed to parse command: " << m_cmd;
            throw std::runtime_error("Unrecognized command type!");
        }
    }
    reset();
}

void Parser::run() {
    buildSymbolTable();
    //m_inFile.seekg(0);
    std::string sym, dest, comp, jump, result;
    while (hasMoreCommands()) {
        advance();
        if (!sanitize(m_cmd)){ continue; }
        if ( isCommandA(m_cmd, sym) ) {
            uint16_t d;
            if (!isNumeric(sym)) {
                d = static_cast<uint16_t>(m_symbolTable.getAddress(sym));
            } else {
                d = Parser::str2num(sym);
            }
            result = Code::toBinary(d);
        } else if ( isCommandC(m_cmd, dest, comp, jump) ) {
            result = "111" + Code::getComp(comp) + Code::getDest(dest) + Code::getJump(jump);
        } else if ( isCommandL(m_cmd, sym) ) {
            continue; // skip pseudocommand
        } else {
            LOG << "Failed to parse command: " << m_cmd;
            throw std::runtime_error("Unrecognized command type!");
        }
        if (!result.empty()){
            println(result);
        }
    }
}

uint16_t Parser::str2num(const std::string& s) {
    if ( Parser::isNumeric(s) ){
        return static_cast<uint16_t>(std::stoul(s));
    }
    LOG << "Cannot convert non-numeric '" << s << "' to decimal";
    throw std::runtime_error("Invalid numeric string!");
    return -1;
}

void Parser::println(const std::string& s) {
    *m_outputStream << s << std::endl;
    //std::cout << s << std::endl;
}

std::string Parser::getCmd() const{ return m_cmd; }
uint64_t Parser::getLineNumber() const { return m_lineno; }

SymbolTable::SymbolTable() {
    m_ramCounter = 0;
    
    addEntry("SP",      0);
    addEntry("LCL",     1);
    addEntry("ARG",     2);
    addEntry("THIS",    3);
    addEntry("THAT",    4);
    for (int i=0; i<16; i++) {
        addEntry("R" + std::to_string(i), i);
    }
    addEntry("SCREEN", 16384);
    addEntry("KBD", 24576);
    m_ramCounter = 16;
}

bool SymbolTable::addEntry(const std::string& key, int address) {
    auto result = m_map.insert(symbolEntry(key, address));
    return result.second;
}

bool SymbolTable::addVariable(const std::string& key) {
    if ( addEntry(key, m_ramCounter) ) {
        ++m_ramCounter;
        return true;
    }
    return false;
}

bool SymbolTable::contains(const std::string& key) const {
    return m_map.find(key) != m_map.end();
}

int SymbolTable::getAddress(const std::string& symbol) {
    if (!contains(symbol)) {
        addVariable(symbol);
    }
    return m_map.at(symbol);
}
