/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/auth/fix/application.hpp"

#include "roq/io/engine/context_factory.hpp"

#include "roq/auth/fix/config.hpp"
#include "roq/auth/fix/settings.hpp"

using namespace std::literals;

namespace roq {
namespace auth {
namespace fix {

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  auto params = args.params();
  if (std::empty(params))
    log::fatal("Unexpected"sv);
  Settings settings{args};
  auto config = Config::parse_file(settings.config_file);
  auto context = io::engine::ContextFactory::create_libevent();
  Controller{settings, config, *context}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace fix
}  // namespace auth
}  // namespace roq
