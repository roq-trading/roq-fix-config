/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/service.hpp"

#include "roq/auth/fix/controller.hpp"

namespace roq {
namespace auth {
namespace fix {

struct Application final : public Service {
  using Service::Service;  // inherit constructors

 protected:
  int main(args::Parser const &) override;

 private:
  using value_type = Controller;  // note!
};

}  // namespace fix
}  // namespace auth
}  // namespace roq
