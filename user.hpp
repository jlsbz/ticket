//
// Created by Arya on 2018/5/11.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_USER_H
#define TRAIN_TICKET_BOOKING_SYSTEM_USER_H

#include <iostream>
#include "utility.hpp"

namespace sjtu
{

    class user {

    public:
        string name, pwd, email, phone;
        bool loginSta;
        int id, priv;

    public:
        user(): name(), pwd(), email(), phone() {
            id = -1;
            priv = 0;
            loginSta = false;
        }

        int getPri() {
            return priv;
        }

        bool operator< (const user &other) const{
            return id < other.id;
        }

    };

    std::ostream& operator<< (std::ostream &os, const user &obj) {
        os << obj.name << ' ' << obj.email << ' ' << obj.phone << ' ' << obj.priv;
        return os;
    }
    std::istream& operator>> (std::istream &is, user &obj) {
        is >> obj.name >> obj.pwd >> obj.email >> obj.phone;
        return is;
    }
}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_USER_H
