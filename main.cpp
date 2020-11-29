#include <iostream>
#include "botconnection.h"
#include "botoptions.h"

using namespace std;


int main(int argc,char* argv[])
{
    BotOptions opts(argc,argv);

    DBWrapper db( $(opts,"login"),
                  $(opts,"password"),
                  $(opts,"db"),
                  $(opts,"host"),
                  $(opts,"port").no
                );


    tgbot::LongPollBot bot($(opts,"token"));

    BotConnection bt(bot,db,$(opts,"token"));

    bot.start();

    return 0;
}
