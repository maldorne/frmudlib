/* Coded by Hellbent 971016 */

#include <port.h>
#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

#define USER_HANDLER "/obj/handlers/user_handler.c"

void setup()
{
    position = 0; 
}

string query_usage()
{
    return "portstatus";
}

string query_short_help()
{
    return "Shows number of players per port.  Less people on a port"
    " means a bit less lag for that port.\n";
}

static int cmd( string arg, object me )
{
    int port3000, port3100, totalusers;

    port3000 = USER_HANDLER->get_num_play( PLAY1_PORT );
    port3100 = USER_HANDLER->get_num_play( PLAY2_PORT );
    totalusers = port3000 + port3100;

    write( "There are "+
      port3000+" players on port 3000, and "+port3100+" players on"
      " port 3100,\nfor a total of "+totalusers+
      " players.\n" );

    return 1;
}
