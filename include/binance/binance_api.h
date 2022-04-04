#ifndef STONKS_BINANCE_BINANCE_API_H_
#define STONKS_BINANCE_BINANCE_API_H_

#include <optional>
#include <string>
#include <vector>

#include "binance_types.h"

namespace stonks::binance {
using Symbol = std::string;

std::optional<std::vector<Symbol>> GetSymbols();

using Balances = std::string;

std::optional<Balances> GetBalances();

std::optional<PlaceOrderResult> PlaceOrder(
    std::string_view symbol, Side side, Type type,
    std::optional<double> quantity = std::nullopt,
    std::optional<double> price = std::nullopt,
    TimeInForce time_in_force = TimeInForce::kGoodTillCanceled);

std::optional<std::vector<Kline>> GetKlines(
    std::string_view symbol, CandlestickInterval interval,
    std::optional<int64_t> start_time = std::nullopt,
    std::optional<int64_t> end_time = std::nullopt,
    std::optional<int64_t> limit = std::nullopt);
}  // namespace stonks::binance

#endif  // STONKS_BINANCE_BINANCE_API_H_