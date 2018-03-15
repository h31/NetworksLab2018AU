#include <algorithm>
#include <sstream>
#include <cstring>
#include <cassert>
#include "Date.h"

static const int BIT_SHIFT = 7;
static const int BIT_SHIFT_MASK  = (1 << BIT_SHIFT) - 1;

static inline void raise_date_error() {
    throw std::runtime_error("Date error");
}

Date Date::now() {
    time_t timer;
    ::time(&timer);
    auto global_time = gmtime(&timer);
    Date result(*global_time, true);
    return result;
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

static int64_t number_from_string(const std::string &num_string, size_t &offset) {
    auto size = static_cast<size_t>(num_string.at(offset));
    if (offset >= num_string.size()) {
        raise_date_error();
    }
    offset++;
    int64_t result = 0;
    for (size_t i = 0; i < size; ++i, ++offset) {
        result <<= BIT_SHIFT;
        result |= static_cast<int>(num_string.at(offset));
    }
    return result;
}

Date Date::from_string(const std::string &date_string) {
    tm global_time;
    size_t offset = 0;
    auto get_byte = [&]() {
        char result = date_string.at(offset);
        offset++;
        return result;
    };
    
    global_time.tm_sec = get_byte();
    global_time.tm_min = get_byte();
    global_time.tm_hour = get_byte();
    global_time.tm_mday = get_byte();
    global_time.tm_mon = get_byte();
    
    global_time.tm_year = static_cast<int>(number_from_string(date_string, offset) - 1900);
    if (global_time.tm_year < 0) {
        raise_date_error();
    }
    global_time.tm_wday = get_byte();
    global_time.tm_yday = static_cast<int>(number_from_string(date_string, offset));
    global_time.tm_isdst = get_byte();
//    global_time.tm_gmtoff = number_from_string(date_string, offset);
//    size_t tm_zone_size = date_string.size() - offset;
//    char *tm_zone = static_cast<char *>(malloc(tm_zone_size + 1));
//    bzero(tm_zone, tm_zone_size + 1);
//    strncpy(tm_zone, date_string.c_str() + offset, tm_zone_size);
//    global_time.tm_zone = tm_zone;
    return Date(global_time, false);
}

std::string Date::to_string() const {
    std::string result;
    result += static_cast<char>(global_time.tm_sec);
    result += static_cast<char>(global_time.tm_min);
    result += static_cast<char>(global_time.tm_hour);
    result += static_cast<char>(global_time.tm_mday);
    result += static_cast<char>(global_time.tm_mon);
    result += number_to_string(1900 + global_time.tm_year);
    result += static_cast<char>(global_time.tm_wday);
    result += number_to_string(global_time.tm_yday);
    result += static_cast<char>(global_time.tm_isdst);
//    result += number_to_string(global_time.tm_gmtoff);
//    result += std::string(global_time.tm_zone);
    return result;
}

std::string Date::pretty_string() const {
    std::string result = asctime(&global_time);
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }
    return result;
}

Date::~Date() {
//    if (!is_tm_from_now) {
//        free(const_cast<char *>(global_time.tm_zone));
//    }
}

Date::Date(tm global_time, bool is_tm_from_now)
        : global_time(global_time)
        , is_tm_from_now(is_tm_from_now)
{}

