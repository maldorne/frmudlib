//Ljen nov -96
// Joram, 18 of September '96

inherit "/std/outside";
#include "path.h"

void setup(){
   set_short("Savannah");
   set_long("Going to the east the savannah opens in all its magnificence :"
            "the tall tress are the unique big vegetal life you can see around"
            ", forming the very core of the ecosystem.  Although the grass that "
            "grows in here is the principal place where the vital circle starts, "
            "this is not visible to you and it dims when looking at the great "
            "predators and herbivorous animals.\n");

    set_light(80);

    add_exit( "west",   SAVANNAH+"e1",   "path" );
    add_exit( "east",   SAVANNAH+"g1",   "path" );
}
