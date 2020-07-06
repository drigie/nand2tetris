#include "Parser.h"
#include <iostream>

using namespace Hack;

Parser::Parser() : m_eof(false), m_lineno(0) {}

Parser::Parser(const std::string& filepath) : m_eof(false), m_lineno(0) {
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
    return !m_eof;
}

bool Parser::readLine(std::string& line){
    if (!m_file.is_open()){
        LOG << "Could not open file!";
        return false;
    }
    if (!std::getline(m_file, line)) {
        m_eof = true;
        return false;
    }
    return true;
}

std::string Parser::getCmd() const{ return m_cmd; }
uint64_t Parser::getLineNumber() const { return m_lineno; }
