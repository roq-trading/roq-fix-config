/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include "roq/utils/container.hpp"

#include "roq/io/context.hpp"

#include "roq/io/sys/signal.hpp"
#include "roq/io/sys/timer.hpp"

#include "roq/io/net/tcp/listener.hpp"

#include "roq/config/fix/config.hpp"
#include "roq/config/fix/session.hpp"
#include "roq/config/fix/settings.hpp"

namespace roq {
namespace config {
namespace fix {

struct Controller final : public io::sys::Signal::Handler,
                          public io::sys::Timer::Handler,
                          public io::net::tcp::Listener::Handler,
                          public Session::Handler {
  Controller(Settings const &, Config const &, io::Context &);

  Controller(Controller &&) = default;
  Controller(Controller const &) = delete;

  void dispatch();

 protected:
  // io::sys::Signal::Handler
  void operator()(io::sys::Signal::Event const &) override;

  // io::sys::Timer::Handler
  void operator()(io::sys::Timer::Event const &) override;

  // io::net::tcp::Listener::Handler
  void operator()(io::net::tcp::Connection::Factory &) override;
  void operator()(io::net::tcp::Connection::Factory &, io::NetworkAddress const &) override;

  // Session::Handler
  void operator()(Session::Disconnected const &) override;
  void operator()(Session::Upgraded const &) override;

  void remove_zombies();

 private:
  Settings const &settings_;
  Config const &config_;
  // io
  io::Context &context_;
  std::unique_ptr<io::sys::Signal> terminate_;
  std::unique_ptr<io::sys::Signal> interrupt_;
  std::unique_ptr<io::sys::Timer> timer_;
  std::unique_ptr<io::net::tcp::Listener> listener_;
  // sessions
  uint64_t next_session_id_ = {};
  utils::unordered_map<uint64_t, std::unique_ptr<Session>> sessions_;
  utils::unordered_set<uint64_t> zombies_;
  std::chrono::nanoseconds next_cleanup_ = {};
};

}  // namespace fix
}  // namespace config
}  // namespace roq
