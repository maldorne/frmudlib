#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
#define PEOPLER "/obj/handlers/peopler"

void setup()
{
position = 1;
}

static int cmd(string str, object me) {
  me->set_trivial_action();
  return (int)PEOPLER->do_qpeople(str);
} /* do_people() */


