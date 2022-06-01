#ifndef STONKS_CLIENTS_CLIENT_PROXY_H_
#define STONKS_CLIENTS_CLIENT_PROXY_H_

#include "client.h"
#include "proxy.h"

namespace stonks {
class ProxyClient : public network::ProxyInterface {
 public:
  explicit ProxyClient();

  [[nodiscard]] auto GetEndpointPort(std::string_view endpoint) const
      -> std::optional<int> override;
  void RegisterEndpoint(std::string_view endpoint) override;

 private:
  network::Client client_;
};
}  // namespace stonks

#endif  // STONKS_CLIENTS_CLIENT_PROXY_H_
