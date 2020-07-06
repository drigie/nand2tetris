#pragma once
#include <string>
#include <fstream>
#define NO_PATH_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_PREAMBLE std::endl << NO_PATH_FILE << ":" << __func__ << ":" << __LINE__ << " - "
#define LOG std::cout << LOG_PREAMBLE
#define LOG_ERR std::cerr << LOG_PREAMBLE

enum class CommandType{
    A_COMMAND,
    C_COMMAND,
    L_COMMAND
};

namespace Hack{

class Parser{
public:
Parser();
Parser(const std::string& filepath);
bool hasMoreCommands() const; 
bool advance(); /// reads next command and assigns to m_cmd
CommandType getCommandType() const; 
std::string getSymbol() const; 
std::string getDest() const;
std::string getComp() const;
std::string getJump() const;
std::string getCmd() const;
uint64_t getLineNumber() const;

private:
std::string m_cmd; /// current command 
std::ifstream m_file; /// input filestream
bool readLine(std::string& line);
bool m_eof = false; /// true if end of file is reached
uint64_t m_lineno = 0;
};

namespace Code{
std::string getDest(std::string destMnemonic);
std::string getComp(std::string compMnemonic);
std::string getJump(std::string jumpMnemonic);
}

}
