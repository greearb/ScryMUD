// $Id: social2.h,v 1.8 2002/09/22 03:30:54 eroper Exp $
// $Revision: 1.8 $  $Author: eroper $ $Date: 2002/09/22 03:30:54 $

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

/* social2.h */

#ifndef SOCIAL2_INCLUDE
#define SOCIAL2_INCLUDE

#include <string2.h>
#include "classes.h"
#include "room.h"
#include "critter.h"
#include "object.h"
#include "door.h"



void tango(int i_th, const String* vict, critter& pc, room& rm);
void shake(int i_th, const String* vict, critter& pc, room& rm);
void groan(int i_th, const String* vict, critter& pc, room& rm);
void beckon(int i_th, const String* vict, critter& pc, room& rm);
void sob(int i_th, const String* vict, critter& pc, room& rm);
void smirk(int i_th, const String* vict, critter& pc, room& rm);
void splash(int i_th, const String* vict, critter& pc, room& rm);
void itch(int i_th, const String* vict, critter& pc, room& rm);
void wiggle(int i_th, const String* vict, critter& pc, room& rm);
void agree(int i_th, const String* vict, critter& pc, room& rm);
void blush(int i_th, const String* vict, critter& pc, room& rm);
void pat(int i_th, const String* vict, critter& pc, room& rm);
void twiddle(int i_th, const String* vict, critter& pc, room& rm);
void clap(int i_th, const String* vict, critter& pc, room& rm);
void hum(int i_th, const String* vict, critter& pc, room& rm);
void hmm(int i_th, const String* vict, critter& pc, room& rm);
void grumble(int i_th, const String* vict, critter& pc, room& rm);
void listen(int i_th, const String* vict, critter& pc, room& rm);
void pinch(int i_th, const String* vict, critter& pc, room& rm);
void roll(int i_th, const String* vict, critter& pc, room& rm);
void strut(int i_th, const String* vict, critter& pc, room& rm);
void whistle(int i_th, const String* vict, critter& pc, room& rm);
void slap(int i_th, const String* vict, critter& pc, room& rm);
void muahaha(int i_th, const String* vict, critter& pc, room& rm);
void hehe(int i_th, const String* vict, critter& pc, room& rm);
void silly(int i_th, const String* vict, critter& pc, room& rm);
void scold(int i_th, const String* vict, critter& pc, room& rm);
void blush(int i_th, const String* vict, critter& pc, room& rm);
void kiss(int i_th, const String* vict, critter& pc, room& rm);
void afk(int i_th, const String* vict, critter& pc, room& rm);
void giggle(int i_th, const String* vict, critter& pc, room& rm);
void shrug(int i_th, const String* vict, critter& pc, room& rm);
void wibble(int i_th, const String* vict, critter& pc, room& rm);
void yawn(int i_th, const String* vict, critter& pc, room& rm);
void bonk(int i_th, const String* vict, critter& pc, room& rm);
void pft(int i_th, const String* vict, critter& pc, room& rm);
void peck(int i_th, const String* vict, critter& pc, room& rm);
void trout(int i_th, const String* vict, critter& pc, room& rm);
void tremble(int i_th, const String* vict, critter& pc, room& rm);
void goo(int i_th, const String* vict, critter& pc, room& rm);
void smooch(int i_th, const String* vict, critter& pc, room& rm);
void swoon(int i_th, const String* vict, critter& pc, room& rm);
void twirl(int i_th, const String* vict, critter& pc, room& rm);
void tickle(int i_th, const String* vict, critter& pc, room& rm);
void battlecry(int i_th, const String* vict, critter& pc, room& rm);
void loser(int i_th, const String* vict, critter& pc, room& rm);
void nestle(int i_th, const String* vict, critter& pc, room& rm);
void sweatdrop(int i_th, const String* vict, critter& pc, room& rm);
void shudder(int i_th, const String* vict, critter& pc, room& rm);
void cower(int i_th, const String* vict, critter& pc, room& rm);
void climb(int i_th, const String* vict, critter& pc, room& rm);
void shock(int i_th, const String* vict, critter& pc, room& rm);
void wiggletoes(int i_th, const String* vict, critter& pc, room& rm);
void stare(int i_th, const String* vict, critter& pc, room& rm);
void blink(int i_th, const String* vict, critter& pc, room& rm);
void eyebrow(int i_th, const String* vict, critter& pc, room& rm);
void whimper(int i_th, const String* vict, critter& pc, room& rm);
void duck(int i_th, const String* vict, critter& pc, room& rm);
void pet(int i_th, const String* vict, critter& pc, room& rm);

#endif
