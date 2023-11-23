/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <memory>

#include "roq/io/net/tcp/connection.hpp"

#include "roq/web/rest/server.hpp"

#include "roq/auth/fix/config.hpp"
#include "roq/auth/fix/response.hpp"
#include "roq/auth/fix/settings.hpp"

namespace roq {
namespace auth {
namespace fix {

struct Session final : public web::rest::Server::Handler {
  struct Disconnected final {
    uint64_t session_id = {};
  };

  struct Upgraded final {
    uint64_t session_id = {};
  };

  struct Handler {
    virtual void operator()(Disconnected const &) = 0;
    virtual void operator()(Upgraded const &) = 0;
  };

  Session(Handler &, Settings const &, Config const &, uint64_t session_id, io::net::tcp::Connection::Factory &);

 protected:
  bool ready() const;
  bool zombie() const;

  void close();

  // web::rest::Server::Handler
  void operator()(web::rest::Server::Disconnected const &) override;
  void operator()(web::rest::Server::Request const &) override;
  void operator()(web::rest::Server::Text const &) override;
  void operator()(web::rest::Server::Binary const &) override;

  // rest

  void route(Response &, web::rest::Server::Request const &, std::span<std::string_view> const &path);

  void get_user(Response &, web::rest::Server::Request const &);

  // ws

  void process(std::string_view const &message);

  void process_jsonrpc(std::string_view const &method, auto const &id);

  // helpers

  void send_result(std::string_view const &message, auto const &id);
  void send_error(std::string_view const &message, auto const &id);

  void send_jsonrpc(std::string_view const &type, std::string_view const &message, auto const &id);

  template <typename... Args>
  void send_text(fmt::format_string<Args...> const &, Args &&...);

 private:
  Handler &handler_;
  Settings const &settings_;
  Config const &config_;
  uint64_t const session_id_;
  std::unique_ptr<web::rest::Server> server_;
  enum class State { WAITING, READY, ZOMBIE } state_ = {};
  std::string encode_buffer_;  // XXX TODO make shared
};

}  // namespace fix
}  // namespace auth
}  // namespace roq
