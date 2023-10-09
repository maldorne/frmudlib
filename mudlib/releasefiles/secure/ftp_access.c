// FTP access generator
// 31 may 93 Chrisy
// Modified for RoD Chrisy 15 June 1993

#include <standard.h>
#include "/secure/master.h"

#define OUTFILE "/secure/ftp_access.inf"
#define SAVEFILE "/secure/ftp_access"
#define MASTERFILE MASTER

// restored from master.o

mapping positions;
mapping permissions;

// Globals :)

mapping perms;
int make_perms_active;

// prototypes

void make_perms();

// functions

int query_prevent_shadow() { return 1; }

nomask void create()
{
  seteuid(getuid());

  if(file_length(MASTERFILE+".o"))
    restore_object(MASTERFILE,1);

  permissions = MASTER->query_permissions();

  perms = ([ ]);

  if(!positions) positions = ([]);
  if(!permissions) permissions = (["":([]) ]);

  make_perms();
}

nomask void dest_me()
{
  destruct(this_object());
}

nomask void save_me()
{
  string *nams, *paths;
  mapping acc;
  int sz, asz, i, j;

  save_object(SAVEFILE,1);

  rm(OUTFILE);
  write_file(OUTFILE, "# This file is generated by the "+
      "Mudlib.\n# You shouldn't change it.\n"+
      "# Created: "+ctime(time())+".\n");

  nams = keys(perms);
  sz = sizeof(nams);
  for(i=0; i<sz; i++)
  {
    acc = perms[nams[i]];
    paths = keys(acc);
    asz = sizeof(paths);

    for(j=0; j<asz; j++)
      write_file(OUTFILE, sprintf("%s %s %d\n", nams[i],
          paths[j], acc[paths[j]]));
  }

  SETEUID;
}

void make_perms()
{
  if(make_perms_active)
    return;
  call_out("do_make_perms", 0);
  make_perms_active = 1;
}

void do_make_perms()
{
  string *paths, *nams;
  mapping acc;
  int sz, nsz, i, j;

  make_perms_active = 0;
  paths = keys(permissions);
  sz = sizeof(paths);
  
  for(i=0; i<sz; i++)
  {
    acc = permissions[paths[i]];
  if(!mappingp(acc)) acc = ([ ]);
    nams = keys(acc);
    nsz = sizeof(nams);
    for(j=0; j<nsz; j++)
    {
      if(!perms[nams[j]])
        perms[nams[j]] = ([ ]);
      if(acc[nams[j]]&WRITE_MASK) acc[nams[j]] |= READ_MASK;
      perms[nams[j]][paths[i]] = acc[nams[j]];
    }
  }

  nams = keys(positions);
  nsz = sizeof(nams);
  for(i=0; i<nsz; i++)
  {
    if(!perms[nams[i]])
      perms[nams[i]] = ([ ]);
    switch(positions[nams[i]])
    {
    case LORD:
      perms[nams[i]]["/d"] = READ_MASK|WRITE_MASK;
      break;
    case HIGH_LORD:
      perms[nams[i]]["/"] = READ_MASK|WRITE_MASK;
      break;
    }
    perms[nams[i]]["/w/"+nams[i]] = READ_MASK|WRITE_MASK;
  }
  save_me();
}
