#include "lexer.h"

#include <iostream>

using namespace std;

int main(void) {
    Lexer lexer;

    lexer.AddLexer(lexKeyword);
    lexer.AddLexer(lexSymbol);
    lexer.AddLexer(lexString);
    lexer.AddLexer(lexNumberic);
    lexer.AddLexer(lexIdentifier);

    std::string sql = "select * from users where user_id = 100";

    auto [tokens, hint] = lexer.Lex(sql);
    std::cout << "hint: " << hint << std::endl << std::endl;
    if (!tokens.empty()) {
        for (const auto& t : tokens) {
            std::cout << t << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
