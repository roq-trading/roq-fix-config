/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/service.hpp"

#include "roq/config/fix/controller.hpp"

namespace roq {
namespace config {
namespace fix {

struct Application final : public Service {
  using Service::Service;  // inherit constructors

 protected:
  int main(args::Parser const &) override;

 private:
  using value_type = Controller;  // note!
};

}  // namespace fix
}  // namespace config
}  // namespace roq
