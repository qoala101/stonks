#ifndef STONKS_NETWORK_NETWORK_JSON_H_
#define STONKS_NETWORK_NETWORK_JSON_H_

#include <cstdint>
#include <exception>
#include <memory>

#include "not_null.hpp"

namespace stonks::network {
/**
 * @brief JSON object.
 */
// NOLINTNEXTLINE(*-special-member-functions)
class IJson {
 public:
  /**
   * @brief Private implementation to be provided by the client.
   * @remark Can be used by other client entities to access the
   * implementation details.
   */
  class Impl;

  /**
   * @brief Creates a copy with the same data.
   */
  [[nodiscard]] virtual auto Clone() const
      -> cpp::not_null<std::unique_ptr<IJson>> = 0;

  virtual ~IJson() = default;

  [[nodiscard]] virtual auto GetChild(std::string_view key) const
      -> cpp::not_null<std::unique_ptr<IJson>> = 0;
      
  virtual void SetChild(std::string_view key, const IJson &child) = 0;

  /**
   * @brief Gives implementation details.
   */
  [[nodiscard]] virtual auto GetImpl() const -> const Impl & = 0;
};

template <typename T>
auto ParseFromJson(const IJson &json) -> T;

template <>
auto ParseFromJson(const stonks::network::IJson &json) -> int;

template <>
auto ParseFromJson(const stonks::network::IJson &json) -> int64_t;

template <>
auto ParseFromJson(const stonks::network::IJson &json) -> double;

template <>
auto ParseFromJson(const stonks::network::IJson &json) -> std::string;

template <typename T>
auto ConvertToJson(const T &value) -> cpp::not_null<std::unique_ptr<IJson>>;

template <>
auto ConvertToJson(const int &value)
    -> cpp::not_null<std::unique_ptr<stonks::network::IJson>>;

template <>
auto ConvertToJson(const int64_t &value)
    -> cpp::not_null<std::unique_ptr<stonks::network::IJson>>;

template <>
auto ConvertToJson(const double &value)
    -> cpp::not_null<std::unique_ptr<stonks::network::IJson>>;

template <>
auto ConvertToJson(const std::string &value)
    -> cpp::not_null<std::unique_ptr<stonks::network::IJson>>;
}  // namespace stonks::network

#endif  // STONKS_NETWORK_NETWORK_JSON_H_
