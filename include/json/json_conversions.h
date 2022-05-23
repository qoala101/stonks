#ifndef STONKS_JSON_JSON_CONVERSIONS_H_
#define STONKS_JSON_JSON_CONVERSIONS_H_

#include <cpprest/json.h>

#include <any>
#include <chrono>
#include <magic_enum.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/transform.hpp>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "binance_types.h"
#include "concepts.h"
#include "finance_types.h"

namespace stonks::json {
template <NotVector T>
[[nodiscard]] auto ParseFromJson(const web::json::value &json) -> T;

template <>
auto ParseFromJson(const web::json::value &json) -> bool;
auto ConvertToJson(const bool &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> int;
template <>
auto ParseFromJson(const web::json::value &json) -> int64_t;
template <>
auto ParseFromJson(const web::json::value &json) -> double;
template <Number T>
auto ConvertToJson(T data) -> web::json::value {
  return web::json::value::number(data);
}

template <Enumeration T>
auto ConvertToJson(T data) -> web::json::value {
  return web::json::value::string(std::string{magic_enum::enum_name(data)});
}

template <>
auto ParseFromJson(const web::json::value &json) -> std::string;
auto ConvertToJson(const std::string &data) -> web::json::value;
auto ConvertToJson(std::string_view data) -> web::json::value;
auto ConvertToJson(const char *data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> std::chrono::milliseconds;
auto ConvertToJson(std::chrono::milliseconds data) -> web::json::value;

auto ConvertToJson(boost::uuids::uuid data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json)
    -> binance::PlaceOrderAcknowledgement;

template <>
auto ParseFromJson(const web::json::value &json) -> binance::ApiError;

template <>
auto ParseFromJson(const web::json::value &json) -> binance::OrderInfo;

template <>
auto ParseFromJson(const web::json::value &json) -> binance::Kline;

template <>
auto ParseFromJson(const web::json::value &json) -> binance::AverageSymbolPrice;

template <>
auto ParseFromJson(const web::json::value &json) -> binance::SymbolPrice;

template <>
auto ParseFromJson(const web::json::value &json) -> binance::SymbolExchangeInfo;

template <>
auto ParseFromJson(const web::json::value &json) -> binance::ExchangeInfo;

template <>
auto ParseFromJson(const web::json::value &json) -> binance::SymbolBookTicker;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::Symbol;
auto ConvertToJson(const finance::Symbol &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::TimeDouble;
auto ConvertToJson(const finance::TimeDouble &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::SymbolPrices;
auto ConvertToJson(const finance::SymbolPrices &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::SymbolPriceTick;
auto ConvertToJson(const finance::SymbolPriceTick &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::StrategyData;
auto ConvertToJson(const finance::StrategyData &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::OrderType;
auto ConvertToJson(const finance::OrderType &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::OrderUpdate;
auto ConvertToJson(const finance::OrderUpdate &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json)
    -> finance::OrderProxyOrderUpdate;
auto ConvertToJson(const finance::OrderProxyOrderUpdate &data)
    -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::Amount;
auto ConvertToJson(const finance::Amount &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json)
    -> finance::StrategyOrderRequest;
auto ConvertToJson(const finance::StrategyOrderRequest &data)
    -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json)
    -> finance::OrderProxyOrderRequest;
auto ConvertToJson(const finance::OrderProxyOrderRequest &data)
    -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json)
    -> finance::StrategySubscribeToOrderUpdatesRequest;
auto ConvertToJson(const finance::StrategySubscribeToOrderUpdatesRequest &data)
    -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json)
    -> finance::PriceTicksServiceSubscribeRequest;
auto ConvertToJson(const finance::PriceTicksServiceSubscribeRequest &data)
    -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json)
    -> finance::OrderProxyMonitorRequest;
auto ConvertToJson(const finance::OrderProxyMonitorRequest &data)
    -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json)
    -> finance::OrderMonitorOrderUpdate;
auto ConvertToJson(const finance::OrderMonitorOrderUpdate &data)
    -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> std::runtime_error;
auto ConvertToJson(const std::runtime_error &data) -> web::json::value;

template <>
auto ParseFromJson(const web::json::value &json) -> finance::Period;
auto ConvertToJson(const finance::Period &data) -> web::json::value;

template <Vector T>
[[nodiscard]] auto ParseFromJson(const web::json::value &json) -> T {
  if (!json.is_array()) {
    throw std::runtime_error{"Cannot parse array"};
  }

  const auto parse_item = [](const web::json::value &value) {
    return ParseFromJson<typename T::value_type>(value);
  };

  return json.as_array() | ranges::views::transform(parse_item) |
         ranges::to_vector;
}

template <typename T>
auto ConvertToJson(const std::vector<T> &data) -> web::json::value {
  const auto convert_item = [](const T &item) { return ConvertToJson(item); };
  auto json_items =
      data | ranges::views::transform(convert_item) | ranges::to_vector;
  return web::json::value::array(std::move(json_items));
}

template <typename T>
auto ConvertToJson(const std::optional<T> &data) -> web::json::value {
  if (!data.has_value()) {
    return web::json::value::null();
  }

  return ConvertToJson(*data);
}

template <typename T>
[[nodiscard]] auto ParseFromJsonNoThrow(const web::json::value &json)
    -> std::optional<T> {
  try {
    return ParseFromJson<T>(json);
  } catch (const std::exception &e) {
  }

  return std::nullopt;
}

template <typename T>
[[nodiscard]] auto ConvertToJsonNoThrow(const T &data) -> web::json::value {
  try {
    return ConvertToJson(data);
  } catch (const std::exception &e) {
  }

  return web::json::value::null();
}
}  // namespace stonks::json

#endif  // STONKS_JSON_JSON_CONVERSIONS_H_
