/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include "roq/args/parser.hpp"

#include "roq/config/fix/flags/flags.hpp"

namespace roq {
namespace config {
namespace fix {

struct Settings final : public flags::Flags {
  explicit Settings(args::Parser const &);
};

}  // namespace fix
}  // namespace config
}  // namespace roq

template <>
struct fmt::formatter<roq::config::fix::Settings> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::config::fix::Settings const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(flags={})"
        R"(}})"sv,
        static_cast<roq::config::fix::flags::Flags const &>(value));
  }
};
