#include "endpoints_proxy.h"

namespace stonks {
auto endpoints::Proxy::GetEndpointPort() -> network::EndpointDesc {
  return {.method = web::http::methods::GET,
          .relative_uri = "/GetEndpointPort",
          .params = {{"endpoint", json::Type<std::string>{}}},
          .response_body = json::Type<std::optional<int>>{}};
}

auto endpoints::Proxy::RegisterEndpoint() -> network::EndpointDesc {
  return {.method = web::http::methods::POST,
          .relative_uri = "/RegisterEndpoint",
          .params = {{"endpoint", json::Type<std::string>{}}}};
}
}  // namespace stonks