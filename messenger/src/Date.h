#ifndef MESSENGER_DATE_H
#define MESSENGER_DATE_H

#include <string>
#include <ctime>

struct Date final {
    static Date now();
    
    static Date from_string(const std::string &date_string);
    
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
    
private:
    std::time_t ltimer;
    explicit Date(const std::time_t &local_timer);
};

#endif //MESSENGER_DATE_H
