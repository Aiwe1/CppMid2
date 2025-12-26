#pragma once

#include <cstdlib>
#include <expected>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "types.hpp"

namespace stdx::details {

template <typename T>
std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt) {
    if constexpr (std::is_integral_v<T>) {
        if (!fmt.empty() && (fmt != "%d") && (fmt != "%u")) {
            return std::unexpected(scan_error("Incorrect format(need int)"));
        }
        T res = 0;
        res = std::atoi(input.data());
        if ((res == 0) && (input.size() == 1 && input.data()[0] == '0')) {
            return res;
        }

        if constexpr (std::is_unsigned_v<T>) {
            if (!fmt.empty() && fmt != "%u") {
                return std::unexpected(scan_error("Incorrect format(need uint)"));
            }
            if (input[0] == '-')
                return std::unexpected(scan_error("Incorret value UINT"));
        }

        return res;
    } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
        if (!fmt.empty() && fmt != "%f") {
            return std::unexpected(scan_error("Incorrect format(need double or float)"));
        }
        double res = 0.0;
        res = std::atof(input.data());

        if constexpr (std::is_same_v<T, float>) {
            float res_f = std::atof(input.data());
            if (static_cast<double>(res_f) < res - 0.00005 || static_cast<double>(res_f) > res + 0.00005) {
                return std::unexpected(scan_error("Incorrect format(float)"));
            }
        }

        if (res != 0.0 || input == "0.0")
            return res;

        return std::unexpected(scan_error("Incorret value DOUBLE"));
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (!fmt.empty() && fmt != "%s") {
            return std::unexpected(scan_error("Incorret value STRING"));
        }
        std::string res(input);
        if (res.empty())
            return std::unexpected(scan_error("Incorret value STR"));
        return res;
    } else if constexpr (std::is_same_v<T, std::string_view>) {
        if (!fmt.empty() && fmt != "%s") {
            return std::unexpected(scan_error("Incorret value STRING_VIEW"));
        }
        if (input.empty())
            return std::unexpected(scan_error("Incorret value STR_VIEW"));
        return input;
    }
    return std::unexpected(scan_error("Incorret value"));
}

template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    return parse_value<T>(input, fmt);
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {

    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }

    return std::pair{format_parts, input_parts};
}

}  // namespace stdx::details