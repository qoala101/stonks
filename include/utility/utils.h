#ifndef STONKS_UTILITY_UTILS_H_
#define STONKS_UTILITY_UTILS_H_

#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <string>
#include <string_view>

namespace stonks::utils {
/**
 * @return Unix GMT milliseconds.
 */
std::chrono::milliseconds GetUnixTime();

template <typename T>
constexpr std::chrono::milliseconds FloorTo(std::chrono::milliseconds time) {
  constexpr auto coef = T::period::num * 1000;
  return std::chrono::milliseconds{(time.count() / coef) * coef};
}

/**
 * @return Unix GMT milliseconds or nullopt if cannot parse.
 */
std::chrono::milliseconds ParseUnixTimeFromString(
    std::string_view time, std::string_view format = "%d %b %Y %H:%M:%S");

/**
 * @remark %OS produces whole number of seconds whereas %S would produce seconds
 * with decimals.
 * @return Unix GMT milliseconds or nullopt if cannot parse.
 */
std::string ConvertUnixTimeToString(
    std::chrono::milliseconds time,
    std::string_view format = "%d %b %Y %H:%M:%OS");

std::string SignUsingHmacSha256(std::string_view data, std::string_view key);

boost::uuids::uuid ParseUuidFromString(std::string_view uuid);

std::string ConvertUuidToString(boost::uuids::uuid uuid);

boost::uuids::uuid GenerateUuid();
}  // namespace stonks::utils

#endif  // STONKS_UTILITY_UTILS_H_
