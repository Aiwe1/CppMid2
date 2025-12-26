#pragma once

#include <expected>
#include <string_view>
#include <type_traits>
#include <utility>

#include "parse.hpp"
#include "types.hpp"

namespace stdx {
template <typename... Ts, std::size_t... Is>
void for_each_index_sequence(std::expected<details::scan_result<Ts...>, details::scan_error> &res,
                             std::index_sequence<Is...>,
                             const std::pair<std::vector<std::string_view>, std::vector<std::string_view>> &parsed) {

    res = res.and_then([&](auto &t) -> std::expected<details::scan_result<Ts...>, details::scan_error> {
        details::scan_error error_occured;

        auto perform_parse = [&](auto &e, size_t I) {
            auto parse_res = details::parse_value_with_format<std::remove_reference_t<decltype(e)>>(parsed.second[I],
                                                                                                    parsed.first[I]);

            if (!parse_res) {
                error_occured = parse_res.error();
                return false;
            }
            e = std::move(*parse_res);
            return true;
        };

        if ((perform_parse(std::get<Is>(t.values()), Is) && ...)) {
            return t;
        } else {
            return std::unexpected(error_occured);
        }
    });
}

template <typename... Ts>
void addAll(std::expected<details::scan_result<Ts...>, details::scan_error> &res,
            const std::pair<std::vector<std::string_view>, std::vector<std::string_view>> &parsed) {
    for_each_index_sequence(res, std::make_index_sequence<sizeof...(Ts)>{}, parsed);
}

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {

    auto parsed = details::parse_sources(input, format);
    if (!parsed) {
        return std::unexpected(details::scan_error{"Нет значений"});
    }

    std::expected<details::scan_result<Ts...>, details::scan_error> result;
    addAll<Ts...>(result, parsed.value());

    return result;
}

}  // namespace stdx