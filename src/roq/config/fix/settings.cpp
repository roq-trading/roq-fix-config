/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include "roq/config/fix/settings.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace config {
namespace fix {

Settings::Settings(args::Parser const &) : flags::Flags{flags::Flags::create()} {
  log::info("settings={}"sv, *this);
}

}  // namespace fix
}  // namespace config
}  // namespace roq
