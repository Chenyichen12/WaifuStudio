#pragma once
#include "parser.h"
namespace WaifuL2d {
class PsdParser : public IParser {
 private:
  std::wstring path;
  std::unique_ptr<ParseResult> result;

 public:
  PsdParser(const std::wstring& path);
  void parse() override;

  /**
   * root type = -1
   * bitmap type = 1
   * group type = 2
   */
  ParseResult* getResult() override;
};
}  // namespace WaifuL2d