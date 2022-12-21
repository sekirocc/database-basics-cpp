#include "lexer.h"
#include <cstddef>
#include <string.h>
#include <unordered_set>

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

std::string longestMatch(std::string source, Cursor ic,
                         std::vector<std::string> options) {
    Cursor cur = ic;
    std::string matched;
    std::unordered_set<size_t> skipList;

    for (; cur.pointer < source.size(); cur.pointer++) {
        std::string substr =
            source.substr(ic.pointer, cur.pointer - ic.pointer);

        for (size_t idx = 0; idx < options.size(); idx++) {
            // this option is already skipped
            if (skipList.find(idx) != skipList.end()) {
                continue;
            }

            const auto &option = options[idx];

            // this substr already longer than the option, so skip this option.
            if (substr.size() > option.size()) {
                skipList.insert(idx);
                continue;
            }

            // this substr is already not part of option, so skip this option.
            if (option.rfind(substr, 0) != 0) {
                skipList.insert(idx);
                continue;
            }

            if (substr == option) {
                skipList.insert(idx);
                if (substr.size() > matched.size()) {
                    matched = substr;
                }
                continue;
            }
        }
    }

    return matched;
};

std::tuple<Token, Cursor, bool> lexKeyword(std::string source, Cursor ic) {
    Cursor cur = ic;
    std::vector<std::string> options{
        selectKeyword, fromKeyword, asKeyword,     tableKeyword, createKeyword,
        insertKeyword, intoKeyword, valuesKeyword, intKeyword,   textKeyword,
    };

    std::string match = longestMatch(source, ic, options);
    if (match.size() == 0) {
        return {{}, ic, false};
    }

    cur.pointer = ic.pointer + match.size();
    cur.loc.col = ic.loc.col + match.size();

    Token t{
        .value = match,
        .loc = cur.loc,
        .kind = TokenKind::StringKind,
    };

    return {t, cur, true};
};

std::tuple<Token, Cursor, bool> lexSymbol(std::string source, Cursor ic) {
    Cursor cur = ic;

    char c = source[cur.pointer];

    cur.pointer++;
    cur.loc.col++;

    switch (c) {
    case '\n':
        // new line.
        cur.loc.col = 0;
        cur.loc.line++;
    case '\t':
    case ' ':
        return {{}, cur, false};
    };

    std::vector<std::string> options{
        semicolonSymbol, asteriskSymbol,   commaSymbol,
        leftParenSymbol, rightParenSymbol,
    };
    std::string match = longestMatch(source, ic, options);

    if (match.size() == 0) {
        return {{}, cur, false};
    }

    cur.pointer = ic.pointer + match.size();
    cur.loc.col = ic.loc.col + match.size();

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
    auto [t, c, ok] = lexCharacterDelimited(source, ic, "\"");
    if (ok) {
        return {t, c, ok};
    }

    // todo
    // https://notes.eatonphil.com/database-basics.html

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
