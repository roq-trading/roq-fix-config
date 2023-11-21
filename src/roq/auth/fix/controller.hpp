/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include "roq/io/context.hpp"

#include "roq/auth/fix/config.hpp"
#include "roq/auth/fix/settings.hpp"

namespace roq {
namespace auth {
namespace fix {

struct Controller final {
  Controller(Settings const &, Config const &, io::Context &);

  Controller(Controller &&) = default;
  Controller(Controller const &) = delete;

  void dispatch();

 private:
  io::Context &context_;
};

}  // namespace fix
}  // namespace auth
}  // namespace roq
