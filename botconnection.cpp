#include <sstream>
#include <thread>
#include "xxtelebot/include/tgbot/methods/types.h"
#include "botconnection.h"


using namespace tgbot;
using namespace types;
using namespace methods;
using namespace utils;
using namespace std;
using namespace std::placeholders;

#define BODY(n)\
void BotConnection:: n(const Message& m, const Api &api, const std::vector<string>& args)\
{\
    logCmd(m,args);\
    locateUser(m.chat.id).n(m,api,args);\
}


void BotConnection::setPort(const Message &m, const Api &api, const std::vector<string> &args, const bool mode)
{
    logCmd(m,args);
    locateUser(m.chat.id).setPort(m,api,args,mode);
}

void BotConnection::processMessage(const Message &m, const Api &api)
{    
    locateUser(m.chat.id).processMessage(m,api);
}

BODY(reg)

void BotConnection::start(const Message& m, const Api &api, const std::vector<string>& args)
{
    logCmd(m,args);
    api.sendMessage(to_string(m.chat.id),R"(
Управление ботом:
/reg _ИД_ _ИМЯ_ зарегистрировать новое устройство

/on  включить все каналы на устройстве, которое
     управлялось в последний раз

/off отключить все каналы на устройстве, которое
     управлялось в последний раз

/on (/off)  _ИМЯ_         вклюить (отключить) каналы на
            устройстве _ИМЯ_

/on (/off)  _ИМЯ_ _КАНАЛ_ вклюить (отключить) _КАНАЛ_ на
            устройстве _ИМЯ_
)",
                    methods::types::ParseMode::MARKDOWN);
}

::User &BotConnection::locateUser(const int64_t idChat)
{    
    return users.emplace(idChat, db, umap, token).first->second;
}

void BotConnection::logCmd(const Message &m, const std::vector<std::string>& args)
{    

}

BotConnection::BotConnection(tgbot::LongPollBot& bot, DBWrapper &db, const string &token):bot(bot),db(db),token(token), users(1024)
{
    UErrorCode er=U_ZERO_ERROR;
    umap=ucasemap_open("utf-8",0,&er);

    if(er!= U_ZERO_ERROR)
    {
        throw std::logic_error("could not load case map");
    }

    bot.callback(whenStarts,bind(&BotConnection::setPort,this,_1,_2,_3, false),"/off");
    bot.callback(whenStarts,bind(&BotConnection::setPort,this,_1,_2,_3, true),"/on");
    bot.callback(whenStarts,bind(&BotConnection::reg,this,_1,_2,_3),"/reg");
    bot.callback(whenStarts,bind(&BotConnection::start,this,_1,_2,_3),"/start");
    bot.callback(whenStarts,bind(&BotConnection::start,this,_1,_2,_3),"/help");
    bot.callback(bind(&BotConnection::processMessage,this,_1,_2));
}

BotConnection::~BotConnection()
{
    ucasemap_close(umap);
}
