#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <map>
#define NO_PATH_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_PREAMBLE std::endl << NO_PATH_FILE << ":" << __func__ << ":" << __LINE__ << " - "
#define LOG std::cout << LOG_PREAMBLE
#define LOG_ERR std::cerr << LOG_PREAMBLE

enum class CommandType{
    A_COMMAND,
    C_COMMAND,
    L_COMMAND,
    BAD_COMMAND
};

namespace Hack{

using charset = std::vector<char>;
using stringset = std::vector<std::string>;
using stringMap = std::map<std::string, std::string>;

class Parser{
public:
Parser();
Parser(const std::string& filepath);
void loadFile(const std::string& filepath);
bool hasMoreCommands() const; 
bool advance(); /// reads next command and assigns to m_cmd
CommandType getCommandType() const; 
std::string getSymbol(CommandType t) const; 
std::string getDest() const;
std::string getComp() const;
std::string getJump() const;
std::string getCmd() const;
uint64_t getLineNumber() const;
static bool isCommandA(const std::string& s, std::string& sym);
static bool isCommandL(const std::string& s, std::string& sym);
static bool isCommandC(const std::string& s,
                       std::string& dest, 
                       std::string& comp, 
                       std::string& jump);
static bool isDigit(const char c);
static bool isNumeric(const std::string& s);
static bool isSymbol(const std::string& s);
static bool isDestMnemonic(const char c);
static bool isDestMnemonic(const std::string& s);
static bool isJumpMnemonic(const std::string& s);
static bool isCompMnemonic(const std::string& s);
static const stringMap DEST_MNEMONICS;
static const stringMap JUMP_MNEMONICS;
static const stringMap COMP_MNEMONICS;
void run();

private:
std::string m_cmd; /// current command 
std::ifstream m_file; /// input filestream
bool readLine(std::string& line);
uint64_t m_lineno = 0;
static bool isSymbolChar(const char x);
static bool getDest(const std::string& s, std::string& dest);
static bool getJump(const std::string& s, std::string& dest);
static std::string commandTypeString(const CommandType t);

template <class T>
static bool any_of(T v, std::vector<T> allowed_vals){
    return std::any_of(allowed_vals.begin(), allowed_vals.end(), 
                       [v](const T x){ return (v==x); });
}
static bool sanitize(std::string& cmd); /// remove spaces and comments (true if result not empty)
static const charset SYMBOL_CHARS;
static const charset DEST_CHARS;
};

namespace Code{
const uint16_t MAX_INT = 32767u; // 2^15 - 1 is maximum integer representable by 15 bits
std::string getDest(const std::string& destMnemonic);
std::string getComp(const std::string& compMnemonic);
std::string getJump(const std::string& jumpMnemonic);
std::string toBinary(uint16_t n);
}
}
