// $Id: social2.cc,v 1.4 1999/06/05 23:29:15 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 1999/06/05 23:29:15 $

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

/************************** social2.cc **********/

#include "misc.h"
#include "misc2.h"
#include "commands.h" 
#include "command2.h"    
#include <string2.h>
#include "classes.h"
#include "battle.h"
#include "spec_prc.h"
#include "social2.h"


/*Coded By GROCK*/
void smirk(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS == POS_SLEEP)
         show("You smirk in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You smirk to yourself.\n", pc); 
         Sprintf(buf, "smirks at %s", get_hisself_herself(pc)); 
	 emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You smirk at %S.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S smirks at you.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S smirks at %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You smirk.\n", pc);
      emote("smirks.", pc, rm, TRUE);
   }//else
}//smirk


/*Coded By GROCK*/
void beckon(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS == POS_SLEEP)
         show("You gesture in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You beckon to yourself.\n", pc); 
         Sprintf(buf, "beckons at %s", get_hisself_herself(pc)); 
	 emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You beckon %S to follow you.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S beckons you to follow %s.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_him_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S beckons %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You beckon folks to follow you.\n", pc);
      emote("beckons all to follow.", pc, rm, TRUE);
   }//else
}//beckon

/*Coded By GROCK*/
void sob(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS == POS_SLEEP)
         show("You cry in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You sob quietly to yourself.\n", pc); 
         Sprintf(buf, "sobs quietly to %s.", get_hisself_herself(pc)); 
	 emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You sob on %S's shoulder.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S sobs on your shoulder.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S sobs on %S's shoulder.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You sob quietly.\n", pc);
      emote("sobs quietly.", pc, rm, TRUE);
   }//else
}//sob


/*Coded By GROCK*/
void splash(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (!(rm.isSmallWater() || rm.isBigWater() ||
       rm.isSwamp())) {
     show("You go to splash, and only find a dry floor!\n", pc);
     return;
   }//if

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to splash anyone.\n", pc);
      else if (crit_ptr == &pc) {
         show("You splash yourself in the face!\n", pc); 
         Sprintf(buf, "splashes %s in the face!", get_hisself_herself(pc)); 
	 emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You splash %S in the face!\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S splashes you in the face!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S splashes %S in the face!\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You splash around in the water.\n", pc);
      emote("splashes around in the water.", pc, rm, TRUE);
   }//else
}//splash


/*Coded By GROCK*/
void pat(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You pat yourself on the back.\n", pc); 
         Sprintf(buf, "pats %s on the back.\n", get_hisself_herself(pc)); 
	 emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You pat %S on the back.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S pats you on the back.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S pats %S on the back.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You pat the ground.\n", pc);
      emote("pats the ground.", pc, rm, TRUE);
   }//else
}//pat



/*Coded By GROCK*/
void itch(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS == POS_SLEEP)
         show("You itch horribly in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You make yourself break out in hives!\n", pc); 
         Sprintf(buf, "makes %s break out in hives!",
		 get_hisself_herself(pc)); 
	 emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You make %S feel itchy all over.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S makes you feel itchy all over!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S tries to make %S itchy.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You itch all over...AAAHHHHH infuriating!\n", pc);
      emote("itches all over!", pc, rm, TRUE);
   }//else
}//itch



/*Coded By GROCK*/
void wiggle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You toss and turn restlessly.\n", pc);
      else if (crit_ptr == &pc) {
         show("You wiggle yourself.\n", pc);
         Sprintf(buf, "wiggles %s.\n", get_hisself_herself(pc)); 
	 emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You wiggle your bottom at %S.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S wiggles %s bottom at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
		 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S wiggles %s bottom at %S!\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
		 get_his_her(pc),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You wiggle your bottom.\n", pc);
      Sprintf(buf, "wiggles %s bottom.", get_his_her(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//wiggle




/*Coded By GROCK*/
void agree(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are quite agreeable asleep!\n", pc);
      else if (crit_ptr == &pc) {
         show("You agree with yourself...for a change!\n", pc); 
         Sprintf(buf, "agrees with %s.\n", get_hisself_herself(pc)); 
	 emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You agree with %S.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S agrees with you.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S agrees with %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You agree completely.\n", pc);
      emote("agrees all around.", pc, rm, TRUE);
   }//else
}//agree


/*Coded By Crazy*/
void blush(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You blush.\n", pc); 
         Sprintf(buf, "blushes at %s.\n", get_hisself_herself(pc)); 
      emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You blush at the sight of %S.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S blushes at the sight of you.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S blushes at the sight of %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You turn beet red!\n", pc);
      emote("turns bright red and blushes.", pc, rm, TRUE);
   }//else
}//blush


/* by crazy */
void kiss(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You kiss yourself.\n", pc); //change
         Sprintf(buf, "Kisses %s.\n", get_hisself_herself(pc)); 
//change
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You kiss %S.\n", //change
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S Kisses you long and passionatly.\n", //change
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S kisses %S passionately.\n", //change
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You kiss the air.  Was it fun??\n", pc);
      emote("blows kisses into the air!", pc, rm, TRUE);
   }//else
}//kiss


/* by Crazy */
void afk(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);

   show("You go afk.\n", pc);
   Sprintf(buf, "steps away from %s keyboard.", get_his_her(pc));
   emote(buf, pc, rm, TRUE); 
}//afk


/* Made by Shamu */
void giggle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You giggle uncontrollably!\n", pc);
         Sprintf(buf, "giggles crazily at %s!\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You giggle at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S giggles crazily at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S giggles crazily at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You giggle uncontrollably!\n", pc);
      emote("giggles crazily!", pc, rm, TRUE);
   }//else
}//giggle


/* Made by Shamu */
void shrug(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You shrug helplessly to yourself.\n", pc);
         Sprintf(buf, "shrugs helplessly to %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You shrug in response to %S's remark.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S shrugs helplessly at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S shrugs at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You shrug helplessly.\n", pc);
      emote("shrugs helplessly.", pc, rm, TRUE);
   }//else
}//shrug



/* Made by Shamu */
void wibble(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You turn your face inside out and WIBBLE!\n", pc);
      else if (crit_ptr == &pc) {
         show("You turn your face inside out and WIBBLE!\n", pc);
         Sprintf(buf, "WIBBLES! %s\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You turn your face inside out and WIBBLE at %S!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S WIBBLES at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, 
		 "%S turns %s face inside out and WIBBLES at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
		 get_him_her(pc),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You turn your face inside out and WIBBLE!\n", pc);
      emote("WIBBLES!", pc, rm, TRUE);
   }//else
}//wibble


/* Made by Shamu */
void yawn(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You yawn sleepily and adjust your pillow.\n", pc);
      else if (crit_ptr == &pc) {
         show("You yawn sleepily and adjust your pillow.\n", pc);
         Sprintf(buf, "yawns to %s \n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You yawn at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S yawns sleeping at you. Are you THAT boring?!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S yawns sleepily at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You yawn sleepily and adjust your pillow.\n", pc);
      emote("yawns. Are you THAT boring?", pc, rm, TRUE);
   }//else
}//yawn


/* Made by Shamu */
void bonk(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You bonk youself. What a numbskull.\n", pc);
         Sprintf(buf, "bonks %s. What a weirdo.\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You bonk %S!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S bonks you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S bonks %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You bonk yourself. What a numbskull.\n", pc);
      emote("bonks the air. What a weirdo.", pc, rm, TRUE);
   }//else
}//bonk

/* Made by Shamu */
void pft(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You exclaim, 'pftpftpftpft!'\n", pc);
         Sprintf(buf, "exclaims, 'pftpftpftpft!' at %s.\n", 
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You look at %S and exclaim, 'pftpftpftpft!'.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S looks at you and exclaims, 'pftpftpftpft!'\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S exclaims, 'pftpftpftpft!' to %S!\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You exclaim, 'pftpftpftpft!'\n", pc);
      emote("exclaims, 'pftpftpftpft!'", pc, rm, TRUE);
   }//else
}//pft


/* Made by Shamu */
void clap(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You feel too relaxed.\n", pc);
      else if (crit_ptr == &pc) {
         show("You clap appreciatively!\n", pc);
         Sprintf(buf, "claps appreciatively to %s!\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You clap at %S's performance.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S claps at your performance.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S claps at %S's performance.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You clap appreciatively!\n", pc);
      emote("claps appreciatively!", pc, rm, TRUE);
   }//else
}//clap

/* Made by Shamu */
void shake(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   critter* targ = NULL;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         targ = &pc;
         show("You shake your head in denial.\n", pc);
         Sprintf(buf, "shakes %s head in denial.!\n",
		get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         targ = crit_ptr;
         Sprintf(buf, "You shake your head in denial at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S shakes his head in denial at your claims!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S shakes his head in denial at %S.\n",
                       name_of_crit(pc, ptr->SEE_BIT),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You shake your head in denial.\n", pc);
      emote("shakes his head in denial.", pc, rm, TRUE);
   }//else

   String cmd = "shake";
   if (targ) {
      rm.checkForProc(cmd, NULL_STRING, pc, targ->getIdNum());
   }
   else {
      rm.checkForProc(cmd, NULL_STRING, pc, 0);
   }
}//shake


/* Made by Shamu */
void groan(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You groan in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You groan loudly, clutching your stomach.\n", pc);
         Sprintf(buf, "groans loudly to %s.\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You groan at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S groans at you. What could you have done?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S groans at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You groan loudly, clutching your stomach.\n", pc);
      emote("groans loudly.", pc, rm, TRUE);
   }//else
}//groan


/* Made by Shamu */
void hmm(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You drool a little harder.\n", pc);
      else if (crit_ptr == &pc) {
         show("You think for a while. Hmmmmm....!\n", pc);
         emote("thinks, 'Hmmm....'\n", pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You look at %S and think, 'Hmmm....'\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S looks at you and thinks, 'Hmmm....'\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S glances at %S and goes, 'Hmmmm...'\n", 
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You think for a while. Hmmmmm....\n", pc);
      emote("thinks for a while. Hmmmmm....", pc, rm, TRUE);
   }//else
}//hmm


/* Made by Shamu */
void hum(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You hum in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You hum a tune.\n", pc);
         Sprintf(buf, "hums a tune to %s.\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You hum a tune just for %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S hums a tune for you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S hums a tune for %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You hum a tune.\n", pc);
      emote("hums a tune.", pc, rm, TRUE);
   }//else
}//hum


/* Made by Shamu */
void grumble(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You grumble in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You grumble to yourself.\n", pc);
         Sprintf(buf, "grumbles to %s. Is something wrong?\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You grumble at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S grumbles at you. What could you have done?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S grumbles at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You grumble. Is something bothering you?\n", pc);
      emote("grumbles at the world!", pc, rm, TRUE);
   }//else
}//grumble


/* Made by Shamu */
void listen(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You listen to your dreams.\n", pc);
      else if (crit_ptr == &pc) {
         show("You listen attentively.\n", pc);
         Sprintf(buf, "listens attentively to %s.\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You listen to %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S listens to you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S listens attentively to %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You listen attentively.\n", pc);
      emote("listens attentively.", pc, rm, TRUE);
   }//else
}//listen


/* Made by Shamu */
void pinch(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You pinch yourself. Nope, you're not dreaming.\n", pc);
         Sprintf(buf, "pinches %s on the arm.\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You pinch %S on the arm..\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S pinches you on the arm. Ouch!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S pinches %S on the arm. Uh-oh!\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You pinch yourself. Nope, you're not dreaming.\n", pc);
      emote("pinches a bug.", pc, rm, TRUE);
   }//else
}//pinch


/* Made by Shamu */
void roll(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You roll your eyes.\n", pc);
      else if (crit_ptr == &pc) {
         show("You roll your eyes.\n", pc);
         Sprintf(buf, "rolls his eyes at %s.\n",
		get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You roll your eyes at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S rolls his eyes at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S rolls %s eyes at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT), get_his_her(pc),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You roll your eyes.\n", pc);
      Sprintf(buf, "rolls %s eyes.\n", get_his_her(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//roll


/* Made by Shamu */
void strut(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You strut proudly.\n", pc);
         Sprintf(buf, "struts proudly.\n");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You strut proudly, eyeing %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S struts proudly before you.  Nice, eh?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S struts proudly before %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You strut proudly.\n", pc);
      emote("struts proudly.", pc, rm, TRUE);
   }//else
}//strut


/* Made by Shamu */
void whistle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You whistle a tune in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You whistle a tune to yourself.\n", pc);
         Sprintf(buf, "whistles a tune to %s.\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You whistle a special song for %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S whistles a special song for you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S whistles a song for %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You whistle a tune.\n", pc);
      emote("whistles a tune.", pc, rm, TRUE);
   }//else
}//whistle


/* Made by Shamu */
void slap(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr = NULL;
   critter* crit_ptr = NULL;

   if (vict->Strlen()) {
      crit_ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You slap yourself silly.\n", pc);
         Sprintf(buf, "slaps %s silly. What a weirdo.\n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You slap %S across the face!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S slaps you across the face. Grrr! That did it!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S slaps %S across the face.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You slap the wall.\n", pc);
      emote("slaps the ground in anger.", pc, rm, TRUE);
   }//else

   String cmd = "slap";
   if (crit_ptr) {
      rm.checkForProc(cmd, NULL_STRING, pc, crit_ptr->getIdNum());
   }
   else {
      rm.checkForProc(cmd, NULL_STRING, pc, 0);
   }

}//slap


/* Made by Shamu */
void muahaha(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You gargle in your sleep!\n", pc);
      else if (crit_ptr == &pc) {
         show("You burst forth with a booming 'MUAHAHAHAHA!'\n", pc);
         Sprintf(buf, "bursts forth with a booming 'MUAHAHAHAHA!'\n");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You laugh mercilessly at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S looks at you and yells, 'MUAHAHAHAHAHA!'\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S laughs mercilessly at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You burst forth with a booming 'MUAHAHAHAHAHA!'\n", pc);
      emote("bursts forth with a booming 'MUAHAHAHAHAHA'!", pc, rm, TRUE);
   }//else
}//muahaha


/* Made by Shamu */
void hehe(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You snarfle in your sleep!\n", pc);
      else if (crit_ptr == &pc) {
         show("You giggle like the Pillsbury Doughboy!\n", pc);
         Sprintf(buf, "giggles like the Pillsbury Doughboy at %s! \n",
		 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You giggle like the Pillsbury Doughboy at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S giggles strangely at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, 
		 "%S giggles like the Pillsbury Doughboy at %S!\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You giggle like the Pillsbury Doughboy!\n", pc);
      emote("giggles like the Pillsbury Doughboy!", pc, rm, TRUE);
   }//else
}//hehe


/* Made by Shamu */
void silly(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You twitch a little\n", pc);
      else if (crit_ptr == &pc) {
         show("You say, 'Oh, silly me!\n", pc);
         Sprintf(buf, "exclaims, 'Oh, silly me!'\n");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You say to %S, 'You're so silly!'\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S says to you, 'You're so silly!'\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S says, 'You're so silly!' to %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You say, 'Oh, silly me!'\n", pc);
      emote("exclaims, 'Oh, silly me!'", pc, rm, TRUE);
   }//else
}//silly


/* Made by Shamu */
void scold(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You twitch in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You scold yourself.\n", pc);
         Sprintf(buf, "scolds %sf.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You scold %S!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S scolds you! What did you do?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S scolds %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You scold the harsh world!\n", pc);
      emote("scolds the harsh world!", pc, rm, TRUE);
   }//else
}//scold


/* Coded by Crazy */
void twiddle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   if (vict->Strlen()) {
      critter* crit_ptr = 
           ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);
      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You twiddle your thumbs.\n", pc); 
         Sprintf(buf, "Twiddles %s thumbs.\n", 
		 get_his_her(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You twiddle your thumbs because of %S.\n", 
         name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S twiddles %s thumbs beacuse of you!!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT),
		 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S twiddles %s thumbs beacuse of %S.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), 
		 get_his_her(pc),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You twiddle your thumbs.\n", pc);
      Sprintf(buf, "twiddles %s thumbs.", get_his_her(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//twiddle

