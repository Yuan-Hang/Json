#pragma once
#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace json {

class JsonValue;

// represents the various kinds of JSON values
enum class JsonType {
  kNull,    // null
  kBool,    // true/false
  kNumber,  // number
  kString,  // string
  kArray,   // array
  kObject   // object
};

class Json final {
public:
  using array_t = std::vector<Json>;
  using object_t = std::unordered_map<std::string, Json>;

  // Parse the C++ string to a JSON value
  // if error occurs, errMsg storage the error message
  static Json parse(const std::string& content, std::string& errMsg) noexcept;
  // Serializes the Json value to C++ string
  std::string serialize() const noexcept;

  explicit Json(std::nullptr_t);               // null
  explicit Json(bool);                         // true/false
  explicit Json(int val) : Json(1.0 * val) {}  // number
  explicit Json(double);                       // number

  // string (without this ctor, Json("xx") will call Json(bool)
  explicit Json(const char* cstr) : Json(std::string(cstr)) {}
  explicit Json(const std::string&);  // string
  explicit Json(const array_t&);      // array
  explicit Json(const object_t&);     // object

  Json(const Json&);
  Json(Json&&) noexcept;
  ~Json();
  Json& operator=(Json);

  // Accesses the type of JSON value the current value instance is
  JsonType type() const noexcept;
  // Is the current value a null value?
  bool isNull() const noexcept;
  // Is the current value a boolean value?
  bool isBool() const noexcept;
  // Is the current value a number value?
  bool isNumber() const noexcept;
  // Is the current value a string value?
  bool isString() const noexcept;
  // Is the current value a array value?
  bool isArray() const noexcept;
  // Is the current value a object value?
  bool isObject() const noexcept;

  // Converts the JSON value to a C++ boolean, if and only if it is a boolean
  bool toBool() const;
  // Converts the JSON value to a C++ double, if and only if it is a double
  double toDouble() const;
  // Converts the JSON value to a C++ string, if and only if it is a string
  const std::string& toString() const;
  // Converts the JSON value to a json array, if and only if it is an array
  const array_t& toArray() const;
  // Converts the JSON value to a json object, if and only if it is an object
  const object_t& toObject() const;

  // Accesses a field of a JSON array
  Json& operator[](std::size_t);
  // Accesses a field of a JSON array
  const Json& operator[](std::size_t) const;
  // Accesses a field of a JSON object
  Json& operator[](const std::string&);
  // Accesses a field of a JSON object
  const Json& operator[](const std::string&) const;

  // Get the number of children of the value, 0 for all non-composites
  std::size_t size() const noexcept;

private:
  void swap(Json&) noexcept;
  // Only used internally by serialize()
  std::string serializeString() const noexcept;
  std::string serializeArray() const noexcept;
  std::string serializeObject() const noexcept;

  std::unique_ptr<JsonValue> value_;
};

inline std::ostream& operator<<(std::ostream& os, const Json& json) {
  return os << json.serialize();
}

// Compare two JSON values for equality
bool operator==(const Json&, const Json&) noexcept;
inline bool operator!=(const Json& lhs, const Json& rhs) noexcept {
  return !(lhs == rhs);
}
}  // namespace json
