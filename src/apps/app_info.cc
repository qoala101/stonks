#include "info_server.h"
#include "proxy_client_server.h"

auto main(int /*unused*/, const char* /*unused*/[]) -> int {
  auto proxy = stonks::ProxyClient{};
  constexpr auto endpoint = stonks::InfoServer::kEndpoint;
  proxy.RegisterEndpoint(endpoint);
  const auto port = proxy.GetEndpointPort(endpoint);

  const auto app = stonks::InfoServer{
      "http://localhost:" + std::to_string(*port) + endpoint};
  static_cast<void>(getchar());
}