#include <gtest/gtest.h>
#include <string>
#include "../json.h"
using namespace json;
using namespace std;

Json parseOk(const string& strJson) {
  string errMsg;
  Json _1 = Json::parse(strJson, errMsg);
  EXPECT_EQ(errMsg, "");
  return _1;
}

void testError(const string& expect, const string& strJson) {
  string errMsg;
  Json _1 = Json::parse(strJson, errMsg);
  auto pos = errMsg.find_first_of(":");
  auto actual = errMsg.substr(0, pos);
  EXPECT_EQ(actual, expect);
}

void testRoundtrip(const string& expect) {
  Json _1 = parseOk(expect);
  string actual = _1.serialize();
  if (_1.isNumber())
    EXPECT_EQ(strtod(actual.c_str(), nullptr), strtod(expect.c_str(), nullptr));
  else
    EXPECT_EQ(actual, expect);
}

void testNull(const string& strJson) {
  Json _1 = parseOk(strJson);
  EXPECT_TRUE(_1.isNull());
}

void testBool(bool expect, const string& content) {
  Json _1 = parseOk(content);
  EXPECT_TRUE(_1.isBool());
  EXPECT_EQ(_1.asBool(), expect);
  _1 = Json(!expect);
  EXPECT_EQ(_1.asBool(), !expect);
}

void testNumber(double expect, const string& strJson) {
  Json _1 = parseOk(strJson);
  EXPECT_TRUE(_1.isNumber());
  EXPECT_EQ(_1.asDouble(), expect);
}

void testString(const string& expect, const string& strJson) {
  Json _1 = parseOk(strJson);
  EXPECT_TRUE(_1.isString());
  EXPECT_EQ(_1.asString(), expect);
}

TEST(Str2Json, JsonNull) {
  testNull("null");
  testNull("   null\n\r\t");
}

TEST(Str2Json, JsonBool) {
  testBool(true, "true");
  testBool(false, "false");
}

TEST(Str2Json, JsonNumber) {
  testNumber(0.0, "0");
  testNumber(0.0, "-0");
  testNumber(0.0, "-0.0");
  testNumber(1.0, "1");
  testNumber(-1.0, "-1");
  testNumber(1.5, "1.5");
  testNumber(-1.5, "-1.5");
  testNumber(3.1416, "3.1416");
  testNumber(1E10, "1E10");
  testNumber(1e10, "1e10");
  testNumber(1E+10, "1E+10");
  testNumber(1E-10, "1E-10");
  testNumber(-1E10, "-1E10");
  testNumber(-1e10, "-1e10");
  testNumber(-1E+10, "-1E+10");
  testNumber(-1E-10, "-1E-10");
  testNumber(1.234E+10, "1.234E+10");
  testNumber(1.234E-10, "1.234E-10");
  testNumber(5.0E-324, "5e-324");
  testNumber(0, "1e-10000");
  testNumber(1.0000000000000002, "1.0000000000000002");
  testNumber(4.9406564584124654e-324, "4.9406564584124654e-324");
  testNumber(-4.9406564584124654e-324, "-4.9406564584124654e-324");
  testNumber(2.2250738585072009e-308, "2.2250738585072009e-308");
  testNumber(-2.2250738585072009e-308, "-2.2250738585072009e-308");
  testNumber(2.2250738585072014e-308, "2.2250738585072014e-308");
  testNumber(-2.2250738585072014e-308, "-2.2250738585072014e-308");
  testNumber(1.7976931348623157e+308, "1.7976931348623157e+308");
  testNumber(-1.7976931348623157e+308, "-1.7976931348623157e+308");
  string errMsg;
  Json _1 = Json::parse("1.2e+12", errMsg);
  EXPECT_TRUE(_1.isNumber());
  _1 = Json(3.1415);
  EXPECT_EQ(3.1415, _1.asDouble());
}

TEST(Str2Json, JsonString) {
  testString("", "\"\"");
  testString("Hello", "\"Hello\"");
  testString("Hello\nWorld", "\"Hello\\nWorld\"");
  testString("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
  string s = "Hello";
  s.push_back('\0');
  s += "World";
  testString(s, "\"Hello\\u0000World\"");
  testString("\x24", "\"\\u0024\"");
  testString("\xC2\xA2", "\"\\u00A2\"");
  testString("\xE2\x82\xAC", "\"\\u20AC\"");
  testString("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");
  testString("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");
  string errMsg;
  Json _1 = Json::parse("\"something\"", errMsg);
  _1 = Json("another thing");
  EXPECT_EQ(_1.asString(), "another thing");
}

TEST(Str2Json, JsonArray) {
  Json _1 = parseOk("[ ]");
  EXPECT_TRUE(_1.isArray());
  EXPECT_EQ(_1.size(), 0);

  Json _2 = parseOk("[ null , false , true , 123 , \"abc\" ]");
  EXPECT_TRUE(_2.isArray());
  EXPECT_EQ(_2.size(), 5);
  EXPECT_EQ(_2[0], Json(nullptr));
  EXPECT_EQ(_2[1], Json(false));
  EXPECT_EQ(_2[2], Json(true));
  EXPECT_EQ(_2[3], Json(123.0));
  EXPECT_EQ(_2[4], Json("abc"));

  Json _3 = parseOk("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
  EXPECT_TRUE(_3.isArray());
  EXPECT_EQ(_3.size(), 4);

  EXPECT_TRUE(_3[0].isArray());
  EXPECT_EQ(_3[0].size(), 0);

  EXPECT_TRUE(_3[1].isArray());
  EXPECT_EQ(_3[1].size(), 1);
  EXPECT_EQ(_3[1][0].asDouble(), 0);

  EXPECT_TRUE(_3[2].isArray());
  EXPECT_EQ(_3[2].size(), 2);
  EXPECT_EQ(_3[2][0].asDouble(), 0);
  EXPECT_EQ(_3[2][1].asDouble(), 1);

  EXPECT_TRUE(_3[3].isArray());
  EXPECT_EQ(_3[3].size(), 3);
  EXPECT_EQ(_3[3][0].asDouble(), 0);
  EXPECT_EQ(_3[3][1].asDouble(), 1);
  EXPECT_EQ(_3[3][2].asDouble(), 2);
}

TEST(Str2Json, JsonObject) {
  Json _1 = parseOk("{ }");
  EXPECT_TRUE(_1.isObject());
  EXPECT_EQ(_1.size(), 0);

  Json _2 = parseOk(
      " { "
      "\"n\" : null , "
      "\"f\" : false , "
      "\"t\" : true , "
      "\"i\" : 123 , "
      "\"s\" : \"abc\", "
      "\"a\" : [ 1, 2, 3 ],"
      "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
      " } ");
  EXPECT_TRUE(_2.isObject());
  EXPECT_EQ(_2.size(), 7);

  EXPECT_TRUE(_2["n"].isNull());

  EXPECT_TRUE(_2["f"].isBool());
  EXPECT_EQ(_2["f"].asBool(), false);

  EXPECT_TRUE(_2["t"].isBool());
  EXPECT_EQ(_2["t"].asBool(), true);

  EXPECT_TRUE(_2["i"].isNumber());
  EXPECT_EQ(_2["i"].asDouble(), 123.0);

  EXPECT_TRUE(_2["s"].isString());
  EXPECT_EQ(_2["s"].asString(), "abc");

  EXPECT_TRUE(_2["a"].isArray());
  EXPECT_EQ(_2["a"].size(), 3);

  EXPECT_TRUE(_2["o"].isObject());
  EXPECT_EQ(_2["o"].size(), 3);
}

TEST(Error, ExpectValue) {
  testError("EXPECT VALUE", "");
  testError("EXPECT VALUE", " ");
}

TEST(Error, InvalidValue) {
  testError("INVALID VALUE", "nul");
  testError("INVALID VALUE", "?");
  testError("INVALID VALUE", "+0");
  testError("INVALID VALUE", "+1");
  testError("INVALID VALUE", ".123");
  testError("INVALID VALUE", "1.");
  testError("INVALID VALUE", "inf");
  testError("INVALID VALUE", "INF");
  testError("INVALID VALUE", "NAN");
  testError("INVALID VALUE", "nan");
  testError("INVALID VALUE", "[1,]");
  testError("INVALID VALUE", "[\"a\", nul]");
}

TEST(Error, RootNotSingular) {
  testError("ROOT NOT SINGULAR", "null x");
  testError("ROOT NOT SINGULAR", "0123");
  testError("ROOT NOT SINGULAR", "0x0");
  testError("ROOT NOT SINGULAR", "0x123");
}

TEST(Error, NumberTooBig) {
  testError("NUMBER TOO BIG", "1e309");
  testError("NUMBER TOO BIG", "-1e309");
}

TEST(Error, MissQuotationMark) {
  testError("MISS QUOTATION MARK", "\"");
  testError("MISS QUOTATION MARK", "\"abc");
}

TEST(Error, InvalidStringEscape) {
  testError("INVALID STRING ESCAPE", "\"\\v\"");
  testError("INVALID STRING ESCAPE", "\"\\'\"");
  testError("INVALID STRING ESCAPE", "\"\\0\"");
  testError("INVALID STRING ESCAPE", "\"\\x12\"");
}

TEST(Error, InvalidStringChar) {
  testError("INVALID STRING CHAR", "\"\x01\"");
  testError("INVALID STRING CHAR", "\"\x1F\"");
}

TEST(Error, InvalidUnicodeHex) {
  testError("INVALID UNICODE HEX", "\"\\u\"");
  testError("INVALID UNICODE HEX", "\"\\u0\"");
  testError("INVALID UNICODE HEX", "\"\\u01\"");
  testError("INVALID UNICODE HEX", "\"\\u012\"");
  testError("INVALID UNICODE HEX", "\"\\u/000\"");
  testError("INVALID UNICODE HEX", "\"\\uG000\"");
  testError("INVALID UNICODE HEX", "\"\\u0/00\"");
  testError("INVALID UNICODE HEX", "\"\\u0G00\"");
  testError("INVALID UNICODE HEX", "\"\\u000/\"");
  testError("INVALID UNICODE HEX", "\"\\u00G/\"");
  testError("INVALID UNICODE HEX", "\"\\u 123/\"");
}

TEST(Error, InvalidUnicodeSurrogate) {
  testError("INVALID UNICODE SURROGATE", "\"\\uD800\"");
  testError("INVALID UNICODE SURROGATE", "\"\\uDBFF\"");
  testError("INVALID UNICODE SURROGATE", "\"\\uD800\\\\\\");
  testError("INVALID UNICODE SURROGATE", "\"\\uD800\\uDBFF\"");
  testError("INVALID UNICODE SURROGATE", "\"\\uD800\\uE000\"");
}

TEST(Error, MissCommaOrSquareBracket) {
  testError("MISS COMMA OR SQUARE BRACKET", "[1");
  testError("MISS COMMA OR SQUARE BRACKET", "[1}");
  testError("MISS COMMA OR SQUARE BRACKET", "[1 2");
  testError("MISS COMMA OR SQUARE BRACKET", "[[]");
}

TEST(Error, MissKey) {
  testError("MISS KEY", "{:1,");
  testError("MISS KEY", "{1:1,");
  testError("MISS KEY", "{true:1,");
  testError("MISS KEY", "{false:1,");
  testError("MISS KEY", "{null:1,");
  testError("MISS KEY", "{[]:1,");
  testError("MISS KEY", "{{}:1,");
  testError("MISS KEY", "{\"a\":1,");
}

TEST(Error, MissColon) {
  testError("MISS COLON", "{\"a\"}");
  testError("MISS COLON", "{\"a\",\"b\"}");
}

TEST(Error, MissCommaOrCurlyBracket) {
  testError("MISS COMMA OR CURLY BRACKET", "{\"a\":1");
  testError("MISS COMMA OR CURLY BRACKET", "{\"a\":1]");
  testError("MISS COMMA OR CURLY BRACKET", "{\"a\":1 \"b\"");
  testError("MISS COMMA OR CURLY BRACKET", "{\"a\":{}");
}

TEST(Json, Ctor) {
  {
    Json _1(nullptr);
    EXPECT_TRUE(_1.isNull());
  }
  {
    Json _1(true);
    EXPECT_TRUE(_1.isBool());
    EXPECT_EQ(_1.asBool(), true);

    Json _2(false);
    EXPECT_TRUE(_2.isBool());
    EXPECT_EQ(_2.asBool(), false);
  }
  {
    Json _1(0);
    EXPECT_TRUE(_1.isNumber());
    EXPECT_EQ(_1.asDouble(), 0);

    Json _2(100.1);
    EXPECT_TRUE(_2.isNumber());
    EXPECT_EQ(_2.asDouble(), 100.1);
  }
  {
    Json _1("hello");
    EXPECT_TRUE(_1.isString());
    EXPECT_EQ(_1.asString(), "hello");
  }
  {
    vector<Json> arr{Json(nullptr), Json(true), Json(1.2)};
    Json _1(arr);
    EXPECT_TRUE(_1.isArray());
    EXPECT_TRUE(_1[0].isNull());
  }
  {
    unordered_map<string, Json> obj;
    obj.insert({"hello", Json(nullptr)});
    obj.insert({"world", Json("!!")});
    Json _1(obj);
    EXPECT_TRUE(_1.isObject());
    EXPECT_TRUE(_1["world"].isString());
  }
}

TEST(Json2Str, literal) {
  testRoundtrip("null");
  testRoundtrip("true");
  testRoundtrip("false");
}

TEST(Json2Str, JsonNumber) {
  testRoundtrip("0");
  testRoundtrip("-0");
  testRoundtrip("1");
  testRoundtrip("-0");
  testRoundtrip("1.5");
  testRoundtrip("-1.5");
  testRoundtrip("3.25");
  testRoundtrip("1e+20");
  testRoundtrip("1.234e+20");
  testRoundtrip("1.234e-20");
  testRoundtrip("1.0000000000000002");
  testRoundtrip("4.9406564584124654e-324");
  testRoundtrip("-4.9406564584124654e-324");
  testRoundtrip("2.2250738585072009e-308");
  testRoundtrip("-2.2250738585072009e-308");
  testRoundtrip("2.2250738585072014e-308");
  testRoundtrip("-2.2250738585072014e-308");
  testRoundtrip("1.7976931348623157e+308");
  testRoundtrip("-1.7976931348623157e+308");
}

TEST(Json2Str, JsonString) {
  testRoundtrip("\"\"");
  testRoundtrip("\"Hello\"");
  testRoundtrip("\"Hello\\nWorld\"");
  testRoundtrip("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
  testRoundtrip("\"Hello\\u0000World\"");
}

TEST(Json2Str, JsonArray) {
  testRoundtrip("[  ]");
  testRoundtrip("[ null, false, true, 123, \"abc\", [ 1, 2, 3 ] ]");
}

TEST(Json2Str, JsonObject) {
  testRoundtrip("{  }");
  testRoundtrip(
      R"({ "o": { "3": 3, "2": 2, "1": 1 }, "a": [ 1, 2, 3 ], "s": "abc", "n": null, "f": false, "t": true, "i": 123 })");
}
