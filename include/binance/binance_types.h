#ifndef STONKS_BINANCE_BINANCE_TYPES_H_
#define STONKS_BINANCE_BINANCE_TYPES_H_

#include <chrono>
#include <string>
#include <variant>
#include <vector>

#include "binance_enums.h"

namespace stonks::binance {
using Fill = std::string;

struct PlaceOrderAcknowledgement {
  std::string symbol{};
  int64_t order_id{};
  int64_t order_list_id{};
  std::string client_order_id{};
  std::chrono::milliseconds transaction_time{};

 private:
  friend bool operator==(const PlaceOrderAcknowledgement &,
                         const PlaceOrderAcknowledgement &) = default;
};

struct OrderInfo {
  std::string symbol{};
  int64_t order_id{};
  int64_t order_list_id{};
  std::string client_order_id{};
  double price{};
  double original_quantity{};
  double executed_quantity{};
  double cummulative_quote_quantity{};
  OrderStatus status{};
  OrderTimeInForce time_in_force{};
  OrderType type{};
  OrderSide side{};
  double stop_price{};
  double iceberg_quantity{};
  std::chrono::milliseconds time{};
  std::chrono::milliseconds update_time{};
  bool is_working{};
  double original_quote_order_quantity{};

 private:
  friend bool operator==(const OrderInfo &, const OrderInfo &) = default;
};

struct Kline {
  std::chrono::milliseconds open_time{};
  double open_price{};
  double high_price{};
  double low_price{};
  double close_price{};
  double volume{};
  std::chrono::milliseconds close_time{};
  double quote_asset_volume{};
  int64_t num_trades{};
  double taker_buy_base_asset_volume{};
  double taker_buy_quote_asset_volume{};

 private:
  friend bool operator==(const Kline &, const Kline &) = default;
};

struct ApiError {
  int64_t code{};
  std::string message{};
};

struct AverageSymbolPrice {
  int64_t mins{};
  double price{};
};

struct SymbolPrice {
  std::string symbol{};
  double price{};
};

struct SymbolExchangeInfo {
  std::string symbol{};
  //  std::string status{};
  std::string base_asset{};
  // int base_asset_precision{};
  std::string quote_asset{};
  // int quote_precision{};
  //  std::string order_types{};
  // bool iceberg_allowed{};
  // bool oco_allowed{};
  // bool quote_order_quantity_market_allowed{};
  // bool allow_trailing_stop{};
  // bool is_spot_trading_allowed{};
  // bool is_margin_trading_allowed{};
  //  std::string filters{};
  double min_quantity{};
  double step_size{};
  double min_notional{};
  double tick_size{};
  //  std::string permissions{};
};

struct ExchangeInfo {
  // std::string timezone{};
  // std::chrono::milliseconds server_time{};
  // std::string rate_limits{};
  // std::string exchange_filters{};
  std::vector<SymbolExchangeInfo> symbols{};
};

struct SymbolBookTicker {
  int64_t order_book_update_id{};
  std::string symbol{};
  double best_bid_price{};
  double best_bid_quantity{};
  double best_ask_price{};
  double best_ask_quantity{};
};
}  // namespace stonks::binance

#endif  // STONKS_BINANCE_BINANCE_TYPES_H_
