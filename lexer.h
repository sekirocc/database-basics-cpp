#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

using Keyword = std::string;

const Keyword selectKeyword = "select";
const Keyword fromKeyword = "from";
const Keyword whereKeyword = "where";
const Keyword asKeyword = "as";
const Keyword tableKeyword = "table";
const Keyword createKeyword = "create";
const Keyword insertKeyword = "insert";
const Keyword intoKeyword = "into";
const Keyword valuesKeyword = "values";
const Keyword intKeyword = "int";
const Keyword textKeyword = "text";

using Symbol = std::string;

const Symbol equalSymbol = "=";
const Symbol semicolonSymbol = ";";
const Symbol asteriskSymbol = "*";
const Symbol commaSymbol = ",";
const Symbol leftParenSymbol = "(";
const Symbol rightParenSymbol = ")";

enum class TokenKind {
    WhitespaceKind,
    KeywordKind,
    SymbolKind,
    IdentifierKind,
    StringKind,
    NumericKind,
};

inline ostream& operator<<(ostream& out, const TokenKind& k) {
    switch (k) {
    case TokenKind::KeywordKind:
        out << "KeywordKind";
        break;
    case TokenKind::SymbolKind:
        out << "SymbolKind";
        break;
    case TokenKind::IdentifierKind:
        out << "IdentifierKind";
        break;
    case TokenKind::StringKind:
        out << "StringKind";
        break;
    case TokenKind::NumericKind:
        out << "NumericKind";
        break;
    default:
        out << "";
        break;
    }

    return out;
};

struct Location {
    int line;
    int col;
    Location() : line{0}, col{0} {};
};

struct Token {
    std::string value;
    TokenKind kind;
    Location loc;
};

inline ostream& operator<<(ostream& out, const Token& t) {
    out << "value: " << t.value << std::endl;
    out << "kind: " << t.kind << std::endl;
    out << "location: line " << t.loc.line << " col " << t.loc.col << std::endl;

    return out;
}

struct Cursor {
    uint32_t pointer;
    Location loc;
    Cursor() : pointer{0}, loc{} {};
};

using LexFunc = std::function<std::tuple<Token, Cursor, bool>(std::string, Cursor)>;

std::tuple<Token, Cursor, bool> lexKeyword(std::string source, Cursor ic);
std::tuple<Token, Cursor, bool> lexSymbol(std::string source, Cursor ic);
std::tuple<Token, Cursor, bool> lexString(std::string source, Cursor ic);
std::tuple<Token, Cursor, bool> lexNumberic(std::string source, Cursor ic);
std::tuple<Token, Cursor, bool> lexIdentifier(std::string source, Cursor ic);

std::tuple<Token, Cursor, bool> lexCharacterDelimited(std::string source, Cursor ic,
                                                      char delimiter);

class Lexer {
  public:
    Lexer();
    void AddLexer(LexFunc f);
    std::tuple<std::vector<Token>, std::string> Lex(std::string source);

  private:
    std::vector<std::string> tokens;
    std::vector<LexFunc> lex_funcs;
};
