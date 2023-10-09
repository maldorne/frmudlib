/* Benedick's Airplane */

inherit "std/container.c";

string str, name;

void setup() {
    add_alias("airplane");
    add_alias("plane");
    set_short("An airplane");
}
string long()
{
    if(environment(this_player()) != this_object())
        return "You are looking at an airplane.  Be afraid.";
    return "You are inside an airplane.  Through the windows you see " +
    (string)environment(this_object())->long();
}
void init() {
    ::init();
    add_action("go","fly");
    add_action("enter","enter");
    add_action("out","out");
    add_action("view","view");
}

int view()
{
    name = capitalize(this_player()->query_name());
    if(environment(this_player()) != this_object()) return 0;
    write("Through the windows you see " +
      (string)environment(this_object())->long());
    tell_room(environment(this_player()),this_player()->query_cap_name()+
      " peers out the windows.\n",TP);

    tell_room(environment(this_object()),name +
      " peers at you through the windows of the airplane.\n");
    return 1;
}

int out()
{

    name = capitalize(this_player()->query_name());
    if(environment(this_player()) != this_object()) return 0;
    write("You exit the airplane.\n");
    tell_room(environment(this_object()),name +
      " emerges from the airplane.\n");
    tell_room(environment(this_player()),this_player()->query_cap_name()+
      " exits the airplane.\n",TP);
    this_player()->move(environment(this_object()));
    return 1;
}
int enter(string str)
{
    name = capitalize(this_player()->query_name());
    if(str == "airplane" || str == "plane") {
        write("You enter the airplane.\n");
        this_player()->move(this_object());
        tell_room(environment(this_player()),this_player()->query_cap_name()+
          " enters the airplane.\n",TP);

        tell_room(environment(this_object()),name +
          " enters the airplane.\n");
        write("Through the windows you see " +
          environment(this_object())->long());
        return 1;
    }
    else {
        return 0;
    }
}
int go(string str)
{
    string *dirs;
    int t;
    dirs = environment(this_object())->query_dest_dir();

    for (t=0; t<sizeof(dirs); t+=2)
    {
        if (dirs[t]==str)
        {
            tell_room(environment(this_object()),"There is a great roar "+
              "and rush of air as the airplane flies "+str+".\n");
            move(dirs[t+1]);
            t=sizeof(dirs);
            tell_room(environment(this_object()),"An airplane flies in " +
              "with a great roar!\n");
        }
    }

    write("You man the controls and fly the plane "+ str +".\n");
    tell_room(environment(this_player()),this_player()->query_cap_name()+
      "mans the controls and flies " + str + ".\n",TP);
    write ("Through the windows you see " + 
      (string)environment(this_object())->long());
    tell_room(environment(this_player()),"Through the windows you see "+
      (string)environment(this_object())->short(),TP);
    return 1;
}
