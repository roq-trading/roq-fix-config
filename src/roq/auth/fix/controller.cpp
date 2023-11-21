/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/auth/fix/controller.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace auth {
namespace fix {

// === IMPLEMENTATION ===

Controller::Controller(Settings const &settings, Config const &config, io::Context &context) : context_{context} {
}

void Controller::dispatch() {
}

}  // namespace fix
}  // namespace auth
}  // namespace roq
