#ifndef MESSENGER_DATE_H
#define MESSENGER_DATE_H

#include <string>
#include <ctime>

struct Date final {
    static Date now();
    
    static Date from_string(const std::string &date_string);
    
    std::string pretty_string() const;
    
    std::string to_string() const;
    
    ~Date();

private:
    tm global_time;
    bool is_tm_from_now = false;
    Date(tm global_time, bool is_tm_from_now);
};

#endif //MESSENGER_DATE_H
