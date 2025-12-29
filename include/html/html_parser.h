#include "html/node.h"
#include "html/token.h"

std::shared_ptr<Node> parse(const std::vector<Token> &tokens);
std::shared_ptr<Node> create_node(const Token &token);