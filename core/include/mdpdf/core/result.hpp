#pragma once

#include <string>
#include <utility>
#include <variant>

namespace mdpdf::core {

struct Error {
  std::string message;
};

template <typename T>
class Result {
public:
  static Result ok(T value) { return Result{std::move(value)}; }
  static Result fail(std::string message) {
    return Result{Error{std::move(message)}};
  }

  bool is_ok() const { return std::holds_alternative<T>(data_); }
  const T& value() const { return std::get<T>(data_); }
  T& value() { return std::get<T>(data_); }
  const Error& error() const { return std::get<Error>(data_); }

private:
  explicit Result(T v) : data_(std::move(v)) {}
  explicit Result(Error e) : data_(std::move(e)) {}
  std::variant<T, Error> data_;
};

}  // namespace mdpdf::core
