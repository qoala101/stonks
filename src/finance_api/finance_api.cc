#include "finance_api.h"

#include <spdlog/spdlog.h>

#include <compare>
#include <cstdint>
#include <gsl/assert>
#include <limits>
#include <memory>
#include <range/v3/iterator/basic_iterator.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/view.hpp>
#include <ratio>
#include <string>
#include <string_view>
#include <thread>
#include <variant>

#include "binance_api.h"
#include "binance_enums.h"
#include "binance_types.h"
#include "finance_conversions.h"
#include "finance_enum_conversions.h"
#include "finance_types.h"
#include "utils.h"

namespace stonks::finance {
std::optional<std::vector<Candle>> GetCandles(
    const Symbol &symbol, Interval interval,
    std::chrono::milliseconds start_time, std::chrono::milliseconds end_time) {
  Expects(interval != Interval::kInvalid);
  Expects(start_time < end_time);

  const auto first_candle_open_time =
      CeilStartTimeToInterval(start_time, interval);
  const auto last_candle_close_time =
      FloorEndTimeToInterval(end_time, interval);

  if (first_candle_open_time > last_candle_close_time) {
    return std::vector<Candle>{};
  }

  const auto num_candles = CalculateNumIntervalsInPeriod(
      first_candle_open_time, last_candle_close_time, interval);

  std::this_thread::sleep_until(
      std::chrono::system_clock::time_point{last_candle_close_time});

  const auto interval_time = ConvertIntervalToMilliseconds(interval);
  start_time = first_candle_open_time;
  end_time = last_candle_close_time;
  auto candles = std::vector<Candle>{};

  while (candles.size() < num_candles) {
    if (!candles.empty()) {
      start_time = candles.back().close_time + std::chrono::milliseconds{1};
    }

    const auto klines = binance::GetKlines(
        symbol.base_asset + symbol.quote_asset, interval, start_time,
        last_candle_close_time, std::numeric_limits<int>::max());

    if (!klines.has_value()) {
      spdlog::error("Cannot get candles");
      return std::nullopt;
    }

    const auto last_kline_close_time =
        klines->empty() ? last_candle_close_time : klines->back().close_time;
    auto kline = klines->begin();

    for (auto kline_open_time = start_time;
         kline_open_time < last_kline_close_time;
         kline_open_time += interval_time) {
      if (const auto kline_is_valid = (kline != klines->end()) &&
                                      (kline->open_time == kline_open_time)) {
        candles.emplace_back(ParseCandleFromBinanceKline(*kline));
        kline++;
        continue;
      }

      auto &null_kline = candles.emplace_back();
      null_kline.open_time = kline_open_time;
      null_kline.close_time =
          kline_open_time + interval_time - std::chrono::milliseconds{1};
    }
  }

  return candles;
}

std::chrono::milliseconds CeilStartTimeToInterval(
    std::chrono::milliseconds time, Interval interval) {
  Expects(interval != Interval::kInvalid);

  const auto interval_ms =
      stonks::finance::ConvertIntervalToMilliseconds(interval).count();
  return std::chrono::milliseconds{
      ((time.count() + interval_ms - 1) / interval_ms) * interval_ms};
}

std::chrono::milliseconds FloorEndTimeToInterval(std::chrono::milliseconds time,
                                                 Interval interval) {
  Expects(interval != Interval::kInvalid);

  const auto interval_ms =
      stonks::finance::ConvertIntervalToMilliseconds(interval).count();
  return std::chrono::milliseconds{
      (((time.count() + 1) / interval_ms) * interval_ms) - 1};
}

int CalculateNumIntervalsInPeriod(std::chrono::milliseconds start_time,
                                  std::chrono::milliseconds end_time,
                                  Interval interval) {
  Expects(interval != Interval::kInvalid);
  Expects(start_time < end_time);

  return (FloorEndTimeToInterval(end_time, interval).count() + 1 -
          CeilStartTimeToInterval(start_time, interval).count()) /
         stonks::finance::ConvertIntervalToMilliseconds(interval).count();
}

std::optional<OrderError> PlaceOrder(
    const OrderProxyOrderRequest &order_request) {
  const auto result = binance::PlaceOrder(
      order_request.symbol.base_asset + order_request.symbol.quote_asset,
      order_request.buy_or_sell,
      ConvertOrderTypeToBinanceOrderType(order_request.order_type),
      binance::OrderTimeInForce::kGoodTillCanceled,
      order_request.amount.GetBaseAmount(),
      order_request.amount.GetQuoteAmount(),
      order_request.order_type.GetPrice(),
      utils::ConvertUuidToString(order_request.order_uuid), std::nullopt,
      std::nullopt, std::nullopt, std::nullopt, 10000);

  if (std::holds_alternative<binance::PlaceOrderAcknowledgement>(result)) {
    return std::nullopt;
  }

  if (std::holds_alternative<binance::ApiError>(result)) {
    return ParseOrderErrorFromBinanceApiError(
        std::get<binance::ApiError>(result));
  }

  return OrderError{.message = "stonks: Binance API failure."};
}

std::optional<OrderInfo> GetOrderInfo(const Symbol &symbol,
                                      boost::uuids::uuid uuid) {
  const auto order =
      binance::QueryOrder(symbol.base_asset + symbol.quote_asset, std::nullopt,
                          utils::ConvertUuidToString(uuid));

  if (!order.has_value()) {
    spdlog::error("Cannot query order");
    return std::nullopt;
  }

  return ParseOrderInfoFromBinanceOrderInfo(*order);
}

bool IsOrderStatusFinal(OrderStatus order_status) {
  switch (order_status) {
    case OrderStatus::kInvalid:
    case OrderStatus::kFilled:
    case OrderStatus::kCancelled:
    case OrderStatus::kRejected:
    case OrderStatus::kExpired:
      return true;
      break;
    default:
      break;
  }

  return false;
}

std::optional<double> GetAverageSymbolPrice(const Symbol &symbol) {
  const auto result =
      binance::GetAverageSymbolPrice(symbol.base_asset + symbol.quote_asset);

  if (!result.has_value()) {
    spdlog::error("Cannot get average symbol price");
    return std::nullopt;
  }

  return result->price;
}

std::optional<double> GetSymbolPrice(const Symbol &symbol) {
  const auto result =
      binance::GetSymbolPrice(symbol.base_asset + symbol.quote_asset);

  if (!result.has_value()) {
    spdlog::error("Cannot get symbol price");
    return std::nullopt;
  }

  return result->price;
}

std::optional<std::vector<SymbolPrice>> GetAllSymbolsPrices() {
  const auto result = binance::GetAllSymbolsPrices();

  if (!result.has_value()) {
    spdlog::error("Cannot get all symbols prices");
    return std::nullopt;
  }

  const auto binance_symbol_price_to_symbol_price =
      [](const binance::SymbolPrice &symbol_price) {
        return SymbolPrice{
            .symbol = ParseSymbolFromBinanceSymbol(symbol_price.symbol),
            .price = TimeDouble{.time = utils::GetUnixTime(),
                                .value = symbol_price.price}};
      };
  return *result |
         ranges::views::transform(binance_symbol_price_to_symbol_price) |
         ranges::to_vector;
}

std::optional<std::vector<Symbol>> GetAllSymbols() {
  const auto result = binance::GetExchangeInfo();

  if (!result.has_value()) {
    spdlog::error("Cannot get all symbols");
    return std::nullopt;
  }

  const auto symbol_from_symbol_info =
      [](const binance::SymbolExchangeInfo &symbol_info) {
        return Symbol{.base_asset = symbol_info.base_asset,
                      .quote_asset = symbol_info.quote_asset};
      };
  return result->symbols | ranges::views::transform(symbol_from_symbol_info) |
         ranges::to_vector;
}

auto GetSymbolsInfo() -> std::optional<std::vector<SymbolInfo>> {
  const auto result = binance::GetExchangeInfo();

  if (!result.has_value()) {
    spdlog::error("Cannot get all symbols");
    return std::nullopt;
  }

  const auto symbol_info_from_binance_symbol_info =
      [](const binance::SymbolExchangeInfo &symbol_info) {
        return SymbolInfo{.base_asset = symbol_info.base_asset,
                          .quote_asset = symbol_info.quote_asset};
      };
  return result->symbols |
         ranges::views::transform(symbol_info_from_binance_symbol_info) |
         ranges::to_vector;
}
}  // namespace stonks::finance