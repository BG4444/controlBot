#ifndef BOTCONNECTION_H
#define BOTCONNECTION_H


#include "dbwrapper.h"
#include "user.h"

class BotConnection
{

    UCaseMap* umap;
    tgbot::LongPollBot& bot;
    DBWrapper& db;
    const std::string& token;
    std::map<int64_t, User> users;

    HEADER(start)
    HEADER(reg)

    void processMessage(const tgbot::types::Message& m, const tgbot::methods::Api& api);
    User &locateUser(const int64_t idChat);
    void logCmd(const tgbot::types::Message& m,const std::vector<std::string>& args);
public:
    BotConnection(tgbot::LongPollBot& bot,DBWrapper& db,const std::string& token);
    virtual~ BotConnection();
};

#endif // BOTCONNECTION_H
