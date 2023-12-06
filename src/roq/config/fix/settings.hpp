/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <fmt/compile.h>
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
  template <typename Context>
  constexpr auto parse(Context &context) {
    return std::begin(context);
  }
  template <typename Context>
  auto format(roq::config::fix::Settings const &value, Context &context) const {
    using namespace fmt::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(flags={})"
        R"(}})"_cf,
        static_cast<roq::config::fix::flags::Flags const &>(value));
  }
};
