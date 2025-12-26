#pragma once

#include <string>
#include <tuple>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
class scan_result {
public:
    std::tuple<Ts...> &values() { return res_; }

private:
    std::tuple<Ts...> res_;
};

}  // namespace stdx::details