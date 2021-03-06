#ifndef STONKS_SERVICES_BREAKOUT_STRATEGY_SERVICE_H_
#define STONKS_SERVICES_BREAKOUT_STRATEGY_SERVICE_H_

#include <pplx/pplxtasks.h>

#include <atomic>
#include <thread>

#include "finance_types.h"

namespace stonks {
class BreakoutStrategyService {
 public:
  explicit BreakoutStrategyService(finance::Symbol symbol);

  pplx::task<void> Start();
  pplx::task<void> Stop();

 private:
  const finance::Symbol symbol_{};
  std::thread thread_{};
  std::atomic_bool service_state_{};
};
}  // namespace stonks

#endif  // STONKS_SERVICES_BREAKOUT_STRATEGY_SERVICE_H_
