#ifndef STONKS_BINANCE_BINANCE_TYPES_H_
#define STONKS_BINANCE_BINANCE_TYPES_H_

#include <string>
#include <vector>

#include "binance_enums.h"

namespace stonks::binance {
using Fill = std::string;

struct PlaceOrderResult {
  std::string client_order_id{};
  double cummulative_quote_quantity{};
  double executed_quantity{};
  std::vector<Fill> fills{};
  int64_t order_id{};
  int64_t order_list_id{};
  double original_quantity{};
  double price{};
  Side side{};
  Status status{};
  std::string symbol{};
  TimeInForce time_in_force{};
  int64_t transaction_time{};
  Type type{};

 private:
  friend bool operator==(const PlaceOrderResult &,
                         const PlaceOrderResult &) = default;
};

struct Kline {
  int64_t open_time{};
  double open_price{};
  double high_price{};
  double low_price{};
  double close_price{};
  double volume{};
  int64_t close_time{};
  double quote_asset_volume{};
  int64_t num_trades{};
  double taker_buy_base_asset_volume{};
  double taker_buy_quote_asset_volume{};

 private:
  friend bool operator==(const Kline &, const Kline &) = default;
};
}  // namespace stonks::binance

#endif  // STONKS_BINANCE_BINANCE_TYPES_H_