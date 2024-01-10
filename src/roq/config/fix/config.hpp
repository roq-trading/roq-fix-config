/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <fmt/format.h>

#include <ranges>

#include <string>
#include <string_view>

namespace roq {
namespace config {
namespace fix {

struct User final {
  std::string component;
  std::string username;
  std::string password;
  std::string accounts;  // XXX TODO
  uint32_t strategy_id = {};
};

struct Config final {
  static Config parse_file(std::string_view const &);
  static Config parse_text(std::string_view const &);

  absl::flat_hash_map<std::string, User> const users;

 protected:
  explicit Config(auto &node);
};

}  // namespace fix
}  // namespace config
}  // namespace roq

template <>
struct fmt::formatter<roq::config::fix::User> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::config::fix::User const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(component="{}", )"
        R"(username="{}", )"
        R"(password="{}", )"
        R"(accounts="{}", )"
        R"(strategy_id={})"
        R"(}})"sv,
        value.component,
        value.username,
        value.password,
        value.accounts,
        value.strategy_id);
  }
};

template <>
struct fmt::formatter<roq::config::fix::Config> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::config::fix::Config const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(users=[{}])"
        R"(}})"sv,
        fmt::join(std::ranges::views::transform(value.users, [](auto &item) { return item.second; }), ","sv));
  }
};
