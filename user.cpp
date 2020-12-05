#include "user.h"
#include <sstream>
#include <iterator>

using namespace std;


string User::toUtf8Upper(const string &in)
{
    string ret;
    ret.resize(in.size()*3);
    UErrorCode er=U_ZERO_ERROR;
    const auto len=ucasemap_utf8ToUpper(umap,ret.data(),ret.capacity(),in.c_str(),in.length(),&er);

    if(er==U_ZERO_ERROR)
    {
        ret.resize(len);
        return ret;
    }

    cerr << in << " encoding failed!["<<er << "]"  << endl;
    return in;
}

void User::doMessageStillWorking(const tgbot::types::Message &m, const tgbot::methods::Api &api)
{
    stringstream str;
    str<<"Мы еще не закончили ";

    switch(state)
    {
        case Idle:
        return;

    }

    str << endl
        << "Чтобы я от тебя отстал, скомандуй /end.";

    api.sendMessage(to_string(m.chat.id), str.str());
}

void User::toDefaultState()
{
    if(oldStates.empty())
    {
        state=Idle;
    }
    else
    {
        state=oldStates.top();
        oldStates.pop();
    }
}

void User::pushState(const User::States newState)
{
    oldStates.push(state);
    state=newState;
}


string User::extractUserName(const tgbot::types::Message &m)
{
    if(!m.from)
    {
        return "UnKnOwN";
    }
    if (!m.from->username || (m.from->username && m.from->username->empty()))
    {
        return m.from->firstName;
    }
    return *m.from->username;
}


void User::processMessage(const tgbot::types::Message &m, const tgbot::methods::Api &api)
{
    switch(state)
    {      
        default:
            api.sendMessage(to_string(m.chat.id),"Я не жду сообщений. Давай команду :)");
        return;
    }
}

User::User(DBWrapper& db, UCaseMap *umap, const string &token):db(db),
                                                               umap(umap),
                                                               token(token)
{

}

void User::reg(const tgbot::types::Message& m, const tgbot::methods::Api& api,  const std::vector<std::string>& args)
{
    if(state == Idle)
    {
        if(args.size()==1 || args.size() > 3)
        {
            api.sendMessage(to_string(m.chat.id), "Укажите идентификатор устройства и, через пробел, его имя");
        }
        else
        {
            stringstream ans;
            ans << "Устройство "
                << args[1]
                << " с именем "
                << args[2];
            try
            {
                db.registerDevice(m.chat.id, args[1], args[2]);
                ans << " успешно зарегистрировано";

            }
            catch(exception& ex)
            {
                ans << " зарегистрировать не удалось.\nПричина: "
                    << ex.what()
                    << '\n';
            }
            api.sendMessage(to_string(m.chat.id), ans.str());
        }
    }
    else
    {
    }
}

void User::iNeedPort(const tgbot::types::Message& m, const tgbot::methods::Api& api)
{
    api.sendMessage(to_string(m.chat.id), "Укажите имя устройства и порт для управления");
}

void User::doSetPort(const tgbot::methods::Api& api, const tgbot::types::Message& m, const bool mode)
{
    stringstream ans;
    if(port)
    {
        ans << "Устройство "
            << lastDevice
            << " порт "
            << *port
            << " установлен в "
            << mode
            << '\n';
        db.setDevice(lastDevice, *port, mode, m.chat.id);
    }
    else
    {
        ans << "Устройство "
            << lastDevice
            << " все порты установлены в "
            << mode
            << '\n';
        db.setDeviceAll(lastDevice, mode, m.chat.id);
    }
    api.sendMessage(to_string(m.chat.id), ans.str());
}

void User::setPort(const tgbot::types::Message& m, const tgbot::methods::Api& api,  const std::vector<std::string>& args, const bool mode)
{
    switch(args.size())
    {
        case 1:
        {
            if(lastDevice.empty())
            {
                iNeedPort(m, api);
            }
            else
            {
                doSetPort(api, m, mode);
            }
            break;
        }
        case 2:
        {
            lastDevice = args[1];
            port.reset();
            doSetPort(api, m, mode);
            break;
        }
        case 3:
        {
            lastDevice = args[1];
            port = stoll(args[2]);
            doSetPort(api, m, mode);
            break;
        }
        default:
        {
            iNeedPort(m, api);
            break;
        }
    }
}
