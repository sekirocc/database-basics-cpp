#include "lexer.h"
#include <cstddef>
#include <string.h>

//
//
// Lex Functions
//
//

std::tuple<Token, Cursor, bool>
lexCharacterDelimited(std::string source, Cursor ic, char delimiter) {
    Cursor cur = ic;
    if (ic.pointer == source.size() - 1) {
        return {{}, cur, false};
    };
    if (source[ic.pointer] != delimiter) {
        return {{}, cur, false};
    }

    cur.pointer++;
    cur.loc.col++;

    std::vector<char> collected;
    for (; cur.pointer < source.size(); cur.pointer++) {
        char c = source[cur.pointer];

        if (c == delimiter) {
            if (cur.pointer >= source.size() - 1 ||
                source[cur.pointer + 1] == delimiter) {
                Token t{
                    .value = std::string(collected.begin(), collected.end()),
                    .loc = cur.loc,
                    .kind = TokenKind::StringKind,
                };
                return {t, cur, true};
            }
        }

        collected.push_back(c);
    }
    return {{}, cur, false};
};
std::tuple<Token, Cursor, bool> lexKeyword(std::string source, Cursor ic) {
    return {};
};
std::tuple<Token, Cursor, bool> lexSymbol(std::string source, Cursor ic) {
    return {};
};
std::tuple<Token, Cursor, bool> lexString(std::string source, Cursor ic) {
    return lexCharacterDelimited(source, ic, '\'');
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
                continue;
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
            break;
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

//
//
// Lexer
//
//

Lexer::Lexer(){};

void Lexer::AddLexer(LexFunc f) { lex_funcs.push_back(f); };

std::tuple<std::vector<Token>, std::string> Lexer::Lex(std::string source) {
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

        // print log
        return {{}, hint};
    }

    return {tokens, ""};
};
