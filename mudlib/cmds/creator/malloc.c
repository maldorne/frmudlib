#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup()
{
position = 1;
}

static int cmd(string str, object me) {
  tell_object(me,malloc_status());
  me->set_trivial_action();
  return 1;
} /* do_malloc() */


