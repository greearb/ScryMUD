// $Id$
// $Revision: 1.3 $  $Author$ $Date$

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

/*  Socials.cc: These are simplistic commands.  They usually only
    involve text messages, but some can provoke responses from mobs,
    not to mention other players. */

#include "misc.h"
#include "misc2.h"
#include "commands.h" 
#include "command2.h"    
#include <string2.h>
#include "classes.h"
#include "battle.h"
#include "spec_prc.h"
#include <PtrArray.h>

extern PtrArray<room> room_list;
//extern List<room*> embattled_rooms; 
//extern List<room*> affected_rooms;


/* by shamu */
void lag(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You complain about the lag, your hitpoints, your mana...\n",
              pc);
         Sprintf(buf, "complains on and on about the lag.\n",
                 get_hisself_herself(pc));
         emote(buf, pc, ROOM, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You complain to %S about the lag.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S whines to you about the lag.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S whines about the lag to %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You complain about the lag, your hitpoints, your mana...\n", pc);
      emote("complains on and on about the lag.", pc, ROOM, TRUE);
   }//else
}//lag




void rofl(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You roll on the floor, laughing hysterically!\n", pc);
         Sprintf(buf, "rolls on the floor, laughing hysterically!\n",
                 get_hisself_herself(pc));
         emote(buf, pc, ROOM, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You roll on the floor laughing your guts out at %S!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S rolls on the floor laughing at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S rolls on the floor laughing at %S!\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You roll on the floor laughing hysterically!\n", pc);
      emote("rolls on the floor laughing hysterically!", pc, ROOM, TRUE);
   }//else
}//rofl


/* shamu */
void insane(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("Your sanity is slowly slipping away...\n", pc);
         Sprintf(buf,
"\b's sanity is slowly slipping away, right before your very eyes!\n", 
                get_hisself_herself(pc));
         emote(buf, pc, ROOM, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You whisper to %S to call the men in white coats.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S tells you to call the men in white coats.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, 
                 "%S whispers to %S to call the men in white coats.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("Your sanity is slowly slipping away...\n", pc);
      emote(
        "\b's sanity is slowly slipping away, right before your very eyes!",
        pc, ROOM, TRUE);
   }//else
}//insane



/* Social By Acrwyn */
void flex(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You flex impressively for yourself.\n", pc);
         Sprintf(buf, "flexes impressively for %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You flex impressively for %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S flexes impressively for you. Oooo!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S flexes impressively for %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
     show("You flex your muscles impressively, watching them ripple.\n", pc);
     emote("flexes impressively.", pc, rm, TRUE);
   }//else
}//flex

/* Social by Acrwyn */
void hug(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You hug yourself in a reassuring way.\n", pc);
         Sprintf(buf, "hugs %s warmly.\n",
get_hisself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You give %S a big, warm hug.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S gives you a great, big, warm hug.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S hugs %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You hug the air.  Lonely?\n", pc);
      emote("hugs the air, looking lonely.", pc, rm, TRUE);
   }//else
}//hug

void poke(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
                            rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You poke yourself ruefully.\n", pc);
         Sprintf(buf, "pokes %s in the eye.\n",
                 get_hisself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You poke %S in the ribs.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S pokes you in the ribs.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S pokes %S in the ribs.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You poke around.\n", pc);
      emote("pokes around.", pc, rm, TRUE);
   }//else
}//poke

void hop(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
                            rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS != POS_STAND)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You hop in little circles around yourself.\n", pc);
         Sprintf(buf, "hops gleefully in little circles around %s.\n",
                 get_hisself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You gleefully hop in little circles around %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S gleefully hops in little circles around you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S gleefully hops around %S in little circles!\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You gleefully hop in little circles!\n", pc);
      emote("gleefully hops in little circles!", pc, rm, TRUE);
   }//else
}//hop


/* Social By Acrwyn */
void smile(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You smile happily to yourself.\n", pc);
         Sprintf(buf, "smiles happily at %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You smile happily at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S smiles happily at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S smiles happily at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You smile happily.\n", pc);
      emote("smiles happily.", pc, rm, TRUE);
   }//else
}//smile

/* Social By Acrwyn */
void growl(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You growl quietly to yourself.\n", pc);
         Sprintf(buf, "growls menacingly at %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You growl loudly at %S.  Grrr!!!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S growls menacingly at you!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S growls loudly at %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You growl menacingly like the beast you really are. Grrr!!!\n",
           pc);
      emote("growls menacingly.  Grrr!", pc, rm, TRUE);
   }//else
}//growl

/* Social By Acrwyn*/
void dance(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You dance romantically with yourself.\n", pc);
         Sprintf(buf, "dances about merrily with %s.\n",
get_hisself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You dance about with %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S dances about merrily with you.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S dances about merrily with %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You dance around in happy circles!\n", pc);
      emote("dances about in happy circles!", pc, rm, TRUE);
   }//else
}//dance

/* Social By Acrwyn */
void thank(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You thank yourself.\n", pc); 
         Sprintf(buf, "thanks %s profusely.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You thank %S profusely.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S thanks you profusely!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S thanks %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You thank your lucky stars.\n", pc);
      emote("thanks the gods, eyes turned heavenward.", pc, rm, TRUE);
   }//else
}//thank

/* Social By Acrwyn */
void curtsey(int i_th, const String* vict, critter& pc, room& rm)
{
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You curtsey to yourself and nearly fall over.\n",
pc); 
         Sprintf(buf, "curtseys gracefully to %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You curtsey gracefully to %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S curtseys gracefully to you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S curtseys gracefully before %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You curtsey low and gracefully.\n", pc);
      emote("curtseys low and gracefully.", pc, rm, TRUE);
   }//else
}//curtsey

/* ANOTHER Social by Acrwyn */
void laugh(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You laugh at yourself.  Silly you!\n", pc); 
         Sprintf(buf, "laughs loudly at %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You laugh loudly at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S laughs loudly, pointing at you.\n",     
            name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S laughs loudly at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You laugh loudly.  Hahahahaha!\n", pc);
      emote("laughs loudly.", pc, rm, TRUE);
   }//else
}//laugh

/* Social By Acrwyn */
void chuckle(int i_th, const String* vict, critter& pc, room& rm)
{
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You chuckle to yourself.\n", pc);
         Sprintf(buf, "chuckles to %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You chuckle at %S.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S chuckles at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S chuckles %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You chuckle.\n", pc);
      emote("chuckles.", pc, rm, TRUE);
   }//else
}//chuckle

/* Social By Acrwyn */
void ack(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You ACK and spit at yourself.\n", pc); 
         Sprintf(buf, "ACKS and spits on %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You ACK and spit at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S ACKS and spits at you!  ACK!  PTHHH!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S ACKS and spits at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("you ACK and spit.  ACK!  PTHHH!\n", pc);
      emote("ACKS and spits.  ACK!  PTHHH!", pc, rm, TRUE);
   }//else
}//ack

/* Social By Acrwyn */
void bounce(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You bounce around yourself and get dizzy.\n", pc);
         Sprintf(buf, "bounces excitedly around %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You bounce excitedly around %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S bounces around you excitedly\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S bounces excitedly around %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {     
      show("You bounce about excitedly!  BOIIING!\n", pc);
      emote("bounces about excitedly!", pc, rm, TRUE);
   }//else
}//bounce

/* Social By "The Love Monkey" */
void burp(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You burp loudly at yourself.  PHEW!.\n", pc);
         Sprintf(buf, "burps loudly at %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You burp loudly at %S, sending them reeling.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S sends you reeling with a horrific burp!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S burps loudly at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You burp happily.  BUUUUURP!\n", pc);
      emote("burps loud and happily.", pc, rm, TRUE);
   }//else
}//burp

/* Social By Acrwyn */
void snicker(int i_th, const String* vict, critter& pc, room& rm)
{
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You snicker knowingly to yourself.\n", pc);
         Sprintf(buf, "snickers to %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You snicker quietly at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S snickers quietly at you.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S snickers quietly at %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You snicker quietly.\n", pc);
      emote("snickers quietly.", pc, rm, TRUE);
   }//else
}//snicker

/* Social By Acrwyn */
void scream(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You scream in frustration at yourself.\n", pc);
         Sprintf(buf, "screams deafeningly at %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You get close and scream loudly at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S screams loudly into your ears. Ouch!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S screams deafeningly at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You scream like you've never screamed before!!!\n",
pc);
      emote("screams deafeningly.  AAAAAH!", pc, rm, TRUE);
   }//else
}//scream

/* Socials By Acrwyn */
void panic(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You panic at the thought of yourself.\n", pc);
         emote("panics and runs screaming in erratic circles.\n",
               pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, 
                "You panic and run in circles around %S while screaming.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, 
                "%S panics and runs in circles around you, screaming.\n",  
                name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, 
                 "%S panics and runs screaming in circles around %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You panic, running about in circles and screaming.\n",
pc);
      emote("panics, running about in circles and screaming", pc,
rm, TRUE);
   }//else
}//panic

/* Social By Acrwyn */
void grunt(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You grunt to yourself.\n", pc);
         Sprintf(buf, "grunts to %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You grunt loudly at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S grunts loudly at you!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S grunts at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You grunt.\n", pc);
      emote("grunts loudly.", pc, rm, TRUE);
   }//else
}//grunt

/* Social By Acrwyn */
void ponder(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         if (strcasecmp(*(Top(pc.names)), "levander") == 0) {
            show("You ARE a Ponder!!.\n", pc);
         }//if
         else {
            show("You ponder your own existence.\n", pc);
         }//else
         Sprintf(buf, "ponders %s.\n", get_hisself_herself(pc));  
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You ponder %S thoughtfully.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S ponders you thoughtfully.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S ponders %S thoughtfully.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You ponder, thinking deep thoughts.\n", pc);
      emote("ponders, eyebrows furrowed in deep thought.", pc, rm,
TRUE);
   }//else
}//ponder

/* Social By Acrwyn */
void scratch(int i_th, const String* vict, critter& pc, room& rm)
{
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You scratch yourself thoughtfully.\n", pc);
         Sprintf(buf, "scratches %s thoughtfully.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You scratch %S thoughtfully.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S scratches your back.  Ahhh.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S scratches %S behind the ears.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You scratch, thoughtfully.\n", pc);
      emote("thoughtfully scratches.", pc, rm, TRUE);
   }//else
}//scratch

/* Social By Acrwyn */
void lick(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You lick yourself. Mmmm.\n", pc);
         Sprintf(buf, "licks %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You lick %S.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S licks you!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S licks %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You lick your lips.\n", pc);
      Sprintf(buf, "licks %s lips.", get_his_her(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//lick

/* Social By Acrwyn */
void cackle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT,
rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You cackle gleefully at yourself.\n", pc); 
         Sprintf(buf, "cackles gleefully at %s.\n",
get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You cackle gleefully at %S.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S cackles at you gleefully.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S cackles at %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You cackle gleefully!\n", pc);
      emote("cackles gleefully!", pc, rm, TRUE);
   }//else
}//cackle


/* by Acrwyn */
void weep(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You weep big tears on yourself.\n", pc); //change
         Sprintf(buf, "weeps at %s.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "Bawling, you weep huge tears on %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S weeps huge tears on you!!\n", //change
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S weeps on %S.\n", //change
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You weep big mournful tears.  Poor you.\n", pc);
      emote("weeps loudly, drenching everything with tears!!", pc,
            rm, TRUE);
   }//else
}//weep

/* by Acrwyn */
void disco(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_STAND)
         show("You are not in a position to disco.\n", pc);
      else if (crit_ptr == &pc) {
         show("You disco with yourself across the room.\n", pc);
         Sprintf(buf, "disco dances with %s.\n",get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You disco wildly with %S.\n", //change
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S wildly dances the disco with you!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S does a funky disco dance with %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You flail your arms and twirl ... it's the disco man!\n", pc);
      emote("\b, collar flipped up, does the disco!!", pc, rm, TRUE);
   }//else
}//disco


void puke(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You puke your shoes.\n", pc);
         Sprintf(buf, "pukes on %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You puke your guts out all over %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S spews chunks all over you!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S pukes all over %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You spew chunks...did THAT come out of your stomach???\n", pc);
      emote("turns green and blows chunks!!", pc, rm, TRUE);
   }//else
}//puke



void cheer(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You cheer yourself..  What a hoser!.\n", pc);
         Sprintf(buf, "cheers %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You give a big cheer for %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S cheers you on!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S cheers %S to victory and glory!!\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You give a good try at starting the wave....nothing happens!\n", 
                pc);
      emote("throws up the hands and gives a big cheer!!", pc, rm, 
                TRUE);
   }//else
}//cheer



void nod(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You nod to yourself.\n", pc);
         Sprintf(buf, "nods to %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You nod your head in agreement with %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S nods in agreement with you!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S nods in agreement with %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You nod your head in agreement.\n", pc);
      emote("nods in agreement!!", pc, rm, TRUE);
   }//else
}//nod

void grin(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You grin at yourself.\n", pc);
         Sprintf(buf, "grins at %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You grin mischieviously at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S grins mischieviously at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S grins at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You grin widely!\n", pc);
      emote("grins at the thought.", pc, rm, TRUE);
   }//else
}//grin


void wave(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You wave goodbye to yourself.\n", pc);
         Sprintf(buf, "waves goodbye to  %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You wave goodbye to %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S waves goodbye to you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S waves goodbye to %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You wave merrily!\n", pc);
      emote("waves in your general direction.", pc, rm, TRUE);
   }//else
}//wave


void bow(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You attempt to bow to yourself.\n", pc);
         Sprintf(buf, "tries to bow to %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You bow to %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S bows before you!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S bows to %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
         if (crit_ptr->mob && crit_ptr->mob->proc_data &&
             crit_ptr->mob->proc_data->bow_proc) {
           ptr = crit_ptr;
           if (crit_ptr->isMob()) {
             ptr = mob_to_smob(*crit_ptr, rm.getCurRoomNum());
           }//if
           do_domob_bow_proc(*ptr, pc);
         }//if
      }//else
   }//if a victim
   else {      
      show("You bow.\n", pc);
      emote("bows.", pc, rm, TRUE);
   }//else
}//bow


void curse(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You curse to yourself.\n", pc);
         Sprintf(buf, "curses at %s folly.\n", get_his_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You call down the wrath of ancient gods upon %S!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S invokes ancient names to speed your doom!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, 
            "%S calls forth the wrath of ancient gods to hasten %S's doom.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
         if (crit_ptr->mob && crit_ptr->mob->proc_data &&
             crit_ptr->mob->proc_data->curse_proc) {
           ptr = crit_ptr;
           if (crit_ptr->isMob()) {
             ptr = mob_to_smob(*crit_ptr, rm.getCurRoomNum());
           }//if
           do_domob_curse_proc(*ptr, pc);
         }//if
      }//else
   }//if a victim
   else {      
      show("You curse darkly at the world.", pc);
      Sprintf(buf, "curses the situation %s finds %s in.", get_he_she(pc),
              get_hisself_herself(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//curse


void disdain(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.CRITTERS);
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
            have_crit_named(rm.CRITTERS, i_th, vict, pc.SEE_BIT, rm);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You wallow in self pity.\n", pc);
         Sprintf(buf, "turns a glowering eye upon %s.\n",
                 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You glower at %S with great disdain.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);

         Sprintf(buf, "%S glares disdainfully at you!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while (ptr = cll.next()) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S glares disdainfully at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You glower with disdain at the world.\n", pc);
      emote("glowers disdainfully.", pc, rm, TRUE);
   }//else
}//disdain











