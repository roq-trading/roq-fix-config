/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/auth/fix/settings.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace auth {
namespace fix {

Settings::Settings(args::Parser const &) : flags::Flags{flags::Flags::create()} {
  log::debug("settings={}"sv, *this);
}

}  // namespace fix
}  // namespace auth
}  // namespace roq
