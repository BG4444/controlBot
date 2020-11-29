#ifndef BOTOPTIONS_H
#define BOTOPTIONS_H

#include "opt-parser/optparser.h"
#include "opt-parser/netportnumber.h"

struct optlist
{
    static constexpr auto options=tuple_cat(
                OPTPARSER::makeO("port",[](){return OPTPARSER::netPortNumber("5432");}),
                OPTPARSER::makeO<std::string>("host"),
                OPTPARSER::makeO<std::string>("db"),
                OPTPARSER::makeO<std::string>("login"),
                OPTPARSER::makeO<std::string>("password"),
                OPTPARSER::makeO<std::string>("token")
                );

};

class BotOptions:public OPTPARSER::Options<optlist>
{
public:
    BotOptions(int argc,char* argv[]):Options(argc,argv){}
};

#endif // LEARNBOTOPTIONS_H
