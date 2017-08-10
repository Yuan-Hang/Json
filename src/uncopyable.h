#pragma once

class Uncopyable {
public:
  Uncopyable() = default;
  Uncopyable(const Uncopyable&) = delete;
  ~Uncopyable() = default;
  Uncopyable& operator=(const Uncopyable&) = default;
};
