// $Id: login.h,v 1.3 1999/06/05 23:29:14 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 1999/06/05 23:29:14 $

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

/************************  login.h  ***************************/
/*  Contains all the prototypes for all commands found in command*.cc */


#ifndef LoginInclude
#define LoginInclude

#include "const.h"
#include "classes.h"

int start_login(critter& pc);
int do_login(critter& pc);
int quit_do_login_new(critter& pc);
int quit_do_login_old(critter& pc);
int is_valid_name(const String& name);

#endif //include
