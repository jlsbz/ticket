//
// Created by Arya on 2018/6/6.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_IMPLEMENT_HPP
#define TRAIN_TICKET_BOOKING_SYSTEM_IMPLEMENT_HPP

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include "utility.hpp"
#include "user.hpp"
#include "train.hpp"
#include "ticket.hpp"
#include "BPlusTree.hpp"
#include "tuple.hpp"
#include "constant.h"
#include "vector.hpp"

typedef _string::String<> string;
typedef sjtu::train train;
typedef sjtu::ticket ticket;
typedef sjtu::user user;
typedef sjtu::time Time;
typedef sjtu::Station Station;
typedef sjtu::PriceName PriceName;
typedef sjtu::pair<train, sjtu::vector<int>> trainInfo;
const int CAPACITY = sjtu::TRAIN_CAPACITY;

sjtu::BPlusTree <int, user> idUser("user");
sjtu::BPlusTree <string, train> sale("sale");
sjtu::BPlusTree <string, train> nSale("nSale");
sjtu::BPlusTree <string, string> locTrain("locTrain");
sjtu::BPlusTree <sjtu::tuple3<string, string, char>, string> direct("direct");
sjtu::BPlusTree <sjtu::tuple3<string, string, string>, int> trKindTicket("trKindTicket");
sjtu::BPlusTree <sjtu::pair<string, string>, int> trDate("trDate");
sjtu::BPlusTree <sjtu::tuple3<int, string, char>, int> idTicket("idTicket");
sjtu::BPlusTree <sjtu::tuple6<int, string, string, string, string, string>, int> infoOrderId("infoOrderId");
sjtu::BPlusTree <int, ticket> orderIdTicket("orderIdTicket");
sjtu::BPlusTree <string, int> lastId("lastId");
sjtu::BPlusTree <int, Station> station("station");
sjtu::BPlusTree <string, PriceName> priceName("priceName");

int idCnt, orderCnt, stationCnt;

namespace arya {

    Station getStation(train const &tr, int const &i) {
        return station.find(tr.stationId[i]).second;
    }

    string stationName(train const &tr, int const &i) {
        return getStation(tr, i).getName();
    }

    ticket getTicket(int const &i) {
        return orderIdTicket.find(i).second;
    }

    bool intersect(train const &tr, ticket const &tic, string const &loc1, string const &loc2) {
        int l, r, s, t;
        for (int i = 0; i < tr.stationNum; ++i) {
            auto now = stationName(tr, i);
            if (now == loc1)
                l = i;
            if (now == loc2)
                r = i;
            if (now == tic.loc1)
                s = i;
            if (now == tic.loc2)
                t = i;
        }
        return !(r <= s || l >= t);
    }

    void nextDay(string &date) {
        static int Days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int month = (int)sjtu::stringToDouble(date.get(5, 6));
        int day = (int)sjtu::stringToDouble(date.get(8, 9));
        if(day == Days[month]) {
            month++;
            date[5] = (char)(month / 10 + '0');
            date[6] = (char)(month % 10 + '0');
            date[8] = '0';
            date[9] = '1';
        }
        else {
            day++;
            date[8] = (char)(day / 10 + '0');
            date[9] = (char)(day % 10 + '0');
        }
    }

    string getStartTime(train const &tr, string date, string const &loc) {
        string res(date);
        Station last;
        bool flag = false;
        for(int i = 0; i < tr.stationNum; ++i) {
            Station st = getStation(tr, i);
            if(i && st.getStart() < last.getStart())
                flag = 1;
            last = st;
            if(st.getName() == loc) {
                if(flag)
                    nextDay(res);
                res += ' ' ;
                res += st.getStart().out();
                return res;
            }
        }
        throw sjtu::invalid_operation();
    }

    string getArriveTime(train const &tr, string date, string const &loc) {
        string res(date);
        Station last;
        bool flag = false;
        for(int i = 0; i < tr.stationNum; ++i) {
            Station st = getStation(tr, i);
            if(i && st.getStart() < last.getStart())
                flag = 1;
            last = st;
            if(st.getName() == loc) {
                if(flag)
                    nextDay(res);
                res += ' ' ;
                res += st.getArrive().out();
                return res;
            }
        }
        throw sjtu::invalid_operation();
    }

    double getPrice(train const &tr, string const &loc1, string const &loc2, int const &priceId) {
        int pos1 = 0, pos2 = 0;
        double sum = 0;
        for(int i = 0; i < tr.stationNum; ++i) {
            Station st = getStation(tr,  i);
            if(st.getName() == loc1)
                pos1 = i;
            if(st.getName() == loc2)
                pos2 = i;
        }
        for(int i = pos1 + 1; i <= pos2; i++) {
            Station st = getStation(tr,  i);
            sum += st.getPrice(priceId);
        }
        return sum;
    }

    int Register(user &u) {
        int id = ++idCnt;
        u.id = id;
        if (id == 2018)
            u.priv = 2;
        else
            u.priv = 1;
        if (idUser.insert(id, u))
            return id;
        return -1;
    }

    bool queryProfile(int const &id, user &u) {
        auto res = idUser.find(id);
        if (!res.first)
            return false;
        else {
            u = res.second;
            return true;
        }
    }

    bool login(int const &id, string const &pwd) {
        user u;
        if (!queryProfile(id, u))
            return false;
        if (u.pwd != pwd)
            return false;
        u.loginSta = true;
        idUser.modify(id, u);
        return true;
    }

    bool modifyProfile(int const &id, user &u) {
        auto res = idUser.find(id);
        if (!res.first)
            return false;
        u.id = res.second.id;
        u.priv = res.second.priv;
        idUser.modify(id, u);
        return true;
    }

    void modifyPrivilege(int const &id, user &u, int const &priv) {
        u.priv = priv;
        idUser.modify(id, u);
    }

    bool excludeTicket(train const &tr, string const &loc1, string const &loc2, string const &date,
                       sjtu::vector<int> &ret) {
        int sold = 0;
        ret.clear();
        for(int j = 0; j < tr.priceNum; ++j)
            ret.push_back(0);
        PriceName pn = priceName.find(tr.id).second;
        auto allTicket = trDate.find_multi(sjtu::pair<string, string>(tr.getId(), date));
        for (int i = 0; i < allTicket.size(); ++i) {
            ticket tic = getTicket(allTicket[i]);
            if (tic.getNum() && intersect(tr, tic, loc1, loc2)) {
                for(int j = 0; j < tr.priceNum; ++j)
                    if(tic.getKind() == pn[j]) {
                        ret[j] += tic.getNum();
                        if(ret[j] == CAPACITY)
                            sold++;
                    }
                }
            }
        return sold < tr.priceNum;
    }

    void printQueryTicket(train const &tr, string const &loc1, string const &loc2, string const &date,
                          const sjtu::vector<int> &used) {
        std::cout << tr.id << ' ' << loc1 << ' ' << getStartTime(tr, date, loc1) << ' ' << loc2 << ' '
                  << getArriveTime(tr, date, loc2);
        PriceName pn = priceName.find(tr.id).second;
        for (int i = 0; i < tr.priceNum; ++i) {
            int rem = CAPACITY;
            rem -= used[i];
            if (rem > 0) {
                std::cout << ' ' << pn[i] << ' ' << rem << ' ' << getPrice(tr, loc1, loc2, i);
            }
        }
        std::cout << std::endl;
    }

    bool cmp(trainInfo const &a, trainInfo const &b) {
        return a.first < b.first;
    }

    bool queryTicket(string const &loc1, string const &loc2, string const &date, string const &catalog) {
        int len = catalog.length(), tot = 0;
        trainInfo q[sjtu::MAX_DIRECT];
        for (int i = 0; i < len; ++i) {
            char cat = catalog[i];
            auto allTrain = direct.find_multi(sjtu::tuple3<string, string, char>(loc1, loc2, cat));
            for (int j = 0; j < allTrain.size(); ++j) {
                train tr = sale.find(allTrain[j]).second;
                sjtu::vector<int> ret;
                if (excludeTicket(tr, loc1, loc2, date, ret)) {
                    q[++tot] = trainInfo(tr, ret);
                }
            }
        }
        sort(q + 1, q + 1 + tot, cmp);
        std::cout << tot << std::endl;
        for(int i = 1; i <= tot; ++i) {
            auto tr = q[i].first;
            auto used = q[i].second;
            printQueryTicket(tr, loc1, loc2, date, used);
        }
        return true;
    }

    Time calTime(train const &tr1, train const &tr2, string const &loc0, string const &loc1, string const &loc2) {
        Time t[4];
        for (int i = 0; i < tr1.stationNum; ++i) {
            auto st = getStation(tr1, i);
            if (st.getName() == loc0) {
                t[0] = st.getStart();
            } else if (st.getName() == loc1) {
                t[1] = st.getArrive();
            }
        }
        for (int i = 0; i < tr2.stationNum; ++i) {
            auto st = getStation(tr2, i);
            if (st.getName() == loc1) {
                t[2] = st.getStart();
            } else if (st.getName() == loc2) {
                t[3] = st.getArrive();
            }
        }
        Time res(0, 0), day(24, 0);
        for (int i = 0; i < 3; ++i)
            res += t[i] < t[i + 1] ? t[i + 1] - t[i] : t[i + 1] - t[i] + day;
        return res;
    }

    bool queryTransfer(string const &loc1, string const &loc2, string const &date, string const &catalog) {
        //loc1->loc0->loc2
        //time, loc1, loc0, loc2, tr1, tr2
        typedef sjtu::tuple6<Time, string, string, string, trainInfo, trainInfo> Result;
        Result ans;
        bool flag = false;
        auto trainS = locTrain.find_multi(loc1);
        if (!trainS.size())
            return false;
        for(int it = 0; it < trainS.size(); ++it) {
            auto tr1 = sale.find(trainS[it]).second;
            for(int i = tr1.stationNum - 1; i >= 0; --i) {
                auto loc0 = stationName(tr1, i);
                if (loc0 == loc1)
                    break;
                sjtu::vector<int> ret1, ret2;
                if(excludeTicket(tr1, loc1, loc0, date, ret1)) {
                    for (int pos = catalog.length() - 1; pos >= 0; pos--) {
                        char cat = catalog[pos];
                        auto trainS2 = direct.find_multi(sjtu::tuple3<string, string, char>(loc0, loc2, cat));
                        {
                            for (int j = 0; j < trainS2.size(); ++j) {
                                auto tr2 = sale.find(trainS2[j]).second;
                                if (excludeTicket(tr2, loc0, loc2, date, ret2)) {
                                    auto now = Result(calTime(tr1, tr2, loc1, loc0, loc2), loc1, loc0, loc2,
                                                      trainInfo(tr1, ret1), trainInfo(tr2, ret2));
                                    if (!flag || now.head() < ans.head()) {
                                        flag = true;
                                        ans = now;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!flag)
            return false;
        printQueryTicket(ans.data5.first, ans.data2, ans.data3, date, ans.data5.second);
        printQueryTicket(ans.data6.first, ans.data3, ans.data4, date, ans.data6.second);
        return true;
    }

    void modifyTicketSet(int const &orderId, int const &num) {
        auto tic = getTicket(orderId);
        tic.modifyNum(num);
        orderIdTicket.modify(orderId, tic);
    }

    void insertOrder(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2,
                     string const &date, string const &kind) {
        auto key = sjtu::tuple6<int, string, string, string, string, string>(id, trainId, loc1, loc2, date, kind);
        auto tmp = infoOrderId.find(key);
        auto tr = sale.find(trainId).second;
        char catalog = tr.catalog;
        if (!tmp.first) {
            //create a new order
            int cnt = ++orderCnt;
            ticket newTic(id, cnt, num, trainId, loc1, loc2, date, kind, catalog);
            infoOrderId.insert(key, cnt);
            orderIdTicket.insert(cnt, newTic);

            trKindTicket.insert(sjtu::tuple3<string, string, string>(trainId, date, kind), cnt);
            trDate.insert(sjtu::pair<string, string>(trainId, date), cnt);
            idTicket.insert(sjtu::tuple3<int, string, char>(id, date, catalog), cnt);
        }
        else {
            modifyTicketSet(tmp.second, num);
        }
    }

    bool buyTicket(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2,
                   string const &date, string const &kind) {
        int used = 0;
        auto res = sale.find(trainId);
        if (!res.first)
            return false;
        train tr = res.second;
        auto pn = priceName.find(tr.id).second;
        bool flag = false;
        for(int i = 0; i < tr.priceNum; ++i)
            if(pn[i] == kind) {
                flag = true;
                break;
            }
        if(!flag)
            return false;
        auto tickets = trKindTicket.find_multi(sjtu::tuple3<string, string, string>(trainId, date, kind));
        for(int i = 0; i < tickets.size(); ++i) {
            auto ticket = getTicket(tickets[i]);
            if (intersect(tr, ticket, loc1, loc2)) {
                used += ticket.getNum();
            }
        }
        if (CAPACITY - used >= num) {
            insertOrder(id, num, trainId, loc1, loc2, date, kind);
            return true;
        }
        return false;
    }

    void printTicket(ticket const &tic, train const &tr, const string &date) {
        PriceName pn = priceName.find(tic.trainId).second;
        std::cout << tic.trainId << ' ';
        std::cout << tic.loc1 << ' ' << getStartTime(tr, date, tic.loc1) << ' ';
        std::cout << tic.loc2 << ' ' << getArriveTime(tr, date, tic.loc2);
        for (int j = 0; j < tr.priceNum; ++j) {
            std::cout << ' ' << pn[j] << ' ';
            if (pn[j] == tic.kind)
                std::cout << tic.num << ' ';
            else
                std::cout << "0 ";
            std::cout << getPrice(tr, tic.loc1, tic.loc2, j);
        }
        std::cout << std::endl;
    }

    bool queryOrder(int const &id, string const &date, string const &catalog) {
        user u;
        if (!queryProfile(id, u))
            return false;
        int len = catalog.length(), tot = 0;
        for (int i = 0; i < len; ++i) {
            char cat = catalog[i];
            auto tickets = idTicket.find_multi(sjtu::tuple3<int, string, char>(id, date, cat));
            auto sz = tickets.size();
            for(int j = 0; j < sz; ++j) {
                auto tic = getTicket(tickets[j]);
                if(tic.num == 0)
                    continue;
                tot++;
            }
        }
        std::cout << tot << std::endl;
        for (int i = 0; i < len; ++i) {
            char cat = catalog[i];
            auto tickets = idTicket.find_multi(sjtu::tuple3<int, string, char>(id, date, cat));
            auto sz = tickets.size();
            for(int j = 0; j < sz; ++j) {
                auto tic = getTicket(tickets[j]);
                if(tic.num == 0)
                    continue;
                auto tr = sale.find(tic.trainId).second;
                printTicket(tic, tr, date);

            }
        }
        return true;
    }

    bool refundTicket(int const &id, int const &num, string const &trainId, string const &loc1, string const &loc2,
                      string const &date, string const &kind) {
        auto tmp = infoOrderId.find(
                sjtu::tuple6<int, string, string, string, string, string>(id, trainId, loc1, loc2, date, kind));
        if (!tmp.first)
            return false;
        auto orderId = tmp.second;
        auto tic = getTicket(orderId);
        if (tic.num < num)
            return false;
        modifyTicketSet(orderId, -num);
        return true;
    }

    void printTrain(train const &tr) {
        std::cout << tr << ' ';
        PriceName pn = priceName.find(tr.id).second;
        std::cout << pn << std::endl;

        for(int i = 0; i < tr.stationNum; ++i) {
            Station st = getStation(tr, i);
            std::cout << st << std::endl;
        }
    }

    bool addTrain(string const &id) {
        bool flag = !(sale.find(id).first || nSale.find(id).first);
        train newTr(id);
        std::cin >> newTr;
        PriceName pn(newTr.priceNum);
        std::cin >> pn;

        for(int i = 0; i < newTr.stationNum; ++i) {
            Station newSt(newTr.priceNum);
            std::cin >> newSt;
            if(flag) {
                newTr.stationId[i] = ++stationCnt;
                station.insert(stationCnt, newSt);
            }
        }
        if(!flag)
            return false;
        priceName.insert(id, pn);
        nSale.insert(id, newTr);
        return true;
    }

    bool saleTrain(string const &id) {
        auto tmp0 = nSale.find(id);
        if (!tmp0.first) {
            return false;
        }
        auto tr = tmp0.second;
        nSale.erase(id);
        sale.insert(id, tr);

        char cat = tr.catalog;
        string loc[sjtu::STATION_NUM];
        for (int i = 0; i < tr.stationNum; ++i) {
            loc[i] = stationName(tr, i);
        }
        for (int i = 0; i < tr.stationNum; ++i) {
            for (int j = i + 1; j < tr.stationNum; ++j) {
                auto key = sjtu::tuple3<string, string, char>(loc[i], loc[j], cat);
                direct.insert(key, id);
            }
        }
        for (int i = 0; i < tr.stationNum; ++i) {
            locTrain.insert(loc[i], id);
        }
        return true;
    }

    bool queryTrain(string const &id, train &T) {
        auto tmp = sale.find(id);
        if (tmp.first) {
            T = tmp.second;
            return true;
        }
        return false;
    }

    bool deleteTrain(string const &id) {
        auto tmp = nSale.find(id);
        if (!tmp.first)
            return false;
        train tr = tmp.second;
        for(int i = 0; i < tr.stationNum; ++i)
            station.erase(tr.stationId[i]);
        nSale.erase(id);
        priceName.erase(id);
        return true;
    }

    bool modifyTrain(string const &id) {
        auto tmp = nSale.find(id);
        train newTr(id);
        std::cin >> newTr;
        PriceName pn(newTr.priceNum);
        std::cin >> pn;
        for(int i = 0; i < newTr.stationNum; ++i) {
            Station newSt(newTr.priceNum);
            std::cin >> newSt;
            if(tmp.first) {
                newTr.stationId[i] = ++stationCnt;
                station.insert(stationCnt, newSt);
            }
        }
        if(!tmp.first)
            return false;
        priceName.modify(id, pn);
        nSale.modify(id, newTr);
        train tr = tmp.second;
        for(int i = 0; i < tr.stationNum; ++i)
            station.erase(tr.stationId[i]);
        return true;
    }

}
#endif //TRAIN_TICKET_BOOKING_SYSTEM_IMPLEMENT_HPP
