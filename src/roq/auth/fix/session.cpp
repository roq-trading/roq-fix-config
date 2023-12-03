/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include "roq/auth/fix/session.hpp"

#include <nlohmann/json.hpp>

#include <charconv>
#include <chrono>

#include "roq/logging.hpp"

#include "roq/exceptions.hpp"

#include "roq/json/datetime.hpp"
#include "roq/json/number.hpp"
#include "roq/json/string.hpp"

#include "roq/web/rest/server_factory.hpp"

using namespace std::literals;

namespace roq {
namespace auth {
namespace fix {

// === CONSTANTS ===

namespace {
auto const JSONRPC_VERSION = "2.0"sv;

auto const UNKNOWN_METHOD = "UNKNOWN_METHOD"sv;
}  // namespace

// === HELPERS ===

namespace {}  // namespace

// === IMPLEMENTATION ===

Session::Session(
    Handler &handler,
    Settings const &settings,
    Config const &config,
    uint64_t session_id,
    io::net::tcp::Connection::Factory &factory)
    : handler_{handler}, settings_{settings}, config_{config}, session_id_{session_id},
      server_{web::rest::ServerFactory::create(*this, factory)} {
}

bool Session::ready() const {
  return state_ == State::READY;
}

bool Session::zombie() const {
  return state_ == State::ZOMBIE;
}

void Session::close() {
  (*server_).close();
}

// web::rest::Server::Handler

void Session::operator()(web::rest::Server::Disconnected const &) {
  state_ = State::ZOMBIE;
  auto disconnected = Disconnected{
      .session_id = session_id_,
  };
  handler_(disconnected);
}

void Session::operator()(web::rest::Server::Request const &request) {
  log::info("DEBUG request={}"sv, request);
  auto success = false;
  try {
    if (request.headers.connection == web::http::Connection::UPGRADE) {
      log::info("Upgrading session_id={} to websocket..."sv, session_id_);
      (*server_).upgrade(request);
      state_ = State::READY;
      auto upgraded = Upgraded{
          .session_id = session_id_,
      };
      handler_(upgraded);
    } else {
      auto path = request.path;  // note! url path has already been split
      if (!std::empty(path) && !std::empty(settings_.url_prefix) && path[0] == settings_.url_prefix)
        path = path.subspan(1);  // drop prefix
      if (!std::empty(path)) {
        Response response{*server_, request, encode_buffer_};
        route(response, request, path);
      }
    }
    success = true;
  } catch (RuntimeError &e) {
    log::error("Error: {}"sv, e);
  } catch (std::exception &e) {
    log::error("Error: {}"sv, e.what());
  }
  if (!success)
    close();
}

void Session::operator()(web::rest::Server::Text const &text) {
  log::info(R"(message="{})"sv, text.payload);
  auto success = false;
  try {
    process(text.payload);
    success = true;
  } catch (RuntimeError &e) {
    log::error("Error: {}"sv, e);
  } catch (std::exception &e) {
    log::error("Error: {}"sv, e.what());
  }
  if (!success)
    close();
}

void Session::operator()(web::rest::Server::Binary const &) {
}

void Session::route(
    Response &response, web::rest::Server::Request const &request, std::span<std::string_view> const &path) {
  switch (request.method) {
    using enum web::http::Method;
    case GET:
      if (path[0] == "user"sv) {  // note! just for testing, really
        if (std::size(path) == 1)
          get_user(response, request);
      }
      break;
    case HEAD:
      break;
    case POST:
      break;
    case PUT:
      break;
    case DELETE:
      break;
    case CONNECT:
      break;
    case OPTIONS:
      break;
    case TRACE:
      break;
  }
}

// note! checks if query params can be matched
void Session::get_user(Response &response, web::rest::Server::Request const &request) {
  std::string_view component, username, password;
  for (auto &[key, value] : request.query) {
    if (key == "component"sv) {
      component = value;
    } else if (key == "username"sv) {
      username = value;
    } else if (key == "password"sv) {
      password = value;
    } else {
      throw RuntimeError{R"(Unexpected: query key="{}" not supported)"sv, key};
    }
  }
  if (std::empty(component) || std::empty(username) || std::empty(password))
    throw RuntimeError{R"(Unexpected: missing query params)"sv};
  auto result = false;
  // XXX FIXME we need index for component and username
  for (auto &[key, user] : config_.users) {
    if (user.component == component && user.username == username) {
      if (user.password == password) {
        result = true;
      } else {
        log::warn(
            R"(Unexpected: component="{}", username="{}", password="{}"/"{}")"sv,
            component,
            username,
            password,
            user.password);
      }
    }
  }
  response(web::http::Status::OK, web::http::ContentType::APPLICATION_JSON, R"({{"status":{}}})"sv, result);
}

void Session::process(std::string_view const &message) {
  log::info(R"(message="{}")"sv, message);
  assert(!zombie());
  auto success = false;
  try {
    auto json = nlohmann::json::parse(message);  // note! not fast... you should consider some other json parser here
    auto version = json.at("jsonrpc"sv).template get<std::string_view>();
    if (version != JSONRPC_VERSION)
      throw RuntimeError{R"(Invalid JSONRPC version ("{}"))"sv, version};
    auto method = json.at("method"sv).template get<std::string_view>();
    auto id = json.at("id"sv);
    process_jsonrpc(method, id);
    success = true;
  } catch (RuntimeError &e) {
    log::error("Error: {}"sv, e);
  } catch (std::exception &e) {
    log::error("Error: {}"sv, e.what());
  }
  log::debug("success={}"sv, success);
  if (!success)
    close();
}

void Session::process_jsonrpc(std::string_view const &method, auto const &id) {
  if (method == "subscribe"sv) {
    // XXX TODO insert, update, remove
    std::string message;
    fmt::format_to(std::back_inserter(message), "["sv);
    auto first = true;
    for (auto &[key, user] : config_.users) {
      if (first) {
        first = false;
      } else {
        fmt::format_to(std::back_inserter(message), ","sv);
      }
      fmt::format_to(
          std::back_inserter(message),
          R"({{)"
          R"("action":"insert",)"
          R"("component":"{}",)"
          R"("username":"{}",)"
          R"("password":"{}",)"
          R"("accounts":"{}",)"
          R"("strategy_id":{})"
          R"(}})"sv,
          user.component,
          user.username,
          user.password,
          user.accounts,
          user.strategy_id);
    }
    fmt::format_to(std::back_inserter(message), "]"sv);
    send_result(message, id);
  } else {
    send_error(UNKNOWN_METHOD, id);
  }
}

void Session::send_result(std::string_view const &message, auto const &id) {
  send_jsonrpc("result"sv, message, id);
}

void Session::send_error(std::string_view const &message, auto const &id) {
  send_jsonrpc("error"sv, message, id);
}

void Session::send_jsonrpc(std::string_view const &type, std::string_view const &message, auto const &id) {
  assert(!zombie());
  // note!
  //   response must echo the id field from the request (same type)
  auto type_2 = id.type();
  switch (type_2) {
    using enum nlohmann::json::value_t;
    case string:
      send_text(
          R"({{)"
          R"("jsonrpc":"{}",)"
          R"("{}":{},)"
          R"("id":"{}")"
          R"(}})"sv,
          JSONRPC_VERSION,
          type,
          message,
          id.template get<std::string_view>());
      break;
    case number_integer:
    case number_unsigned:
      send_text(
          R"({{)"
          R"("jsonrpc":"{}",)"
          R"("{}":{},)"
          R"("id":{})"
          R"(}})"sv,
          JSONRPC_VERSION,
          type,
          message,
          id.template get<int64_t>());
      break;
    default:
      log::warn("Unexpected: type={}"sv, magic_enum::enum_name(type_2));
  }
}

template <typename... Args>
void Session::send_text(fmt::format_string<Args...> const &fmt, Args &&...args) {
  encode_buffer_.clear();
  fmt::format_to(std::back_inserter(encode_buffer_), fmt, std::forward<Args>(args)...);
  log::debug(R"(message="{}")"sv, encode_buffer_);
  (*server_).send_text(encode_buffer_);
}

}  // namespace fix
}  // namespace auth
}  // namespace roq
