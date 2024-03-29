<p align="center">
  <img width="150px" alt="Final Realms logo" src="/logo.gif">
</p>

> [!IMPORTANT]  
> The `master` branch is empty, the code for different versions is located in other branches.

# `frmudlib` MUD library

This was the old mudlib from the game **Final Realms** during the nineties. It includes only the **mudlib** without the game content (the _domains_). It runs under a [MudOS MUD driver](https://github.com/maldorne/mudos).

This was the license:

```
----  The FR mudlib version 3.4 ----
(c)opyright Final Realms 1992-1998.

Licence.

You are granted license to use and modify all code in this distribution as
long as you mail mudadm@fr.imaginary.com with a notice that you are using it.


Have fun with it.
For the FR-MUD development team.

  Baldrick, God.
Baldrick@fr.imaginary.com
```

## Current game

This repository have no relationship with the current game, is just a backup of old versions of the software. The current game can be reached at:

- `telnet fr.hyssing.net 4001`

There is an extensive wiki about the game in [https://wiki.solbu.org/display/MUD](https://wiki.solbu.org/display/MUD).

You can find other repositories and backups in:

- https://github.com/silbago/Final-Realms
- https://github.com/quixadhal/fr

## Branches

On this repository, you could find different branches for the following versions of `frmudlib` (from newer to older):

- ~`v4.0b`~
- `v3.5b`
- ~`v3.4`~
- ~`v3.2b3`~

The `master` branch is empty, try any other branch to see different versions. *Striketrough versions are not yet uploaded, we are working on it*.

## Current status of each branch

- `v4.0b`
  - _Not yet uploaded._
- `v3.5b`
  - Working with Docker, using the **MudOS driver v21.7** image (available in the [Github Container Registry](https://github.com/maldorne/frmudlib/pkgs/container/frmudlib)).
- `v3.4`
  - _Not yet uploaded._
- `v3.2b3`
  - _Not yet uploaded._

## How to use this version with Docker

Change to a different branch to see the specific instructions to build and run the containers.
