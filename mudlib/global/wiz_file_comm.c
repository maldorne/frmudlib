// Radix - Added wis_present call to 'cd' allowing 'cd here'
// or any other object, same as 'ed'
// December 15, 1995
/* Baldrick started mesing with it 1997-06
 *
 */

/* Strange order on this inherits. */

inherit "/global/le";
inherit "/global/wiz_object_comm";
inherit "/global/player.c";
inherit "/global/wiz_info_comm";
inherit "/global/wiz_communicate";

#define MASTER "/secure/master"

static string in_editor;
static int file_mod_time;                   // Radix, 1996

varargs object *wiz_present(string str, object onobj, int nogoout);
string desc_object(mixed o);
string desc_f_object(object o);

// Radix 1996
nomask void set_file_mod_time(int t) { file_mod_time = t; }
nomask int query_file_mod_time() { return file_mod_time; }

// Randor brought these back, 27-mar-98
int query_ed_setup() { return ed_setup; }
void set_ed_setup(int i) { ed_setup = i; }

static void wiz_commands() {
    wiz_object_comm::wiz_commands();
    wiz_info_comm::wiz_commands();
    wiz_communicate::wiz_commands();
} /* wiz_commands() */

static void app_commands() {
//    wiz_object_comm::app_commands();
    wiz_info_comm::app_commands();
} /* app_commands() */

void all_commands() {
    add_action("change_dir","cd");
    add_action("edit","ed");
    add_action("le", "le");
    add_action("set_home_dir", "homedir");
    //wiz_object_comm::all_commands();
    wiz_info_comm::all_commands();
} /* all_commands() */

static int edit(string str) {
    string *filenames, spam;
    object *things;
    int egg;

    if ( this_player()->query_current_action_forced() )
      return 0;

    if (!str) {
        in_editor = "(hidden)";
        ed("frog", "fini_editor");
        return 1;
    }
    /* dodgy idea, but allows 'ed here' or 'ed strawberry' */
    if (sizeof(things = wiz_present(str, this_player()))) {
        spam = file_name(things[0]);
        sscanf(spam, "%s#%d", spam, egg);
        if(spam[0..6] == "/global")
           filenames = get_files(str);
        else
        if (file_size(spam) < 0)
            filenames = ({ spam+".c" });
        else
            filenames = ({ spam });
    } else
        filenames = get_files(str);
    if (!sizeof(filenames)) {
        str = get_path(str);
    } else {
        if (sizeof(filenames) > 0) {
            str = filenames[0];
            if (sizeof(filenames) > 1) {
                int loop;

                loop = 0;
                while(loop < sizeof(filenames) && file_size(filenames[loop]) < 0)
                    loop++;
                if(loop >= sizeof(filenames)) {
                    write("No such file.\n");
                    return 0;
                }
                else {
                    str = filenames[loop];
                }
                write("Ambiguous, using : " + str + "\n");
            }
        }
    }
    if (file_size(str) == -2) {
        write("directory\n");
        return 1;
    }
    in_editor = str;
    if (!MASTER->valid_write(str, geteuid(), "frog"))
        write("[read only] ");
    ed(str, "fini_editor");
    this_object()->set_trivial_action();
    return 1;
} /* edit() */

void set_in_editor(mixed fname) {
  int fsize = strlen(file_name(previous_object()));
  if(file_name(previous_object())[fsize-8..fsize-1] == "/cmds/ed")
    in_editor = fname;
  // Radix 1996
  if(sizeof(get_dir(fname,-1)) >1)
     set_file_mod_time(get_dir(fname,-1)[2]);
}
mixed query_in_editor() { return in_editor; }

void fini_editor() {
    // Radix
    master()->check_file_mod_time(in_editor,this_object());
    in_editor = 0;
} /* fini_editor() */

// Radix - Added wis_present call to 'cd' allowing 'cd here'
// or any other object, same as 'ed'
// December 15, 1995
static int change_dir(string str) {
    string *filenames;
    object *obs = ({ });
    string tmp = 0;
    string *arr = ({ });

    if ( this_player()->query_current_action_forced() )
    return 0;

    if (!str)  {
	if(!home_dir) {
	    notify_fail("No homedir.  Use homedir to set it.\n");
	    return 0;
	}
	str = home_dir;
    }
    filenames = get_files(str);
    if(sizeof(filenames) > 1) {
	notify_fail("Ambiguous directory.\n");
	return 0;
    }
    if(!sizeof(filenames))
    {
	obs = wiz_present(str,this_player());
	if(!sizeof(obs))
	{
	    notify_fail("No such directory.\n");
	    return(0);
	}

	// Added 'cd <immortal>' changes your path to their path
	// Radix
	if(interactive(obs[0]) && obs[0]->query_creator())
	    filenames = get_files(obs[0]->query_path());
	else
	{
	    tmp = virtual_file_name(obs[0]);
	    if(!stringp(tmp))
	    {
		notify_fail("Error: Bad wiz_present call, cd failed.\n");
		return(0);
	    }
	    arr = explode(tmp,"/");
	    str = implode(arr[0..sizeof(arr)-2],"/");
	    filenames = get_files("/"+str);
	}

	// Unlikely a loaded object won't have a dir, but leave it
	// to a bastard to do such things...
	// Radix
	if(!sizeof(filenames))
	{
	    notify_fail("No such directory.\n");
	    return(0);
	}
    }
    str = filenames[0];
    if (file_size(str) != -2) 
	write("Bad directory : " + str + ".\n");
    else
	current_path = str;
    write(current_path+"\n");
    this_object()->set_trivial_action();
    return 1;
} /* change_dir() */

string query_path() { return current_path; }
string set_path(string ping) { current_path = ping; }

static int set_home_dir(string str) {
    if ( this_player()->query_current_action_forced() )
      return 0;
    if (str) home_dir = get_path(str);
    write("Home directory set to "+home_dir+".\n");
    this_object()->set_trivial_action();
    return 1;
} /* set_home_dir() */

int review() {
    player::review();
    wiz_info_comm::review();
    this_object()->set_trivial_action();
    return 1;
} /* review() */

/* Not sure if this is used but it was in wiz_inter_comm.c which is gone now.
 * Baldrick, jun '97
 */
/*	Comparison function for alphabetical sorting */
int compare(string one, string two) {  return strcmp(lower_case(one), lower_case(two));  }

