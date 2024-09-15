#pragma once
#include "parser.h"
namespace WaifuL2d {
class PsdParser : public IParser {
 private:
  std::string path;
  std::unique_ptr<ParseResult> result;

 public:
  PsdParser(const std::string& path);
  void parse() override;

  /**
   * root type = -1
   * bitmap type = 1
   * group type = 2
   */
  ParseResult* getResult() override;
};
}  // namespace WaifuL2d