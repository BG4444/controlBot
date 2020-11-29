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
    if(PQresultStatus(res) != type)
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
