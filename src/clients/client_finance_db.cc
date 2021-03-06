#include "client_finance_db.h"

#include <string_view>
#include <utility>

#include "endpoint.h"
#include "endpoints_finance_db.h"
#include "server_finance_db.h"
#include "typed_any.h"
#include "uri.h"

namespace stonks::client {
FinanceDb::FinanceDb(int port)
    : client_{network::LocalUri{port, server::StonksDb::kEndpoint}} {}

auto FinanceDb::SelectAssets() const -> std::vector<std::string> {
  return client_.Execute(endpoints::FinanceDb::SelectAssets())
      .Take<std::vector<std::string>>();
}

void FinanceDb::UpdateAssets(std::vector<std::string> assets) {
  client_.Execute(endpoints::FinanceDb::UpdateAssets(), std::move(assets));
}

auto FinanceDb::SelectSymbols() const -> std::vector<finance::SymbolName> {
  return client_.Execute(endpoints::FinanceDb::SelectSymbols())
      .Take<std::vector<finance::SymbolName>>();
}

auto FinanceDb::SelectSymbolsInfo() const -> std::vector<finance::SymbolInfo> {
  return client_.Execute(endpoints::FinanceDb::SelectSymbolsInfo())
      .Take<std::vector<finance::SymbolInfo>>();
}

void FinanceDb::UpdateSymbolsInfo(
    std::vector<finance::SymbolInfo> symbols_info) {
  client_.Execute(endpoints::FinanceDb::UpdateSymbolsInfo(),
                  std::move(symbols_info));
}

auto FinanceDb::SelectSymbolPriceTicks(
    std::optional<int> limit, const std::optional<finance::Period> &period,
    const std::optional<std::vector<finance::SymbolName>> &symbols) const
    -> std::vector<finance::SymbolPriceTick> {
  return client_
      .Execute(endpoints::FinanceDb::SelectSymbolPriceTicks(),
               {{"limit", limit}, {"period", period}, {"symbols", symbols}})
      .Take<std::vector<finance::SymbolPriceTick>>();
}

void FinanceDb::InsertSymbolPriceTick(
    const finance::SymbolPriceTick &symbol_price_tick) {
  client_.Execute(endpoints::FinanceDb::InsertSymbolPriceTick(),
                  symbol_price_tick);
}
}  // namespace stonks::client