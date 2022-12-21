#include <string>
#include <tuple>
#include <vector>

using namespace std;

using Keyword = std::string;

const Keyword selectKeyword = "select";
const Keyword fromKeyword = "from";
const Keyword asKeyword = "as";
const Keyword tableKeyword = "table";
const Keyword createKeyword = "create";
const Keyword insertKeyword = "insert";
const Keyword intoKeyword = "into";
const Keyword valuesKeyword = "values";
const Keyword intKeyword = "int";
const Keyword textKeyword = "text";

using Symbol = std::string;

const Symbol semicolonSymbol = ";";
const Symbol asteriskSymbol = "*";
const Symbol commaSymbol = ",";
const Symbol leftParenSymbol = "(";
const Symbol rightParenSymbol = ")";

enum class TokenKind {
    KeywordKind,
    SymbolKind,
    IdentifierKind,
    StringKind,
    NumericKind,
};

struct Location {
    uint32_t line;
    uint32_t col;
};

struct Token {
    std::string value;
    TokenKind kind;
    Location loc;
};

struct Cursor {
    uint32_t pointer;
    Location loc;
};

using LexFunc =
    std::function<std::tuple<Token, Cursor, bool>(std::string, Cursor)>;

std::tuple<Token, Cursor, bool> lexKeyword(std::string source, Cursor ic);
std::tuple<Token, Cursor, bool> lexSymbol(std::string source, Cursor ic);
std::tuple<Token, Cursor, bool> lexString(std::string source, Cursor ic);
std::tuple<Token, Cursor, bool> lexNumberic(std::string source, Cursor ic);
std::tuple<Token, Cursor, bool> lexIdentifier(std::string source, Cursor ic);

class Lexer {
  public:
    Lexer();
    void AddLexer(LexFunc f);
    std::tuple<std::vector<Token>, std::string> Lex(std::string source);

  private:
    std::vector<std::string> tokens;
    std::vector<LexFunc> lex_funcs;
};
