#ifndef STONKS_STRATEGIES_BREAKOUT_STRATEGY_H_
#define STONKS_STRATEGIES_BREAKOUT_STRATEGY_H_

#include <optional>
#include <vector>

#include "finance_types.h"

namespace stonks::finance {
class BreakoutStrategy {
 public:
  std::optional<StrategyOrderRequest> ProcessNewCandles(
      const std::vector<Candle> &candles);

 private:
  std::optional<Candle> prev_candle_{};
};
}  // namespace stonks::finance

#endif  // STONKS_STRATEGIES_BREAKOUT_STRATEGY_H_
