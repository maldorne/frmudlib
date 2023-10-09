/*
 * update command
 * Date         Creator         Comment
 * ------------ --------------- ----------------------------------
 * 06 Jun 97    Baldrick        Nicked it and hacked it so it works on FR. 
 *                              Also removec all this_player()
 * 30 Apr 96    Moredhel        Made external, from /global/wiz_file_comm
 * 30 Apr 96    Agamemnon       Added recursive updates, in the form
 *                              "update <file> to <file>"
 * 03 May 96    Parallax        Fix to avoid sscanf errors (!str)
 * 12 May 96    Agamemnon       Added "update all to <file>" - L+ restrict
 */
#include <standard.h>
#include <cmd.h>
/*
#include <users.h>
 */
inherit CMD_BASE;

int recursive_update( string foo, string bar, object me );
int update_all_to( string foo, object me );

void setup(){
    position = 1;
}

static int do_update(object *ov, object me) {
    string pname, dummy;
    int i, j;
    object *invent, rsv, env, dup, loaded;

    "room/void"->bingle_bingle();

    rsv = find_object("room/void");  /* RSV = Room Slash Void */
    if (!rsv) { /* Die in horror */
	notify_fail("The void is lost!\n");
	return 0;
    }

    for (i = 0; i < sizeof(ov); i++) {
	if (!ov[i]) continue;
	env = environment(ov[i]);
	invent = all_inventory(ov[i]);
	/*
	// Taniwha, so rooms stop refilling, buggers bags tho :)

		for (j = 0; j < sizeof(invent); j++)
		    invent[j]->move(rsv);
	*/
	for (j = 0; j < sizeof(invent); j++)
	{
	    if(interactive(ov[i]) || interactive(invent[j]) )
		invent[j]->move(rsv);
	    else invent[j]->dest_me(); // Taniwha 1995, so rooms stop filling
	}

	pname = file_name(ov[i]);
	if (sscanf(pname, "%s#%d", pname, dummy) != 2) { /* a room ? */
	    ov[i] -> dest_me();
	    if (ov[i]) ov[i]->dwep();
	    if (ov[i]) destruct(ov[i]);
	    file_size("/secure/master");
	    if (!ov[i])
		ov[i] = find_object(pname);
	    call_other(pname, "??");
	    ov[i] = find_object(pname);
	} else {
	    loaded = find_object(pname);
	    if (loaded) loaded->dest_me();
	    if (loaded) loaded->dwep();
	    if (loaded) destruct(loaded);

	    dup = clone_object(pname);
	    if (dup && ov[i]) {
		ov[i] -> dest_me();
		if (ov[i]) ov[i]->dwep();
		if (ov[i]) destruct(ov[i]);
		ov[i] = dup;
	    }
	}

	if (!ov[i]) {
	    write("I seem to have lost your object.\n");
	    return 1;
	}

	for (j = 0; j < sizeof(invent); j++)
	    if (invent[j]) invent[j]->move(ov[i]);

	if (env) ov[i]->move(env);
	write("Updated " + me->desc_f_object(ov[i]) + ".\n");
	/*
	      event( CREINFO, "inform", 
		me->query_cap_name()+" updated "+
		me->desc_f_object(ov[i])+".", "update" );
	 */
    }
    return 1;
} /* do_update() */

static int cmd(string str, object me) {
    string tring, *filenames, err;
    string foo, bar;
    object ob, *val, *obs;
    int loop;

    if(!me) return 1;

    if(!str) { }
    else if ( sscanf( str, "%s to %s", foo, bar ) == 2 ){
	if ( foo == "all" &&
	  "/secure/master"->query_lord( geteuid( me ) ) ){
	    return update_all_to( bar, me );
	}
	return recursive_update( foo, bar, me );
    }


    tring = str;
    if (!str || str == "here") {
	str = file_name(environment(me));
	sscanf(str, "%s#%d", str, loop);
	filenames = ({ "/" + str });
	str = "here";
    }
    else {
	filenames = me->get_cfiles(str);
    }
    if(sizeof(filenames) == 0) {
	val = me->wiz_present(tring, me);
	if(!sizeof(val)) {
	    notify_fail("No matching objects/filenames\n");
	    return 0;
	}
	return do_update(val, me);
    }
    obs = ({ });
    for(loop = 0; loop < sizeof(filenames); loop++) {

	str = filenames[loop];
	ob = find_object(str);
	if (!ob) {
	    if(file_size(str) >= 0) {
		if (!(err = catch(str->bing_with_me())))
		    write("Loaded " + str + "\n");
		else
		    write("Failed to load "+str+", error: "+err+"\n");
	    }
	    else {
		val = me->wiz_present(tring, me);
		obs += val;
	    }
	}
	else {
	    obs += ({ ob });
	}
    }
    if (!obs)
    { 
	notify_fail("No such object.\n");
	return 0;
    }
    else
	return do_update(obs, me);
} /* update() */


// Everything below here added by Agamemnon

object *create_update_list( object from, object to ){
    object *ul, ob;
    string *obs, to_name;
    int i;

    if ( from == to ) return ({ from });
    obs = deep_inherit_list( from );
    to_name = base_name(to);
    to_name = to_name[1..strlen(to_name)-1]+".c";
    if ( member_array( to_name, obs ) < 0 ) return 0;

    obs = inherit_list( from );
    for ( i=0; i<sizeof( obs ); i++ ){
	ob = find_object( obs[i] );
	if ( ul = create_update_list( ob, to ) ) return ul + ({ from });
    }
    return 0;
} 

object xfind( string arg, object me ) {
    string tmp;
    object ob;


    if ( ob=find_object( arg ) ) return ob;
    tmp = me->query_current_path() + "/" + arg;
    if ( ob=find_object( tmp ) ) return ob;
    if ( ob=present( arg, me ) ) return ob;
    if ( ob=present( arg, environment(me)) ) return ob;
    return 0;
}

static int recursive_update( string from, string to, object me ){
    object from_ob, to_ob, *obs;
    int code;

    if ( !(from_ob = xfind( from, me ) ) ){
	return notify_fail( "Error: "+from+" not loaded.\n" );
    }


    from = base_name( from_ob );
    if ( !(to_ob = xfind( to, me ) ) ){
	return notify_fail( "Error: "+to+" not loaded.\n" );
    }

    to = base_name( to_ob );
    if ( member_array( to[1..strlen(to)],
	deep_inherit_list( from_ob ) ) < 0 ){
	object ob;
	string tmp;

	if ( member_array( from[1..strlen(from)],
	    deep_inherit_list( to_ob ) ) >= 0 ){
	    // Swap 'em...
	    ob=from_ob;from_ob=to_ob;to_ob=ob;
	    tmp=from;from=to;to=tmp;
	} 
    }

    obs =  create_update_list( from_ob, to_ob );
    code = do_update( obs, me );

    // Check the return code...
    switch( code ) {
    case 1:
	tell_object( me, "Updated "+from+" to "+to+".\n" );
	return 1;
    case 0:
	tell_object( me, "Couldn't find "+to+" in "+from+"!\n" );
	return 1;
    case -1:
	tell_object( me, "Error in updating "+from+" to "+to+"!\n" );
	return 1;
    }
}

int update_all_to( string foo, object me ){
    object b_ob, to_ob, *all_obs, *inh_list, *done;
    string fname,bname;
    int i;

    if ( !(to_ob = xfind( foo, me ) ) ){
	tell_object( me, "Error: "+foo+" is not loaded!\n" );
	return 1;
    }
    if ( !do_update( ({ to_ob }), me ) ){
	tell_object( me, "Error updating all to "+foo+"\n" );
	return 1;
    }

    to_ob = xfind( foo , me );

    all_obs = objects();
    done = ({ to_ob, 0 });
    all_obs -= done;

    i = 0;
    while( i < sizeof( all_obs ) ){
	if ( !all_obs[i] ) i++;
	if ( interactive( all_obs[i] ) ) i++;  // Don't update interactives...
	inh_list = create_update_list( all_obs[i], to_ob );
	if ( !inh_list ) {
	    i++;
	    continue;
	}
	fname = file_name( all_obs[i] );
	bname = base_name( all_obs[i] );
	b_ob = find_object( bname );
	if ( b_ob != all_obs[i] ) 
	    inh_list = inh_list - ({ all_obs[i] }) + ({ b_ob, all_obs[i] });

	inh_list -= done;
	do_update( inh_list, me );
	inh_list = create_update_list( find_object( bname ), to_ob ) - done;
	done += inh_list;
	all_obs -= done;
    }
    return 1;
}
