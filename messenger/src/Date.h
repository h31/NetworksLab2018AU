#ifndef MESSENGER_DATE_H
#define MESSENGER_DATE_H

#include <string>
#include <ctime>

struct Date final {
    Date(const Date &rhs);
    
    static Date now();
    
    static Date from_string(const std::string &date_string);
    
    std::string pretty_string() const;
    
    std::string to_string() const;
    
    ~Date();

private:
    tm global_time;
    explicit Date(tm new_global_time);
};

#endif //MESSENGER_DATE_H
