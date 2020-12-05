#include "dbwrapper.h"
#include <byteswap.h>
#include <iostream>
#include <iomanip>

using namespace std;

DBWrapper::DBWrapper(const std::string& user,
                     const std::string& password,
                     const std::string& db,
                     const std::string& host,
                     const uint16_t port
                     ):conn(PQsetdbLogin(host.c_str(),
                                         std::to_string(port).c_str(),
                                         nullptr,
                                         nullptr,
                                         db.c_str(),
                                         user.c_str(),
                                         password.c_str()))
{    
    if(PQstatus(conn) != CONNECTION_OK)
    {
        throw DBFailed(conn);
    }

    if(PQsetClientEncoding(conn,"UTF-8"))
    {
        throw DBFailed(conn);
    }

    PREPARE(regDevice,         "insert into devices(\"user\", device, name) values ($1, $2, $3)", 3);
    PREPARE(qrySetDevice,      "update status set status = status |  (1 << $2) where device = (select device from devices where name = $1 and \"user\" = $3)", 3);
    PREPARE(qryUnSetDevice,    "update status set status = status & ~(1 << $2) where device = (select device from devices where name = $1 and \"user\" = $3)", 3);
    PREPARE(qrySetDeviceAll,   "update status set status = ~0                  where device = (select device from devices where name = $1 and \"user\" = $2)", 2);
    PREPARE(qryUnSetDeviceAll, "update status set status =  0                  where device = (select device from devices where name = $1 and \"user\" = $2)", 2);
}

void DBWrapper::registerDevice(const int64_t user, const string &device, const string& name)
{
    lock_guard lck(mtx);
    const char * values[]=         {
                                    to_string(user).c_str(),
                                    device.c_str(),
                                    name.c_str()
                                   };
    const AutoRes res(PQexecPrepared(conn,regDevice,3,values,nullptr,nullptr,0));
    DBFailed::check(conn,res,PGRES_COMMAND_OK);
}

void DBWrapper::setDevice(const string &device, const int port, const bool mode, const int64_t user)
{
    lock_guard lck(mtx);
    const char * values[]=         {
                                    device.c_str(),
                                    to_string(port).c_str(),
                                    to_string(user).c_str()
                                   };
    const AutoRes res(PQexecPrepared(conn,( mode ? qrySetDevice : qryUnSetDevice ), 3, values, nullptr, nullptr, 0));

    DBFailed::check(conn,res,PGRES_COMMAND_OK);
}

void DBWrapper::setDeviceAll(const string &device, const bool mode, const int64_t user)
{
    lock_guard lck(mtx);
    const char * values[]=         {
                                    device.c_str(),
                                    to_string(user).c_str()
                                   };
    const AutoRes res(PQexecPrepared(conn,( mode ? qrySetDeviceAll : qryUnSetDeviceAll ), 2, values, nullptr, nullptr, 0));
    DBFailed::check(conn,res,PGRES_COMMAND_OK);
}

DBWrapper::~DBWrapper()
{
    PQfinish(conn);
}

DBWrapper::DBFailed::DBFailed(const PGconn *conn):msg(PQerrorMessage(conn))
{

}

const char *DBWrapper::DBFailed::what() const noexcept
{
    return msg.c_str();
}

void DBWrapper::DBFailed::check(const PGconn *conn, const PGresult *res,const ExecStatusType type)
{
    const auto status = PQresultStatus(res);
    if(status != type)
    {
        throw DBFailed(conn);
    }
}

AutoRes::AutoRes(PGresult *res):unique_ptr<PGresult,void(*)(PGresult*)>(res,&PQclear)
{

}

AutoRes::operator PGresult *() const
{
    return get();
}
