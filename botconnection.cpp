#include "botconnection.h"
#include <sstream>
#include <thread>
#include "xxtelebot/include/tgbot/methods/types.h"


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
    api.sendMessage(to_string(m.chat.id),"Привет! Я простой бот для штабных тренировок."
                                         "Я умею:\nзапоминать новое штабное /new.\n"
                                         "Выдавать штабное по имени /give.\n"
                                         "Выдавать следующее нерешенное штабное /next.\n"
                                         "Выдавать список известных мне штабных /list.\n"
                                         "Дать подсказку к решению штабного /hint.\n"
                                         "Выдать статистику решения /stat.\n"
                                         "Удалить задание /delete.\n"
                                         "Разрезать картинку - команда /split, например\n"
                                         "/split 6 - резать горизонтально на 6 частей\n"
                                         "/split 3x6 - резать по горизонтали на 3, а по вертикали на 6\n"
                                         "Резать можно во время решения штабных.\nУспехов!");
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
