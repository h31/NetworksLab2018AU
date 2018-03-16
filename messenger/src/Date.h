#ifndef MESSENGER_DATE_H
#define MESSENGER_DATE_H

#include <string>
#include <ctime>

struct Date final {
//    Date(const Date &) = default;
    
//    Date(Date &&) = default;
    
    static Date now();
    
    static Date from_string(const std::string &date_string);
    
    static Date invalid() {
        return Date(0);
    }
    
    std::string to_string() const;
    
    std::string pretty_string() const;
    
    struct tm *get_local_time() const;
    
    int hours() const {
        return get_local_time()->tm_hour;
    }

    int minutes() const {
        return get_local_time()->tm_min;
    }

    int seconds() const {
        return get_local_time()->tm_sec;
    }
    
    friend bool operator<(const Date &lhs, const Date &rhs) {
        return lhs.ltimer < rhs.ltimer;
    }
    
    friend bool operator==(const Date &lhs, const Date &rhs) {
        return lhs.ltimer == rhs.ltimer;
    }
    
private:
    std::time_t ltimer;
    
    explicit Date(const std::time_t &local_timer);
};

#endif //MESSENGER_DATE_H
