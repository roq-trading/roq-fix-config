/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/auth/fix/controller.hpp"

#include <charconv>
#include <filesystem>

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace auth {
namespace fix {

// === CONSTANTS ===

namespace {
auto const TIMER_FREQUENCY = 100ms;
auto const CLEANUP_FREQUENCY = 1s;
}  // namespace

// === HELPERS ===

namespace {
auto create_network_address(auto &settings) {
  auto address = settings.client_listen_address;
  uint16_t port = {};
  auto [_, ec] = std::from_chars(std::begin(address), std::end(address), port);
  if (ec == std::errc{}) {
    log::info(R"(The service will be started on port={})"sv, port);
    return io::NetworkAddress{port};
  }
  log::info(R"(The service will be started on path="{}")"sv, address);
  auto const directory = std::filesystem::path{address}.parent_path();
  if (!std::empty(directory) && std::filesystem::create_directory(directory))
    log::info(R"(Created path="{}")"sv, directory.c_str());
  return io::NetworkAddress{address};
}
}  // namespace

// === IMPLEMENTATION ===

Controller::Controller(Settings const &settings, Config const &config, io::Context &context)
    : settings_{settings}, config_{config}, context_{context},
      terminate_{context.create_signal(*this, io::sys::Signal::Type::TERMINATE)},
      interrupt_{context.create_signal(*this, io::sys::Signal::Type::INTERRUPT)},
      timer_{context.create_timer(*this, TIMER_FREQUENCY)},
      listener_{context.create_tcp_listener(*this, create_network_address(settings))} {
}

void Controller::dispatch() {
  (*timer_).resume();
  context_.dispatch();
}

// io::sys::Signal::Handler

void Controller::operator()(io::sys::Signal::Event const &event) {
  log::warn("*** SIGNAL: {} ***"sv, magic_enum::enum_name(event.type));
  context_.stop();
}

// io::sys::Timer::Handler

void Controller::operator()(io::sys::Timer::Event const &event) {
  if (next_cleanup_ < event.now) {
    next_cleanup_ = event.now + CLEANUP_FREQUENCY;
    remove_zombies();
  }
}

// io::net::tcp::Listener::Handler

void Controller::operator()(io::net::tcp::Connection::Factory &factory) {
  auto session_id = ++next_session_id_;
  auto session = std::make_unique<Session>(*this, settings_, config_, session_id, factory);
  sessions_.emplace(session_id, std::move(session));
}

void Controller::operator()(io::net::tcp::Connection::Factory &factory, io::NetworkAddress const &) {
  (*this)(factory);
}

// Session::Handler

void Controller::operator()(Session::Disconnected const &disconnected) {
  log::info("Detected zombie session"sv);
  zombies_.emplace(disconnected.session_id);
}

void Controller::operator()(Session::Upgraded const &) {
}

void Controller::remove_zombies() {
  auto count = std::size(zombies_);
  if (count == 0)
    return;
  for (auto iter : zombies_)
    sessions_.erase(iter);
  zombies_.clear();
  log::info("Removed {} zombied session(s) (remaining: {})"sv, count, std::size(sessions_));
}

}  // namespace fix
}  // namespace auth
}  // namespace roq
