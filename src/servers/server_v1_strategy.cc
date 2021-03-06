#include "server_v1_strategy.h"

#include <gsl/assert>
#include <string_view>
#include <type_traits>
#include <utility>

#include "endpoints_v1_strategy.h"
#include "finance_types.h"
#include "uri.h"

namespace stonks::server {
V1Strategy::V1Strategy(int port, std::shared_ptr<stonks::V1Strategy> entity)
    : server_{network::LocalUri{port, kEndpoint}, {Run()}},
      entity_{std::move(entity)} {
  Expects(entity_ != nullptr);
}

auto V1Strategy::Run() -> network::Endpoint {
  return {endpoints::V1Strategy::Run(),
          network::HasResultTakesParams{[this](network::Params params) {
            return entity_->Run(
                std::move(params).Take<finance::SymbolName>("symbol"));
          }}};
}
}  // namespace stonks::server