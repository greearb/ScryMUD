// $Id: grrmud.h,v 1.2 1999/06/05 23:29:14 greear Exp $
// $Revision: 1.2 $  $Author: greear $ $Date: 1999/06/05 23:29:14 $

//
//ScryMUD Server Code
//Copyright (C) 1998  Ben Greear
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//

#ifndef GRRMUD_INCLUDE_H
#define GRRMUD_INCLUDE_H

void           run_the_game(int port);
void           game_loop(int s);
int            init_socket(int port);
int            new_connection(int s);
int            new_descriptor(int s);
int            get_input(critter& pc);
void           close_sockets(int s);
int            close_socket(critter& pc, short suicide);
int            log_off(critter& pc);
timeval        timediff(timeval *a, timeval *b);
void           nonblock(int s);
int            write_output(critter& pc);
int            get_new_maxdesc(int s);

#endif
