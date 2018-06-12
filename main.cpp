//
// Created by Arya on 2018/5/11.
//

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include "utility.hpp"
#include "user.hpp"
#include "train.hpp"
#include "ticket.hpp"
#include "BPlusTree.hpp"
#include "tuple.hpp"
#include "constant.h"
#include "implement.hpp"
using namespace arya;

void Register()
{
    user u;
    std::cin >> u;
    std::cout << Register(u) << std::endl;
}

void login()
{
    int id;
    string pwd;
    std::cin >> id >> pwd;
    std::cout << login(id, pwd) << std::endl;
}

void queryProfile()
{
    int id;
    user u;
    std::cin >> id;
    if(!queryProfile(id, u))
        std::cout << "0" << std::endl;
    else
        std::cout << u << std::endl;
}

void modifyProfile()
{
    int id;
    user u;
    std::cin >> id >> u;
    std::cout << modifyProfile(id, u) << std::endl;
}

void modifyPrivilege()
{
    int id1, id2, priv;
    user u1, u2;
    std::cin >> id1 >> id2 >> priv;
    if(!queryProfile(id1, u1) || !queryProfile(id2, u2) || u1.getPri() !=2 || u2.getPri() > priv)
        std::cout << "0" << std::endl;
    else
    {
        modifyPrivilege(id2, u2, priv);
        std::cout << "1" << std::endl;
    }
}

void queryTicket()
{
    string loc1, loc2, date, catalog;
    std::cin >> loc1 >> loc2 >> date >> catalog;
    if(!queryTicket(loc1, loc2, date, catalog))
        std::cout << -1 << std::endl;
}

///query tickets without date
void queryAllTickets()
{
    string loc1, loc2, catalog;
    std::cin >> loc1 >> loc2 >> catalog;
    if(!queryAllTickets(loc1, loc2, catalog))
        std::cout << -1 << std::endl;
}

void queryTransfer()
{
    string loc1, loc2, date, catalog;
    std::cin >> loc1 >> loc2 >> date >> catalog;
    if(!queryTransfer(loc1, loc2, date, catalog))
        std::cout << -1 << std::endl;
}

void buyTicket()
{
    string trainId, loc1, loc2, date, kind;
    int id, num;
    std::cin >> id >> num >> trainId >> loc1 >> loc2 >> date >> kind;
    std::cout << buyTicket(id, num, trainId, loc1, loc2, date, kind) << std::endl;
}

void queryOrder()
{
    //query_order *id* *date* *catalog*
    int id;
    string date, catalog;
    std::cin >> id >> date >> catalog;
    if(!queryOrder(id, date, catalog))
        std::cout << "-1" << std::endl;
}

void refundTicket()
{
    string trainId, loc1, loc2, date, kind;
    int id, num;
    std::cin >> id >> num >> trainId >> loc1 >> loc2 >> date >> kind;
    std::cout << refundTicket(id, num, trainId, loc1, loc2, date, kind) << std::endl;
}

void addTrain()
{
    string id;
    std::cin >> id;
    std::cout << addTrain(id) << std::endl;
}

void saleTrain()
{
    string id;
    std::cin >> id;
    std::cout << saleTrain(id) << std::endl;
}

void queryTrain()
{
    string id;
    train T;
    std::cin >> id;
    if(!queryTrain(id, T))
    {
        std::cout << "0" << std::endl;
        return;
    }
    printTrain(T);
}

void deleteTrain()
{
    string id;
    std::cin >> id;
    std::cout << deleteTrain(id) << std::endl;
}

void modifyTrain()
{
    string id;
    std::cin >> id;
    std::cout << modifyTrain(id) << std::endl;
}

void modifyValue(int &val, const string &key, const int &st)
{
    val = st;
    lastId.modify(key, val);
}

void clean()
{
    idUser.clean();
    sale.clean();
    nSale.clean();
    locTrain.clean();
    direct.clean();
    trKindTicket.clean();
    trDate.clean();
    idTicket.clean();
    infoOrderId.clean();
    orderIdTicket.clean();
    station.clean();
    priceName.clean();

    modifyValue(idCnt, "id", 2017);
    modifyValue(orderCnt, "order", 0);
    modifyValue(stationCnt, "station", 0);

    std::cout << 1 << std::endl;
}

void setValue(int &val, const string &key, const int &st)
{
    auto id = lastId.find(key);
    if(id.first)
        val = id.second;
    else {
        val = st;
        lastId.insert(key, val);
    }
}

void init()
{
    idUser.open_file();
    sale.open_file();
    nSale.open_file();
    locTrain.open_file();
    direct.open_file();
    trKindTicket.open_file();
    trDate.open_file();
    idTicket.open_file();
    infoOrderId.open_file();
    orderIdTicket.open_file();
    lastId.open_file();
    station.open_file();
    priceName.open_file();

    setValue(idCnt, "id", 2017);
    setValue(orderCnt, "order", 0);
    setValue(stationCnt, "station", 0);
}

void close()
{
    lastId.modify("id", idCnt);
    lastId.modify("order", orderCnt);
    lastId.modify("station", stationCnt);

    idUser.close_file();
    sale.close_file();
    nSale.close_file();
    locTrain.close_file();
    direct.close_file();
    trKindTicket.close_file();
    trDate.close_file();
    idTicket.close_file();
    infoOrderId.close_file();
    orderIdTicket.close_file();
    lastId.close_file();
    station.close_file();
    priceName.close_file();
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
//    freopen("-2.in", "r", stdin);
//    freopen("test.out", "w", stdout);
    const int funcNum = 17;
    string comm;
    string opt[funcNum];
    opt[0] = string("register");
    opt[1] = string("query_profile");
    opt[2] = string("modify_profile");
    opt[3] = string("modify_privilege");
    opt[4] = string("query_ticket");
    opt[5] = string("query_transfer");
    opt[6] = string("buy_ticket");
    opt[7] = string("query_order");
    opt[8] = string("refund_ticket");
    opt[9] = string("add_train");
    opt[10] = string("sale_train");
    opt[11] = string("query_train");
    opt[12] = string("delete_train");
    opt[13] = string("modify_train");
    opt[14] = string("clean");
    opt[15] = string("login");
    opt[16] = string("exit");
    void (*func[funcNum])() = {Register, queryProfile, modifyProfile, modifyPrivilege, queryTicket, queryTransfer, buyTicket,
                          queryOrder, refundTicket, addTrain, saleTrain, queryTrain, deleteTrain, modifyTrain, clean, login};
    init();
    while(true)
    {
        bool flag = false;
        std::cin >> comm;
        if(comm == opt[16]) {
            std::cout << "BYE" << std::endl;
            break;
        }
        for(int i = 0; i < funcNum - 1; i++)
            if(comm == opt[i]) {
                func[i]();
                flag = true;
                break;
            }
        if(!flag) {
            std::cout << "WRONG COMMAND" << std::endl;
        }
    }
    close();
    return 0;
}
