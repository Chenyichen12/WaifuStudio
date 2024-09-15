#include "parser.h"

WaifuL2d::TreeNode::~TreeNode() {
  for (auto child : children) {
    delete child;
  }
}

WaifuL2d::ParseResult::~ParseResult() {
    delete root;
}
