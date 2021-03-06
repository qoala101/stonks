#include <stdio.h>

#include <memory>
#include <optional>

#include "client_proxy.h"
#include "server_info.h"
#include "stonks_info.h"

auto main(int /*unused*/, const char* /*unused*/[]) -> int {
  auto proxy = stonks::client::Proxy{};
  constexpr auto endpoint = stonks::server::Info::kEndpoint;
  proxy.RegisterEndpoint(endpoint);
  const auto port = proxy.GetEndpointPort(endpoint);

  const auto app =
      stonks::server::Info{*port, std::make_shared<stonks::Info>()};
  static_cast<void>(getchar());
}