#ifndef USER_H
#define USER_H

#include "dbwrapper.h"
#include <unicode/ucasemap.h>
#include <stack>
#include <regex>

#define HEADER(n) void n (const tgbot::types::Message& m,\
                          const tgbot::methods::Api& api,\
                          const std::vector<std::string>& args);
class User
{             
    enum States
    {
        Idle,

    };

    DBWrapper& db;
    UCaseMap* umap;
    const std::string& token;


    States state=Idle;

    std::regex blowCoord;

    std::stack<States> oldStates;

    std::string toUtf8Upper(const std::string& in);
    void doMessageStillWorking(const tgbot::types::Message& m, const tgbot::methods::Api &api);
    void toDefaultState();
    void pushState(const States newState);
public:
    static std::string extractUserName(const tgbot::types::Message& m);
    HEADER(reg);
    void processMessage(const tgbot::types::Message& m, const tgbot::methods::Api& api);
    User(DBWrapper &db,UCaseMap* umap,const std::string& token);
};

#endif // USER_H
