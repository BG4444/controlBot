#ifndef DBWRAPPER_H
#define DBWRAPPER_H
#include <stdlib.h>
#include <libpq-fe.h>
#include <string>
#include "xxtelebot/include/tgbot/bot.h"
#include "xxtelebot/include/tgbot/types.h"
#include "xxtelebot/include/tgbot/utils/make_ptr.h"

#define QUERY(n) static constexpr char n[]=#n;
#define PREPARE(query,text,count) DBFailed::check(conn,PQprepare(conn,query,text,count,nullptr),PGRES_COMMAND_OK);

class AutoRes : public std::unique_ptr<PGresult,void(*)(PGresult*)>
{
public:
    AutoRes(PGresult* res);
    operator PGresult*() const;
};

class DBWrapper
{
    class DBFailed: public std::exception
    {
        std::string msg;
    public:
        DBFailed(const PGconn* conn);
        const char* what() const noexcept override;
        static void check(const PGconn* conn, const PGresult * res, const ExecStatusType type);
    };

    PGconn* conn;

    QUERY(regDevice)
    QUERY(qrySetDevice)
    QUERY(qryUnSetDevice)

public:
    DBWrapper(const std::string& user,
              const std::string& password,
              const std::string& db,
              const std::string& host, const uint16_t port);
    void registerDevice(const int64_t user, const std::string& device, const std::string &name);
    void setDevice(const std::string& device, const int port, const bool mode);
    virtual ~DBWrapper();
};

#endif // DBWRAPPER_H
