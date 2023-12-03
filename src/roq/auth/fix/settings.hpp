/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <fmt/compile.h>
#include <fmt/format.h>

#include "roq/args/parser.hpp"

#include "roq/auth/fix/flags/flags.hpp"

namespace roq {
namespace auth {
namespace fix {

struct Settings final : public flags::Flags {
  explicit Settings(args::Parser const &);
};

}  // namespace fix
}  // namespace auth
}  // namespace roq

template <>
struct fmt::formatter<roq::auth::fix::Settings> {
  template <typename Context>
  constexpr auto parse(Context &context) {
    return std::begin(context);
  }
  template <typename Context>
  auto format(roq::auth::fix::Settings const &value, Context &context) const {
    using namespace fmt::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(flags={})"
        R"(}})"_cf,
        static_cast<roq::auth::fix::flags::Flags const &>(value));
  }
};
