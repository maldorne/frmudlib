/*
  /std/object.c 
  Previous hacks by Dank and Pinkfish
  Baldrick cleaned after them.
  Hamlet fixed timed properties.
  Radix made create_me more accurate - 7/96
  Wonderflug cleaned up as usual.  Removed dynamic/static distinction.
    Got rid of old init_data stuff.
*/

inherit "/std/basic/property";
inherit "/std/basic/extra_look";
inherit "/std/basic/read_desc";
inherit "/std/basic/misc";
inherit "/std/basic/id";

string create_me;
 
void create() 
{
  create_me = "Object";
  id::create();
  property::create();
  extra_look::create();
  misc::create();
  read_desc::create();
  seteuid( master()->creator_file( file_name(this_object()) ) );
  // Radix, July 1996
  create_me = master()->get_create_me( create_me );
  this_object()->setup();
}

void set_name(string str) 
{
  if (name && name != "object") 
    return ;

  name = str;

  if (!short_d)
    short_d = str;

  add_plural(pluralize(str));
}

int query_enchant() { return 0; }
status set_enchant(int i) { return 0; }
status query_weapon() { return 0;}
status query_armour() { return 0; }

mixed *stats() 
{
  return ::stats() + ({
    ({ "name" , name, }),
    ({ "short", short(0), }),
    ({ "plural", query_plural(), }),
    ({ "value", query_value(), }),
    ({ "weight", query_weight(), }),
    ({ "cloned by", create_me, }),
   });
}

/*
 * This returns a mapping of all the data that is associated with the object.
 * The mapping can then be saved in the player object, is the idea.
 * However for many derived objects we don't need all the info, eg
 * a domain object will set all the name/short/plurals etc itself;
 * so we need not save all that info for a derived object, and so don't
 * return it at all if derived is nonzero (ie it is a derived object).
 */
mapping query_auto_load_attributes(int derived) 
{
  if ( derived < 0 )
  {
    if ( file_name(this_object())[0..10] == "/std/object")
      derived = 0;
    else
      derived = 1;
  }

  if ( !derived )
    return 
      ([ 
        "weight" : weight, 
        "light" : light, 
        "short" : short_d, 
        "long" : long_d, 
        "adjective" : adjectives, 
        "alias" : alias, 
        "plural" : plurals, 
        "name" : name, 
        "main plural" : plural_d, 
        "value" : value,
        /* following used to be dynamic */
        "properties" : map_prop,
        "timed"     : freeze_timed_properties(timed_prop), /* Hamlet */
        "read mess" : read_mess,
        "cloned by" : create_me,
      ]);
  else
    /* derived object, only save stuff that the derived object won't
     * initialize correctly; ie stuff that may have been changed from
     * a regularly cloned instance
     */
    return
      ([
        "properties" : map_prop,
        "timed"     : freeze_timed_properties(timed_prop), /* Hamlet */
        "read mess" : read_mess,
        "cloned by" : create_me,
      ]);
} /* query_auto_load_attributes() */

/*
 * In loading, we need not discriminate between derived/underived objects;
 * either the mapping entries are there are they aren't.
 */

void init_auto_load_attributes( mapping attribute_map )
{
  if (!undefinedp(attribute_map["weight"]))
    set_weight(attribute_map["weight"]);
  if (!undefinedp(attribute_map["light"]))
    set_light(attribute_map["light"]);
  if (!undefinedp(attribute_map["short"]))
    set_short(attribute_map["short"]);
  if (!undefinedp(attribute_map["long"]))
    set_long(attribute_map["long"]);
  if (attribute_map["adjective"])
    set_adjectives(attribute_map["adjective"]);
  if (attribute_map["alias"])
    set_aliases(attribute_map["alias"]);
  if (attribute_map["plural"])
    set_plurals(attribute_map["plural"]);
  if (!undefinedp(attribute_map["name"]))
    set_name(attribute_map["name"]);
  if (!undefinedp(attribute_map["main plural"]))
    set_main_plural(attribute_map["main plural"]);
  if (!undefinedp(attribute_map["value"]))
    set_value(attribute_map["value"]);

  if (attribute_map["properties"])
    map_prop = attribute_map["properties"];
  /* Let's restore timed -- Hamlet */
  if (attribute_map["timed"])
    timed_prop = thaw_timed_properties(attribute_map["timed"]);
  set_read_mess(attribute_map["read mess"]);
  create_me = attribute_map["cloned by"];
}

int query_dead()
{
 return 1;
}

// The following is from the TMI-2 Lib. Asmodean Put it here
// No reason to make it so utterly messy.  Cleanup by you guessed it.
int clean_up()
{
  object env, *contents;
  int i;

  if (userp(this_object())) 
    return 1; /* don't clean_up players */

  env = environment();
  if (env && userp(env)) 
    return 1; /* carried ob */
  if (env && environment(env))
    return (int)environment(env)->clean_up(); /* recurse */

  contents = deep_inventory(this_object());
  if (contents) 
  {
    for (i=0;i<sizeof(contents);i++)
      if (userp(contents[i])) 
        return 1; /* holding a user */
  }
  if (!env) 
  {
    /* we're a room with no users inside or a lost object */
    dest_me();
    return 1;
  }
  return 1; /* try again later... */
}

nomask  string query_create_me() { return create_me; }
