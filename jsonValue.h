#pragma once
#include <utility>
#include "json.h"
#include "jsonException.h"

namespace json {

class JsonValue {
public:
  virtual ~JsonValue() = default;

  virtual JsonType type() const = 0;

  virtual bool toBool() const {
    throw JsonException("not a boolean");
  }
  virtual double toDouble() const {
    throw JsonException("not a number");
  }
  virtual const std::string& toString() const {
    throw JsonException("not a string");
  }
  virtual const Json::array_t& toArray() const {
    throw JsonException("not an array");
  }
  virtual const Json::object_t& toObject() const {
    throw JsonException("not an object");
  }

  virtual Json& operator[](size_t) {
    throw JsonException("not an array");
  }
  virtual const Json& operator[](size_t) const {
    throw JsonException("not an array");
  }

  virtual Json& operator[](const std::string&) {
    throw JsonException("not an object");
  }
  virtual const Json& operator[](const std::string&) const {
    throw JsonException("not an object");
  }

  virtual size_t size() const noexcept {
    return 0;
  }
};

template <typename T, JsonType U>
class Value : public JsonValue {
public:
  Value(const T& val) : val_(val) {}
  Value(T&& val) : val_(val) {}

  JsonType type() const final {
    return U;
  }

protected:
  T val_;
};

class JsonNull final : public Value<std::nullptr_t, JsonType::kNull> {
public:
  explicit JsonNull(std::nullptr_t) : Value(nullptr) {}
};

class JsonBool final : public Value<bool, JsonType::kBool> {
public:
  explicit JsonBool(bool val) : Value(val) {}
  bool toBool() const override {
    return val_;
  }
};

class JsonDouble final : public Value<double, JsonType::kNumber> {
public:
  explicit JsonDouble(double val) : Value(val) {}
  double toDouble() const override {
    return val_;
  }
};

class JsonString final : public Value<std::string, JsonType::kString> {
public:
  explicit JsonString(const std::string& val) : Value(val) {}
  explicit JsonString(std::string&& val) : Value(std::move(val)) {}
  const std::string& toString() const override {
    return val_;
  }
};

class JsonArray final : public Value<Json::array_t, JsonType::kArray> {
public:
  explicit JsonArray(const Json::array_t& val) : Value(val) {}
  explicit JsonArray(Json::array_t&& val) : Value(std::move(val)) {}

  const Json::array_t& toArray() const override {
    return val_;
  }

  const Json& operator[](size_t i) const override {
    return val_[i];
  }
  Json& operator[](size_t i) override {
    return val_[i];
  }

  size_t size() const noexcept override {
    return val_.size();
  }
};

class JsonObject final : public Value<Json::object_t, JsonType::kObject> {
public:
  explicit JsonObject(const Json::object_t& val) : Value(val) {}
  explicit JsonObject(Json::object_t&& val) : Value(std::move(val)) {}

  const Json::object_t& toObject() const override {
    return val_;
  }

  const Json& operator[](const std::string& i) const override {
    return val_.at(i);
  }
  Json& operator[](const std::string& i) override {
    return val_.at(i);
  }

  size_t size() const noexcept override {
    return val_.size();
  }
};
}  // namespace json
