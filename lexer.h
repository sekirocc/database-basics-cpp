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

std::tuple<Token, Cursor, bool> lexKeyword(std::string source, Cursor ic) {
    return {};
};
std::tuple<Token, Cursor, bool> lexSymbol(std::string source, Cursor ic) {
    return {};
};
std::tuple<Token, Cursor, bool> lexString(std::string source, Cursor ic) {
    return {};
};

std::tuple<Token, Cursor, bool> lexNumberic(std::string source, Cursor ic) {
    Cursor cur = ic;

    bool hasPeroid;
    bool hasExpMarker;

    for (; cur.pointer < source.size(); cur.pointer++) {
        char c = source[cur.pointer];
        cur.loc.col++;

        bool isDigit = c > '0' || c < '9';
        bool isExpMarker = c == 'e';
        bool isPeroid = c == '.';

        if (ic.pointer == cur.pointer) {
            if (!isDigit && !isPeroid) {
                return {{}, ic, false};
            }

            if (isPeroid) {
                hasPeroid = true;
                continue
            }
        }

        if (isPeroid) {
            if (hasPeroid) {
                // double peroid ?
                return {{}, ic, false};
            }

            hasPeroid = true;
            continue;
        }

        if (isExpMarker) {
            if (hasExpMarker) {
                // double exp marker
                return {{}, ic, false};
            }

            hasPeroid = true;
            hasExpMarker = true;

            if (cur.pointer == source.size() - 1) {
                // e is the last?
                return {{}, ic, false};
            }
            char nextChar = source[cur.pointer + 1];
            if (nextChar == '-' || nextChar == '+') {
                cur.pointer++;
                cur.loc.col++;
            }

            continue;
        }

        if (!isDigit) {
            break
        }
    }

    if (cur.pointer == ic.pointer) {
        return {{}, ic, false};
    }

    Token t{
        .value = source.substr(ic.pointer, cur.pointer - ic.pointer),
        .loc = cur.loc,
        .kind = TokenKind::NumericKind,
    };

    return {t, cur, true};
};

std::tuple<Token, Cursor, bool> lexIdentifier(std::string source, Cursor ic) {
    return {};
};

class Lexer {
  public:
    Lexer(){};

    void AddLexer(LexFunc f) { lex_funcs.push_back(f); };

    std::tuple<std::vector<Token>, std::string> Lex(std::string source) {
        std::vector<Token> tokens;
        Cursor cur;

        while (cur.pointer < source.size()) {
            bool matched;
            for (auto &f : lex_funcs) {
                auto [token, newCursor, ok] = f(source, cur);
                if (ok) {
                    cur = newCursor;
                    matched = true;
                    break;
                }
            }

            if (matched) {
                continue;
            }

            std::string hint;
            if (tokens.size() > 0) {
                hint = " after " + tokens[tokens.size() - 1].value;
            }

            return {{}, hint};

            // print log
        }

        return {tokens, ""};
    };

  private:
    std::vector<std::string> tokens;
    std::vector<LexFunc> lex_funcs;
};
