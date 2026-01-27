#include "html/node.h"
#include "html/token.h"

std::shared_ptr<NODE> parse(const std::vector<TOKEN> &tokens);
std::shared_ptr<NODE> create_node(const TOKEN &token);