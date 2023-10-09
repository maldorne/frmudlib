
/***************************************************************************
* 									   *
*        Ranged weapon code ... should include _all_ thrown weapons        *
*              Before do any read the RANGED_DOC.TXT			   *
*                     Radix - March 3, 1996                                *
*                     Khelben - Juny 23, 1996                              *
*                  							   *
****************************************************************************/
 
inherit "/obj/weapon.c";
 
#include "path.h"
#define RANGE "/obj/handlers/range.c"
// info[] 0- range 1- speed 2- accuracy 3-container 4-ammo 5-hands  
 
mixed info,tmp;
string wtype;
object me, him, env;
int round;
string firemess, readymess; 

private int *ranged_damage;
object shadow;
string ammo;                     
 

int do_hit(object me, object him);
void do_random_target(object me, object him);
void missed_shot(object me, object him);

void lose_concentration();
 
void set_readymess(string arg) { readymess = arg; }
void set_firemess(string arg) { firemess = arg; }








int do_ranged_weapon_damage(object archer, object enemy, mixed tmp);
 
void anger_npc(object a, object e, mixed tmp);   // Will do NPCs attack 
					        //    players. 


int *query_weapon_damage() { return ranged_damage; }
 
int query_ranged_weapon_ammo() { return 1; }
int query_ranged_ammo() { return 1; }

 
create()
{
   ::create();
   info = allocate(6);
   if(wtype)
      info = TABLE->get_info(wtype);
   me, him = ({ });
   round = 0;
   if ( !readymess )
      readymess = "Skillfully, you draw the bow ready and aim at";
   if ( !firemess )
      firemess = "Finishing your aim, you shoot off an arrow at";
// Why this?  Doesn't need to be in base object - Radix
// add_property("guild","ranger");   This must be put in every weapon.
   set_holdable(1);
   if ( (int)info[5] == 2 )
   {   set_twohanded(1); }
}


void init()
{
   if ( (int)this_object()->query_property("cursed") == 100 && !(shadow) )
   {
      if( (string)this_player()->query_cap_name() == 
          (string)this_object()->query_property("stiked") )
      {
         // This clone the arrow shadow when the player log off with a 
         // stinking arrow. With the property "cursed" we are sure that 
         // he cannot drop the arrow.    
      
         shadow = clone_object(SHOOT_SHADOW);
         shadow->setup_shadow(this_player() , this_object());
         this_player()->add_extra_look(shadow);
      }
      else
      {
         this_object()->remove_property("cursed");
         this_object()->remove_property("stiked");      
      }
   }
   add_action("extract_proj","extract");
   add_action("ranged_throw","throw");
   ::init();
}
 
void set_ranged_weapon(string t) 
{
    wtype = TABLE->look_up(t);
    info = TABLE->get_info(wtype);
}
 
int ranged_throw(string victim)
{
    object old = him;
    
    if(!this_object()->query_in_use())
    {
       write("You must be holding this weapon to use it!\n");
       return 1;
    }
    if(!victim || !stringp(victim))
    {
       write("Syntax: "+query_verb()+" <target>\n");
       return 1;
    }
    if(environment()) me = environment(this_object());
    victim = me->expand_nickname(victim);
    him = find_living(victim);
    if(!me || !him)
    {
       write(capitalize(victim)+" does not exist or is a ghost.\n");
       return 1;
    }
    if(him && him == me)
    {
       write("Suicide is not possible, try to kill something else.\n");
       return 1;
    }
    if(him->query_property("dead"))
    {
       write(him->query_cap_name()+" is a ghost.\n");
       return 1;
    }
    if(!RANGE->check_line_of_sight(me,him,info[0]) && ENV(me) != ENV(him))
    {
       write("Could not find "+him->query_cap_name()+" in range.\n");
       return 1;
    }
    if(him->query_hidden()||him->query_invis()||him->query_hide_shadow())
    {
       write("Could not find "+him->query_cap_name()+" in range.\n");
       return 1;
    }
    
    if(!round)
       write(readymess+" "+him->query_cap_name()+".\n");
    else
    {
       if(him == old)
          write("You hesitate and aim at "+him->query_cap_name()+
             " again.\n");
       else
          write("You change aim from "+old->query_cap_name()+" to "+
             him->query_cap_name()+".\n");
    }
    env = environment(me);
    round = 0;
    set_heart_beat(1);
    return 1;
}
 
heart_beat()
{
   int hit;

   if(!him || !me)
   {
      if(!me)
      {
          // die in horror
          dest_me();
          return;
      }
      tell_object(me,"You lost sight of "+him->query_cap_name()+".\n");
      me, him = 0;
      round = 0;
      set_heart_beat(0);
      return;
   }   
   if(env != environment(me))
      lose_concentration();
      
   if(!this_object()->query_in_use())
   {
     tell_object(me,"As you low your weapon you lose your target.\n");
     set_heart_beat(0);
     return;
   }
   if(him->query_property("dead"))
   {
      write(him->query_cap_name()+" is a ghost.\n");
      set_heart_beat(0);
      return;
   }
   if(!RANGE->check_line_of_sight(me,him,info[0])&& env != environment(him))
   {
      tell_object(me,him->query_cap_name()+" is out of range!\n");
      me, him = 0;
      round = 0;
      set_heart_beat(0);
      return;
   }
   if(him->query_hidden() || him->query_invis() || him->query_hide_shadow())
   {
      tell_object(me,"Could not find "+him->query_cap_name()+" in range.\n");
      me, him = 0;
      round = 0;
      set_heart_beat(0);
      return;
   }
 
   if(round == info[1])
   {
      tell_object(me,firemess+" "+him->query_cap_name()+".\n");

      tell_room(ENV(me), me->query_cap_name()+" holds a "+query_short()+
                " and shoot off a projectile.\n", ({me}));

      if(!do_hit(me,him))
      {
         missed_shot(me,him);   // Wrong the shot. 
      }
      me, him = 0; 
      round = 0;
      set_heart_beat(0);
      return;
   }
   
   round++;
   tell_object(me,"You continue your aim at "+him->query_cap_name()+".\n");
}
 
void lose_concetration()
{
   tell_object(me,"Moving, you lose your concentration and begin "
      "again aiming at "+him->query_cap_name()+".\n");
   env = environment(me);
   round = 0;
}

int check_accuracy(object archer, object enemy, int tmp)
{
   int bonus, malus, ac, thac0, accuracy, distance, dex;
   int needed, shoot;
   int prop;

   if( enemy->query_property("NOBUDGE") ||
        environment(enemy)->query_static_property("noguild") ||
        environment(enemy)->query_static_property("nocast") )
    {
    	return -1;    // We'll do them lose ammo.
    }
    if( environment(archer)->query_static_property("noguild") ||
        environment(archer)->query_static_property("nocast") )
    {
    	return -1;    // We'll do them lose ammo.
    }
   ac = enemy->query_total_ac();
   thac0 = archer->query_thac0();
   accuracy = info[2];
   distance = tmp*2;   // Will be hardest if the target is far 
   distance *= 10;
   dex = (int)archer->query_dex()-16;
   dex *= 10;   
   dex /= 2;
   
   needed = thac0 - ac + distance - accuracy - dex;

       // The following was added as petition from Cailet and Verkho.

   if(!archer->query_property(wtype))
      needed = needed + 40;
   else
   {
      prop = (int)archer->query_property(wtype);
      prop *= 25;   // 10 * 2.5 
      needed = needed - prop;
   } 
   shoot = random(200);
   if ( shoot >= needed )
   	return 1;   
   else
   	return 0;
}
 
void missed_shot(object me, object him)
{
   int bad;

   tell_object(me,"You missed "+him->query_cap_name()+"!\n"); 
   bad = random(200);
   
   switch (bad)
   {
      case 0..198:
		    this_object()->move(environment(him));
		    break;
   
      default:     // 199 
   		    do_random_target(me,him);     // Now we can hit 
   		    				       // another target 
   		    break;		
    }   		    
}
 
int do_hit(object archer, object enemy)
{
   int damage,i, dest;
   int shoot;
   
   tmp = RANGE->get_direction_delta(enemy, archer);
   
   
   shoot = check_accuracy(archer, enemy, tmp[1]);
   
   if ( shoot == -1)
   {
      tell_object(me,"You missed "+enemy->query_cap_name()+"!\n"); 
      this_object()->dest_me();
      return 1;     // In this way We'll avoid the miss_shoot function.
   }
      
   if(shoot == 1)
   {
      damage = do_ranged_weapon_damage(archer,enemy,tmp);
      tell_object(archer,"You hit "+enemy->query_cap_name()+" with your "+
                  this_object()->query_short()+" doing "+damage+"!\n");
 
      tell_object(enemy,"You have been struck by a flying "
                  +this_object()->query_short()+
                  " from the "+tmp[0]+" doing "+damage+"!\n");
      tell_room(ENV(enemy), enemy->query_cap_name()+" is hited by a "
                "projectile from the "+tmp[0]+".\n", ({archer,enemy}));
                
      if ( !userp(enemy) )  
      {  
         if( wtype == "throwing knife" )
            this_object()->move(enemy); 
         else
	    this_object()->move(environment(enemy));
      } 
      return 1;
   }
   else if( shoot == 0)
   	return 0;    // Wrong the shoot 
}

void do_random_target(object archer, object enemy)
{ 
   int damage, j, i, dest;
   int deb=0, di;
   object *all;
   
   all = all_inventory(environment(enemy));
   j = sizeof(all); 
   if ( j == 1 && ENV(archer) != ENV(enemy) )  
   {
        // Only our target is in the room 
      	
      	this_object()->dest_me();
   }
   
   else if ( j == 2 && ENV(archer) == ENV(enemy) )    
   {   
        // Only our target and the archer are in the room 
      	
      	this_object()->dest_me();
   }
   
   else
   {
   	for(di=0; di<sizeof(all); di++)
   	i = random(sizeof(all));
   	while ( all[i] == enemy  || all[i] == archer ) 
   	{
           i = random(sizeof(all));  
           deb++;
        }   
   }
   
   if ( userp(all[i]) || all[i]->query_npc() )
   {
      damage = do_ranged_weapon_damage(archer,all[i],tmp);
      tell_object(archer,"Opps...you wrong the shot and you now "+
                  "hit "+all[i]->query_cap_name()+" with your "+
  	          this_object()->query_short()+" doing "+damage+"!\n");
      tell_object(all[i],"You have been struck by a flying "
                  +this_object()->query_short()+
                  " from the "+tmp[0]+" doing "+damage+"!\n");
      tell_object(all[i],"Ahora se te clona una flecha clavada.\n");
      tell_room(ENV(enemy), enemy->query_cap_name()+" is hited by a "
                "projectile from the "+tmp[0]+".\n", ({archer,enemy}));
   }
   
   else
      this_object()->dest_me();
      
   if ( !userp(all[i]) )
   {  
      if( wtype == "throwing knife" )
         this_object()->move(enemy); 
      else
         this_object()->move(environment(enemy));
   }
}


void set_weapon_damage(int *dam) 
{ 
   if(!dam || !sizeof(dam)) return;
   ranged_damage = dam;
   return;
}

int do_ranged_weapon_damage(object archer, object enemy, mixed tmp)
{
   int dam;

   if(!archer || !enemy) return 0;
   if ( sizeof(ranged_damage) != 3 )   /* Not necessary. Can be erased */
      ranged_damage = ({ 1, 5, 0 });
    
   dam = roll(ranged_damage[0],ranged_damage[1])+ranged_damage[2];
   dam = dam + this_object()->query_enchant();
   dam = dam + archer->query_damage_bonus();
 
   if(dam < 0) dam = 1;
   if(dam > 20) dam = 20;
 
   if(environment(enemy) == environment(archer)) 
   {
      if(archer->query_hidden_shadow() || archer->query_hidden())
      {
         dam *= 2;
         archer->destruct_hide_shadow();
         archer->set_hidden(0);
      }
      else
         dam *= 1.3;
   }
   else
   {
      if(tmp[0] == "down") dam *= 2;
      if(tmp[0] == "up") dam /= 2;
   }
      
   enemy->adjust_hp(-dam,archer);
   if(!userp(enemy))     		  // If he is a NPC will move him. 
         anger_npc(archer, enemy, tmp);
   enemy->attack_by(archer);                 
   if (userp(enemy) && wtype=="throwing knife")   
   {                          // We'll not slay NPCs because they are fools 
      this_object()->add_property("cursed",100);
      this_object()->add_property("stiked",enemy->query_cap_name());      
      shadow = clone_object(SHOOT_SHADOW);
      shadow->setup_shadow(enemy , this_object());
      enemy->add_extra_look(shadow);
   }
   if ( userp(enemy) )
   {  
      if( wtype == "throwing knife" )
         this_object()->move(enemy); 
      else
         this_object()->move(environment(enemy));
   }
   dam = (dam * (100 - (int)enemy->query_property("missile")))/100; 
   return dam;
}
 
void anger_npc(object archer, object enemy, mixed tmp)
{
   if ( enemy->query_hp() >= 0 )
   {
      while(environment(archer) != environment(enemy))
         enemy->do_command(tmp[0]);
   }
}

int extract_proj(string str)
{
   object dagger;

   
   if ( ( 
          str == "dagger" || str == "knife" ||
          str == (string)this_object()->query_proj_name() ||
          str == (string)this_object()->query_name() ||    
          str == (string)this_object()->query_cap_name() ||    
          str == (string)this_object()->query_short() ) &&
          (int)this_object()->query_property("cursed") == 100 )
   {
      if ( random(100) > ((int)this_player()->query_dex() * 5) )
      {
         // The player cannot extract the arrow because he is clumsy  
         
              tell_object(this_player(), "You try extract the stinking "+this_object()->query_name()+" "+
                     "but you only remove it into your flesh.\n");
	 this_player()->adjust_hp(-1,this_object());         
         return 1;
      }
      shadow->destruct_me();
           tell_object(this_player(), "As you extract the "+this_object()->query_name()+" you feel a "
                  "deep pain across all your body, but now you're "
                  "better.\n");
      this_object()->remove_property("cursed");	   
      return 1;
   }
   else
      return 0;
}



    
