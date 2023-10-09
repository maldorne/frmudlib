#include "condition.h"

int cond,
    max_cond;

void setup_condition(int i)
{
  if(max_cond = i + random(COND_RANGE) > COND_MAX) max_cond = i = COND_MAX;
  cond = i;
}

/* this could be masked by the object */
varargs void break_me(object adjuster)
{
  object env = environment();
  if(env && interactive(env)){
    if(adjuster && adjuster != env){
      tell_object(env, "\n"+adjuster->query_cap_name()+
                       " destroys your "+this_object()->query_short()+
                       "!\n");
       tell_object(adjuster, "You destroy "+env->query_cap_name()+
        "'s "+this_object()->query_short()+"!\n");
      tell_room(environment(env),adjuster->query_cap_name()+
         " destroys "+env->query_cap_name()+"'s "
         +this_object()->query_short()+"!\n");
    }
    else    
      tell_object(env, "\nYour "+this_object()->query_short()+
                       " exists no more.\n");
  }
  // Taniwha, can't continue execution after a dest.
// So it'll get rather full :), but it fixes the problem
   this_object()->move("/room/rubbish");
   call_out("dest_me",0);
}

string cond_string()
{
  switch ((100*cond)/COND_MAX)
  {
    case 0 :
      return "It is a wreck.\n";
    case 1..10 :
      return "It is in a very bad condition.\n";
    case 11..20 :
      return "It is in a bad condition.\n";
    case 21..30 :
      return "It is in poor condition.\n";
    case 31..40 :
      return "It is below average condition.\n";
    case 51..60 :
      return "It is average condition.\n";
    case 61..70 :
      return "It is in reasonable condition.\n";
    case 71..80 :
      return "It is in good condition.\n";
    case 81..90 :
      return "It is in very good condition.\n";
    default :
      return "It is in excellent condition.\n";
  }
}
 
varargs int adjust_cond(int i, object adjuster)
{
  if(!i) return cond;

  cond += i;

  if(cond < 0) cond = 0;

// can't make better than max_cond
  if(cond > 0 && max_cond < cond){
    cond = max_cond;
    return cond;
  }
// max_cond can slip...
  if(cond < 0 && max_cond < cond + COND_RANGE)
    max_cond = cond + COND_RANGE;

// the bigger i is and the smaller cond is the bigger the chance of breaking
  if(i < 0 && random(-cond/i) <= COND_FAIL)
    break_me(adjuster);
/*
  if(cond < max_cond / 4 && (cond*400/max_cond + random(100) < 100))
    break_me(adjuster);
*/

  return cond;
}

int query_cond() { return cond; }
int query_max_cond() { return max_cond; }
