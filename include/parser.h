#include "node.h"
#include "token.h"

std::shared_ptr<Node> parse(const std::vector<Token> &tokens);
std::shared_ptr<Node> create_node(const Token &token);