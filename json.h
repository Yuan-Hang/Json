#pragma once
#include <cstddef>
#include <memory>         //shared_ptr
#include <ostream>        //ostream
#include <string>         //JsonString
#include <unordered_map>  //JsonObject
#include <vector>         //JsonArray

namespace details {
class JsonValue;
}

namespace json {
class Json final {
 public:
  using array_t = std::vector<Json>;
  using object_t = std::unordered_map<std::string, Json>;

  /// represents the various kinds of JSON values
  enum class JsonType {
    /// Null value
    kNull = 1,
    /// Boolean value
    kBool = 2,
    /// Number value
    kNumber = 4,
    /// String value
    kString = 8,
    /// Array value
    kArray = 16,
    /// Object value
    kObject = 32
  };

  /// Constructor creating a JSON null value
  explicit Json(std::nullptr_t);
  /// Constructor creating a JSON boolean value
  explicit Json(bool);
  /// Constructor creating a JSON number value
  explicit Json(int);
  explicit Json(double);
  /// Constructor creating a JSON string value
  explicit Json(const char*);
  explicit Json(const std::string&);
  explicit Json(std::string&&);
  /// Constructor creating a JSON array value
  explicit Json(const array_t&);
  explicit Json(array_t&&);
  /// Constructor creating a JSON object value
  explicit Json(const object_t&);
  explicit Json(object_t&&);
  /// Copy Constructor
  Json(const Json&);
  /// Move Constructor
  Json(Json&&) noexcept;
  /// Destructor, need this empty dtor, implement where JsonValue is complete
  ~Json();
  /// Copy/Move assignment operator
  Json& operator=(Json);
  /// Swap two JSON values
  void swap(Json&) noexcept;

  /// Compare two JSON values for equality
  bool operator==(const Json&) const noexcept;
  /// Compare two JSON values for equality
  bool operator!=(const Json&) const noexcept;

  /// Accesses the type of JSON value the current value instance is
  JsonType type() const noexcept;
  /// Is the current value a null value?
  bool isNull() const noexcept;
  /// Is the current value a boolean value?
  bool isBool() const noexcept;
  /// Is the current value a number value?
  bool isNumber() const noexcept;
  /// Is the current value a string value?
  bool isString() const noexcept;
  /// Is the current value a array value?
  bool isArray() const noexcept;
  /// Is the current value a object value?
  bool isObject() const noexcept;

  /// Converts the JSON value to a C++ boolean, if and only if it is a boolean
  bool asBool() const;
  /// Converts the JSON value to a C++ double, if and only if it is a double
  double asDouble() const;
  /// Converts the JSON value to a C++ string, if and only if it is a string
  const std::string& asString() const;
  /// Converts the JSON value to a json array, if and only if it is an array
  array_t& asArray();
  /// Converts the JSON value to a json array, if and only if it is an array
  const array_t& asArray() const;
  /// Converts the JSON value to a json object, if and only if it is an object
  object_t& asObect();
  /// Converts the JSON value to a json object, if and only if it is an object
  const object_t& asObect() const;
  /// Accesses a field of a JSON array
  Json& operator[](std::size_t);
  /// Accesses a field of a JSON array
  const Json& operator[](std::size_t) const;
  /// Accesses a field of a JSON object
  Json& operator[](const std::string&);
  /// Accesses a field of a JSON object
  const Json& operator[](const std::string&) const;

  /// Get the number of children of the value, 0 for all non-composites
  std::size_t size() const noexcept;

  /// Parse the C++ string to a JSON value
  /// if error occurs, errMsg storage the error message
  static Json parse(const std::string& content, std::string& errMsg) noexcept;
  /// Serializes the Json value to C++ string
  std::string serialize() const noexcept;

 private:
  /// Only used internally by serialize()
  std::string serializeString() const noexcept;
  std::string serializeArray() const noexcept;
  std::string serializeObject() const noexcept;

  std::unique_ptr<details::JsonValue> value_;
};

inline std::ostream& operator<<(std::ostream& os, const Json& json) {
  return os << json.serialize();
}
}  // namespace json
