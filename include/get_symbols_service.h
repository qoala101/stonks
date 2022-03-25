#ifndef STONKS_GET_SYMBOLS_SERVICE_H_
#define STONKS_GET_SYMBOLS_SERVICE_H_

#include <cpprest/http_listener.h>
#include <pplx/pplxtasks.h>

namespace stonks {
class GetSymbolsService {
 public:
  pplx::task<void> Start();
  pplx::task<void> Stop();

 private:
  web::http::experimental::listener::http_listener http_listener_{};
};
}  // namespace stonks

#endif  // STONKS_GET_SYMBOLS_SERVICE_H_
