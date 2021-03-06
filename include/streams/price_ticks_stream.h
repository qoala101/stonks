#ifndef STONKS_STREAMS_PRICE_TICKS_STREAM_H_
#define STONKS_STREAMS_PRICE_TICKS_STREAM_H_

#include <cpprest/http_listener.h>

#include <condition_variable>
#include <mutex>
#include <vector>

#include "finance_types.h"

namespace stonks::finance {
class PriceTicksStreamRealTime {
 public:
  explicit PriceTicksStreamRealTime();

  ~PriceTicksStreamRealTime();

  auto GetNextPriceTicks() -> std::vector<SymbolPriceTick>;

 private:
  web::http::experimental::listener::http_listener http_listener_{};

  std::vector<SymbolPriceTick> price_ticks_{};
  std::mutex price_ticks_mutex_{};
  std::condition_variable price_ticks_cond_var_{};
};
}  // namespace stonks::finance

#endif  // STONKS_STREAMS_PRICE_TICKS_STREAM_H_
