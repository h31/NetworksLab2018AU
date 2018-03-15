#include <algorithm>
#include <stdexcept>
//#include <sstream>
#include <cstring>
#include <ctime>
#include <cctype>
#include <cassert>
#include "Date.h"

// Locks not needed (Are not needed as client is single-threaded):
// TODO locks on concurrent (from different clients) calls to gmtime, localtime
// TODO locks on concurrent (from different clients) calls to ctime, asctime.

static const int BIT_SHIFT = 7;
static const int BIT_SHIFT_MASK  = (1 << BIT_SHIFT) - 1;

Date Date::now() {
    auto timer = time(nullptr);
    return Date(timer);
}

static std::string number_to_string(int64_t num) {
    std::string num_string;
    while (num > 0) {
        num_string += static_cast<char>(num & BIT_SHIFT_MASK);
        num >>= BIT_SHIFT;
    }
    std::reverse(num_string.begin(), num_string.end());
    num_string = std::string(1, static_cast<char>(num_string.size())) + num_string;
    return num_string;
};

static int64_t number_from_string(const std::string &num_string) {
//    if (offset >= num_string.size()) {
//        raise_date_error();
//    }
    size_t offset = 0;
    auto size = static_cast<size_t>(num_string.at(offset));
    
    offset++;
    int64_t result = 0;
    for (size_t i = 0; i < size; ++i, ++offset) {
        result <<= BIT_SHIFT;
        result |= static_cast<int>(num_string.at(offset));
    }
    return result;
}

static inline int64_t local_time_offset() {
    static int64_t result = -1;
    if (result == -1) {
        std::time_t timer;
        time(&timer);
        auto local_time = localtime(&timer);
        result = static_cast<int64_t>(local_time->tm_gmtoff);
    }
    return result;
}

Date Date::from_string(const std::string &date_string) {
    std::time_t global_timer = number_from_string(date_string);
    std::time_t local_timer = global_timer + local_time_offset();
    return Date{local_timer};
}

std::string Date::to_string() const {
    std::string result;
    result += number_to_string(static_cast<int64_t>(ltimer - local_time_offset()));
    return result;
}

std::string Date::pretty_string() const {
    std::string result = ctime(&ltimer);
    while (!result.empty() && isspace(result.back())) {
        result.pop_back();
    }
    return result;
}

Date::Date(const std::time_t &local_timer)
    : ltimer(local_timer)
{}


struct tm *Date::get_local_time() const {
    return ::localtime(&ltimer);
}
