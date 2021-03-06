#ifndef STONKS_NETWORK_SERVER_H_
#define STONKS_NETWORK_SERVER_H_

#include <memory>
#include <vector>

#include "endpoint.h"
#include "uri.h"

namespace stonks::network {
/**
 * @brief HTTP server wrapper which handles type safety of params, requests, and
 * responses.
 */
class Server {
 public:
  /**
   * @brief Creates and starts HTTP server on the specified URI which would
   * expose the endpoints and redirect the requests to them.
   */
  explicit Server(const LocalUri &uri, std::vector<Endpoint> endpoints);

  Server(const Server &) = delete;
  Server(Server &&) noexcept;

  auto operator=(const Server &) -> Server & = delete;
  auto operator=(Server &&) noexcept -> Server &;

  /**
   * @brief Stops the HTTP server.
   */
  ~Server();

 private:
  class Impl;

  std::unique_ptr<Impl> impl_{};
};
}  // namespace stonks::network

#endif  // STONKS_NETWORK_SERVER_H_
