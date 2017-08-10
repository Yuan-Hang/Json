#include "parse.h"
#include <cassert>
#include <cmath>      //HUGE_VAL
#include <cstdlib>    //strtod
#include <cstring>    //strncmp
#include <stdexcept>  //runtime_error
using namespace std;

namespace json {
constexpr bool is1to9(char ch) { return ch >= '1' && ch <= '9'; }
constexpr bool is0to9(char ch) { return ch >= '0' && ch <= '9'; }

Json Parser::parse() {
  parseWhitespace();
  Json json = parseValue();
  parseWhitespace();
  if (*curr_) error("ROOT NOT SINGULAR");
  return json;
}

Json Parser::parseValue() {
  switch (*curr_) {
  case 'n': return parseLiteral("null");
  case 't': return parseLiteral("true");
  case 'f': return parseLiteral("false");
  case '\"': return parseString();
  case '[': return parseArray();
  case '{': return parseObject();
  case '\0': error("EXPECT VALUE");
  default: return parseNumber();
  }
}

Json Parser::parseLiteral(const string& literal) {
  if (strncmp(curr_, literal.c_str(), literal.size())) error("INVALID VALUE");
  curr_ += literal.size();
  start_ = curr_;
  switch (literal[0]) {
  case 't': return Json(true);
  case 'f': return Json(false);
  default: return Json(nullptr);
  }
}

Json Parser::parseNumber() {
  if (*curr_ == '-') ++curr_;
  // integer
  if (*curr_ == '0')
    ++curr_;
  else {
    if (!is1to9(*curr_)) error("INVALID VALUE");
    while (is0to9(*++curr_))
      ;
  }
  // frac
  if (*curr_ == '.') {
    if (!is0to9(*++curr_)) error("INVALID VALUE");
    while (is0to9(*++curr_))
      ;
  }
  // exp
  if (toupper(*curr_) == 'E') {
    ++curr_;
    if (*curr_ == '-' || *curr_ == '+') ++curr_;
    if (!is0to9(*curr_)) error("INVALID VALUE");
    while (is0to9(*++curr_))
      ;
  }
  double val = strtod(start_, nullptr);
  if (fabs(val) == HUGE_VAL) error("NUMBER TOO BIG");
  start_ = curr_;
  return Json(val);
}

string Parser::parseRawString() {
  string str;
  while (1) {
    switch (*++curr_) {
    case '\"': start_ = ++curr_; return str;
    case '\0': error("MISS QUOTATION MARK");
    default:
      if (static_cast<unsigned char>(*curr_) < 0x20) error("INVALID STRING CHAR");
      str.push_back(*curr_);
      break;
    case '\\':
      switch (*++curr_) {
      case '\"': str.push_back('\"'); break;
      case '\\': str.push_back('\\'); break;
      case '/': str.push_back('/'); break;
      case 'b': str.push_back('\b'); break;
      case 'f': str.push_back('\f'); break;
      case 'n': str.push_back('\n'); break;
      case 't': str.push_back('\t'); break;
      case 'r': str.push_back('\r'); break;
      case 'u': {
        unsigned u1 = parse4hex();
        if (u1 >= 0xd800 && u1 <= 0xdbff) {  // high surrogate
          if (*++curr_ != '\\') error("INVALID UNICODE SURROGATE");
          if (*++curr_ != 'u') error("INVALID UNICODE SURROGATE");
          unsigned u2 = parse4hex();  // low surrogate
          if (u2 < 0xdc00 || u2 > 0xdfff) error("INVALID UNICODE SURROGATE");
          u1 = (((u1 - 0xd800) << 10) | (u2 - 0xdc00)) + 0x10000;
        }
        str += encodeUTF8(u1);
      } break;
      default: error("INVALID STRING ESCAPE");
      }
      break;
    }
  }
}

unsigned Parser::parse4hex() {
  unsigned u = 0;
  for (int i = 0; i != 4; ++i) {
    // now *curr_ = "uXXXX...."
    unsigned ch = static_cast<unsigned>(toupper(*++curr_));
    u <<= 4;  // u *= 16
    if (ch >= '0' && ch <= '9')
      u |= (ch - '0');
    else if (ch >= 'A' && ch <= 'F')
      u |= ch - 'A' + 10;
    else
      error("INVALID UNICODE HEX");
  }
  return u;
}

string Parser::encodeUTF8(unsigned u) noexcept {
  string utf8;
  if (u <= 0x7F)  // 0111,1111
    utf8.push_back(static_cast<char>(u & 0xff));
  else if (u <= 0x7FF) {
    utf8.push_back(static_cast<char>(0xc0 | ((u >> 6) & 0xff)));
    utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
  } else if (u <= 0xFFFF) {
    utf8.push_back(static_cast<char>(0xe0 | ((u >> 12) & 0xff)));
    utf8.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3f)));
    utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
  } else {
    assert(u <= 0x10FFFF);
    utf8.push_back(static_cast<char>(0xf0 | ((u >> 18) & 0xff)));
    utf8.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3f)));
    utf8.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3f)));
    utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
  }
  return utf8;
}

Json Parser::parseArray() {
  Json::array_t arr;
  ++curr_;  // skip '['
  parseWhitespace();
  if (*curr_ == ']') {
    start_ = ++curr_;
    return Json(arr);
  }
  while (1) {
    parseWhitespace();
    arr.push_back(parseValue());
    parseWhitespace();
    if (*curr_ == ',')
      ++curr_;
    else if (*curr_ == ']') {
      start_ = ++curr_;
      return Json(arr);
    } else
      error("MISS COMMA OR SQUARE BRACKET");
  }
}

Json Parser::parseObject() {
  Json::object_t obj;
  ++curr_;
  parseWhitespace();
  if (*curr_ == '}') {
    start_ = ++curr_;
    return Json(obj);
  }
  while (1) {
    parseWhitespace();
    if (*curr_ != '"') error("MISS KEY");
    string key = parseRawString();
    parseWhitespace();
    if (*curr_++ != ':') error("MISS COLON");
    parseWhitespace();
    Json val = parseValue();
    obj.insert({key, val});
    parseWhitespace();
    if (*curr_ == ',')
      ++curr_;
    else if (*curr_ == '}') {
      start_ = ++curr_;
      return Json(obj);
    } else
      error("MISS COMMA OR CURLY BRACKET");
  }
}

void Parser::parseWhitespace() noexcept {
  while (*curr_ == ' ' || *curr_ == '\t' || *curr_ == '\r' || *curr_ == '\n') ++curr_;
  start_ = curr_;
}
}  // namespace json
