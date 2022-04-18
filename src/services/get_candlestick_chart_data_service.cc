#include "get_candlestick_chart_data_service.h"

#include <cpprest/http_client.h>
#include <spdlog/spdlog.h>

#include "finance_api.h"
#include "google_charts_conversions.h"
#include "utils.h"

namespace stonks {
namespace {
void HandleGetRequest(const web::http::http_request &request) {
  spdlog::info("Got {} request on {}", request.method(),
               request.request_uri().to_string());
  const auto candlesticks = finance::GetCandlesticks(
      finance::Symbol{.base_asset = "ETH", .quote_asset = "USDT"},
      finance::Interval::k1Minute, utils::GetUnixTime() - std::chrono::hours{1},
      utils::GetUnixTime());

  if (!candlesticks.has_value()) {
    request.reply(web::http::status_codes::NotFound);
    spdlog::info("Sent NotFound response");
    return;
  }

  request.reply(web::http::status_codes::OK, ConvertToJson(*candlesticks));
  spdlog::info("Sent OK response");
}
}  // namespace

pplx::task<void> GetCandlestickChartDataService::Start() {
  http_listener_ = web::http::experimental::listener::http_listener{
      "http://localhost:6506/api/candlestick_chart_data"};
  http_listener_.support(web::http::methods::GET, HandleGetRequest);
  return http_listener_.open();
}

pplx::task<void> GetCandlestickChartDataService::Stop() {
  return http_listener_.close();
}
}  // namespace stonks