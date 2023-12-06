/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include "roq/config/fix/application.hpp"

#include "roq/io/engine/context_factory.hpp"

#include "roq/config/fix/config.hpp"
#include "roq/config/fix/settings.hpp"

using namespace std::literals;

namespace roq {
namespace config {
namespace fix {

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  auto params = args.params();
  if (!std::empty(params))
    log::fatal("Unexpected"sv);
  Settings settings{args};
  auto config = Config::parse_file(settings.config_file);
  auto context = io::engine::ContextFactory::create_libevent();
  Controller{settings, config, *context}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace fix
}  // namespace config
}  // namespace roq
