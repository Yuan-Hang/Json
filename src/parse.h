#pragma once
#include "json.h"
#include "jsonException.h"
#include "uncopyable.h"

namespace json {

class Parser final : Uncopyable {
public:
  Parser(const std::string& content) noexcept : start_(content.c_str()), curr_(content.c_str()) {}

  Json parse();

private:
  Json parseValue();
  Json parseLiteral(const std::string& literal);
  Json parseNumber();
  Json parseString() { return Json(parseRawString()); }
  std::string parseRawString();
  unsigned parse4hex();
  std::string encodeUTF8(unsigned u) noexcept;
  Json parseArray();
  Json parseObject();
  void parseWhitespace() noexcept;

  [[noreturn]] void error(const std::string& msg) const { throw JsonException(msg + ": " + start_); }

  const char* start_;
  const char* curr_;
};
}  // namespace json
