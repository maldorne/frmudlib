#pragma strict_types
#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){

position = 1;
}



static int cmd(string str, object me) {
    string *filenames = ({ }), tmp;
    object *things;
    int loop, egg;

  if (!str) {
    notify_fail("Tail what file ?\n");
    return 0;
  }
  if(sizeof(things = this_player()->wiz_present(str, this_player()))) {
    tmp = file_name(things[0]);

    sscanf(tmp, "%s#%d", tmp, egg);
    if (file_size(str) <= 0)
      filenames += ({ tmp + ".c" });
  }
  filenames += this_player()->get_files(str);
  if (!sizeof(filenames)) {
     notify_fail(str + ": No such file.\n");
    return 0;
  }
  for(loop = 0; loop < sizeof(filenames); loop++) {
    str = filenames[loop];
    if(sizeof(filenames) > 1) {
       write("FILE : " + str + "\n");
    }
    if (!tail(str))
      write("No such file.\n");
   }
  return 1;
} /* tail_file() */
 
