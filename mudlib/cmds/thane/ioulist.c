#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup()
{
    position = 1;
}


static int cmd(mixed name, object me) {
// Quick hack of a frooble for auto_load IOUs...
// Radix May 1, 1996
   int i;
   object pl, *all;
   mapping tmp = ([ ]);
   if(!name)
   {
      notify_fail("Syntax: ioulist <object>\n");
      return 0;
   }
   pl = this_player()->wiz_present(name,this_player())[0];
   if(!pl) 
   {
      notify_fail("Could not find "+name+"\n");
      return 0;
   }
   all = deep_inventory(pl);
   tell_object(me, "Looking...\n");
   for(i=0; i<sizeof(all); i++)
   {
      if(all[i] && all[i]->query_iou_object())
      {
         tmp = all[i]->query_static_auto_load();
         if(sizeof(tmp["auto_string"]))
         {
             tell_object(me,sprintf("%O - ",file_name(all[i])));
             tell_object(me,sprintf("%O\n",tmp["auto_string"][1]));
         }
      }
   }
   tell_object(me, "Done.\n");
  me->set_trivial_action();
   return 1;
}
