#include "json.h"
#include <cassert>
#include "jsonValue.h"
#include "parse.h"
using namespace std;

namespace json {

Json::Json(nullptr_t) : value_(make_unique<JsonNull>(nullptr)) {}
Json::Json(bool val) : value_(make_unique<JsonBool>(val)) {}
Json::Json(double val) : value_(make_unique<JsonDouble>(val)) {}
Json::Json(const string& val) : value_(make_unique<JsonString>(val)) {}
Json::Json(const array_t& val) : value_(make_unique<JsonArray>(val)) {}
Json::Json(const object_t& val) : value_(make_unique<JsonObject>(val)) {}

Json::Json(const Json& rhs) {
  switch (rhs.type()) {
  case JsonType::kNull: value_ = make_unique<JsonNull>(nullptr); break;
  case JsonType::kBool: value_ = make_unique<JsonBool>(rhs.toBool()); break;
  case JsonType::kNumber: value_ = make_unique<JsonDouble>(rhs.toDouble()); break;
  case JsonType::kString: value_ = make_unique<JsonString>(rhs.toString()); break;
  case JsonType::kArray: value_ = make_unique<JsonArray>(rhs.toArray()); break;
  case JsonType::kObject: value_ = make_unique<JsonObject>(rhs.toObject()); break;
  }
}

Json::Json(Json&& rhs) noexcept : value_(std::move(rhs.value_)) { rhs.value_ = nullptr; }

Json::~Json() {}

Json& Json::operator=(Json rhs) {
  swap(rhs);
  return *this;
}

void Json::swap(Json& rhs) noexcept {
  using std::swap;
  swap(value_, rhs.value_);
}

JsonType Json::type() const noexcept { return value_->type(); }
bool Json::isNull() const noexcept { return type() == JsonType::kNull; }
bool Json::isBool() const noexcept { return type() == JsonType::kBool; }
bool Json::isNumber() const noexcept { return type() == JsonType::kNumber; }
bool Json::isString() const noexcept { return type() == JsonType::kString; }
bool Json::isArray() const noexcept { return type() == JsonType::kArray; }
bool Json::isObject() const noexcept { return type() == JsonType::kObject; }

bool Json::toBool() const { return value_->toBool(); }
double Json::toDouble() const { return value_->toDouble(); }
const string& Json::toString() const { return value_->toString(); }
const Json::array_t& Json::toArray() const { return value_->toArray(); }
const Json::object_t& Json::toObject() const { return value_->toObject(); }

Json& Json::operator[](size_t i) { return value_->operator[](i); }
const Json& Json::operator[](size_t i) const { return value_->operator[](i); }
Json& Json::operator[](const string& i) { return value_->operator[](i); }
const Json& Json::operator[](const string& i) const { return value_->operator[](i); }

size_t Json::size() const noexcept { return value_->size(); }

Json Json::parse(const string& content, string& errMsg) noexcept {
  try {
    Parser p(content);
    return p.parse();
  } catch (JsonException& e) {
    errMsg = e.what();
    return Json(nullptr);
  }
}

string Json::serialize() const noexcept {
  switch (value_->type()) {
  case JsonType::kNull: return "null";
  case JsonType::kBool: return value_->toBool() ? "true" : "false";
  case JsonType::kNumber:
    char buf[32];
    snprintf(buf, sizeof(buf), "%.17g", value_->toDouble());
    return buf;
  case JsonType::kString: return serializeString();
  case JsonType::kArray: return serializeArray();
  case JsonType::kObject: return serializeObject();
  }
  assert(0);
}

string Json::serializeString() const noexcept {
  string ret = "\"";
  for (auto e : value_->toString()) {
    switch (e) {
    case '\"': ret += "\\\""; break;
    case '\\': ret += "\\\\"; break;
    case '\b': ret += "\\b"; break;
    case '\f': ret += "\\f"; break;
    case '\n': ret += "\\n"; break;
    case '\r': ret += "\\r"; break;
    case '\t': ret += "\\t"; break;
    default:
      if (static_cast<unsigned char>(e) < 0x20) {
        char buf[7];
        sprintf(buf, "\\u%04X", e);
        ret += buf;
      } else
        ret += e;
    }
  }
  return ret + '"';
}

string Json::serializeArray() const noexcept {
  string ret = "[ ";
  for (size_t i = 0; i < value_->size(); ++i) {
    if (i > 0) ret += ", ";
    ret += (*this)[i].serialize();
  }
  return ret + " ]";
}

string Json::serializeObject() const noexcept {
  string ret = "{ ";
  bool first = 1;
  for (const pair<string, Json>& p : value_->toObject()) {
    if (first)
      first = 0;
    else
      ret += ", ";

    ret += "\"" + p.first + "\"";
    ret += ": ";
    ret += p.second.serialize();
  }
  return ret + " }";
}

bool operator==(const Json& lhs, const Json& rhs) noexcept {
  if (lhs.type() != rhs.type()) return false;

  switch (lhs.type()) {
  case JsonType::kNull: return true;
  case JsonType::kBool: return lhs.toBool() == rhs.toBool();
  case JsonType::kNumber: return lhs.toDouble() == rhs.toDouble();
  case JsonType::kString: return lhs.toString() == rhs.toString();
  case JsonType::kArray: return lhs.toArray() == rhs.toArray();
  case JsonType::kObject: return lhs.toObject() == rhs.toObject();
  }
  assert(0);
}

}  // namespace json
