// $Id$
// $Revision$  $Author$ $Date$

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
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

/* socials.h */

#ifndef SOCIALS_INCLUDE
#define SOCIALS_INCLUDE

#include "const.h"
#include "classes.h"
#include <string2.h>

/* by Rydia */
void whip(int i_th, const String* vict, critter& pc, room& rm);

/* by Shamu */
void mutter(int i_th, const String* vict, critter& pc, room& rm);
void cough(int i_th, const String* vict, critter& pc, room& rm);
void point(int i_th, const String* vict, critter& pc, room& rm);
void snap(int i_th, const String* vict, critter& pc, room& rm);
void whine(int i_th, const String* vict, critter& pc, room& rm);
void bah(int i_th, const String* vict, critter& pc, room& rm);
void punt(int i_th, const String* vict, critter& pc, room& rm);
void yuck(int i_th, const String* vict, critter& pc, room& rm);
void announce(int i_th, const String* vict, critter& pc, room& rm);
void greet(int i_th, const String* vict, critter& pc, room& rm);
void doze(int i_th, const String* vict, critter& pc, room& rm);
void megawibble(int i_th, const String* vict, critter& pc, room& rm);


void pout(int i_th, const String* vict, critter& pc, room& rm);
void bite(int i_th, const String* vict, critter& pc, room& rm);
void nibble(int i_th, const String* vict, critter& pc, room& rm);
void chomp(int i_th, const String* vict, critter& pc, room& rm);
void social_jump(int i_th, const String* vict, critter& pc, room& rm);
void lag(int i_th, const String* vict, critter& pc, room& rm);
void sigh(int i_th, const String* vict, critter& pc, room& rm);
void wink(int i_th, const String* vict, critter& pc, room& rm);
void poke(int i_th, const String* vict, critter& pc, room& rm);
void hop(int i_th, const String* vict, critter& pc, room& rm);
void rofl(int i_th, const String* vict, critter& pc, room& rm);
void insane(int i_th, const String* vict, critter& pc, room& rm);
void flex(int i_th, const String* vict, critter& pc, room& rm);
void hug(int i_th, const String* vict, critter& pc, room& rm);
void smile(int i_th, const String* vict, critter& pc, room& rm);
void growl(int i_th, const String* vict, critter& pc, room& rm);
void dance(int i_th, const String* vict, critter& pc, room& rm);
void thank(int i_th, const String* vict, critter& pc, room& rm);
void curtsey(int i_th, const String* vict, critter& pc, room& rm);
void laugh(int i_th, const String* vict, critter& pc, room& rm);
void chuckle(int i_th, const String* vict, critter& pc, room& rm);
void ack(int i_th, const String* vict, critter& pc, room& rm);
void bounce(int i_th, const String* vict, critter& pc, room& rm);
void burp(int i_th, const String* vict, critter& pc, room& rm);
void snicker(int i_th, const String* vict, critter& pc, room& rm);
void scream(int i_th, const String* vict, critter& pc, room& rm);
void panic(int i_th, const String* vict, critter& pc, room& rm);
void grunt(int i_th, const String* vict, critter& pc, room& rm);
void ponder(int i_th, const String* vict, critter& pc, room& rm);
void scratch(int i_th, const String* vict, critter& pc, room& rm);
void lick(int i_th, const String* vict, critter& pc, room& rm);
void cackle(int i_th, const String* vict, critter& pc, room& rm);
void weep(int i_th, const String* vict, critter& pc, room& rm);
void disco(int i_th, const String* vict, critter& pc, room& rm);
void puke(int i_th, const String* vict, critter& pc, room& rm);
void cheer(int i_th, const String* vict, critter& pc, room& rm);
void nod(int i_th, const String* vict, critter& pc, room& rm);
void nog(int i_th, const String* vict, critter& pc, room& rm);
void grin(int i_th, const String* vict, critter& pc, room& rm);
void wave(int i_th, const String* vict, critter& pc, room& rm);
void bow(int i_th, const String* vict, critter& pc, room& rm);
void curse(int i_th, const String* vict, critter& pc, room& rm);
void disdain(int i_th, const String* vict, critter& pc, room& rm);
void thpppt(int i_th, const String* vict, critter& pc, room& rm);

#endif
