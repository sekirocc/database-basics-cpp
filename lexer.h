#include <_types/_uint32_t.h>
#include <string>
#include <vector>

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
    NumbericKind,
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

class Lexer {
  public:
    Lexer(){};

    AddLexer(LexFunc f) { lex_funcs.push_back(f); };

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
                hint = " after " + tokens[tokens.size() - 1].value
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
