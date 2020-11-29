#ifndef BOTCONNECTION_H
#define BOTCONNECTION_H


#include "dbwrapper.h"
#include "user.h"
#include <folly/AtomicHashMap.h>

class BotConnection
{

    UCaseMap* umap;
    tgbot::LongPollBot& bot;
    DBWrapper& db;
    const std::string& token;
    folly::AtomicHashMap<int64_t, User> users;

    HEADER(start)
    HEADER(reg)
    void setPort(const tgbot::types::Message& m, const tgbot::methods::Api& api,  const std::vector<std::string>& args, const bool mode);

    void processMessage(const tgbot::types::Message& m, const tgbot::methods::Api& api);
    User &locateUser(const int64_t idChat);
    void logCmd(const tgbot::types::Message& m,const std::vector<std::string>& args);
public:
    BotConnection(tgbot::LongPollBot& bot,DBWrapper& db,const std::string& token);
    virtual~ BotConnection();
};

#endif // BOTCONNECTION_H
