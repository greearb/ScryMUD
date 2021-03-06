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


// Offered by Kadann
void tango(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   
   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);
      
      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You bite down on a rose cutting your tongue!\n", pc);
         Sprintf(buf, "pricks %s tongue on a rose thorn!\n", get_his_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You hand %pS a rose, and embrace the dance of love with %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT), name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS hands you a rose... you instintively bite down on it as you embrace the dance of love.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS grins at %pS.\n",
                       name_of_crit(pc, ptr->SEE_BIT),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You dance the tango with an imaginary friend!\n", pc);
      emote("dances around the room like a wierdo...with a rose...", pc, rm, TRUE);
   }//else
}//tango


/*Coded By GROCK*/
void smirk(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      show("You smirk in your sleep.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You smirk to yourself.\n", pc); 
         Sprintf(buf, "smirks at %s", get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You smirk at %pS.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS smirks at you.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS smirks at %pS.\n", 
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

   if (pc.POS == POS_SLEEP) {
      show("You gesture in your sleep.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You beckon to yourself.\n", pc); 
         Sprintf(buf, "beckons at %s", get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You beckon %pS to follow you.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS beckons you to follow %s.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_him_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS beckons %pS.\n", 
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

   if (pc.POS == POS_SLEEP) {
      show("You cry in your sleep.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You sob quietly to yourself.\n", pc); 
         Sprintf(buf, "sobs quietly to %s.", get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You sob on %pS's shoulder.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS sobs on your shoulder.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS sobs on %pS's shoulder.\n", 
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

   if (pc.POS == POS_SLEEP) {
      show("Let's just hope you have nothing to splash in!\n", pc);
      return;
   }

   if (!(rm.isSmallWater() || rm.isBigWater() ||
       rm.isSwamp())) {
     show("You go to splash, and only find a dry floor!\n", pc);
     return;
   }//if

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to splash anyone.\n", pc);
      else if (crit_ptr == &pc) {
         show("You splash yourself in the face!\n", pc); 
         Sprintf(buf, "splashes %s in the face!", get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You splash %pS in the face!\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS splashes you in the face!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS splashes %pS in the face!\n", 
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

   if (pc.POS == POS_SLEEP) {
      show("Perhaps you should try this while you are awake.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You pat yourself on the back.\n", pc); 
         Sprintf(buf, "pats %s on the back.\n", get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You pat %pS on the back.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS pats you on the back.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS pats %pS on the back.\n", 
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

   if (pc.POS == POS_SLEEP) {
      show("A terrible itch causes you to sleep restlessly.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS == POS_SLEEP)
         show("You itch horribly in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You make yourself break out in hives!\n", pc); 
         Sprintf(buf, "makes %s break out in hives!",
                 get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You make %pS feel itchy all over.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS makes you feel itchy all over!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS tries to make %pS itchy.\n", 
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

   if (pc.POS == POS_SLEEP) {
      show("You toss and turn restlessly.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You wiggle yourself.\n", pc);
         Sprintf(buf, "wiggles %s.\n", get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You wiggle your bottom at %pS.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS wiggles %s bottom at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS wiggles %s bottom at %pS!\n", 
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

   if (pc.POS == POS_SLEEP) {
      show("You are quite agreeable while you are asleep!\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You agree with yourself...for a change!\n", pc); 
         Sprintf(buf, "agrees with %s.\n", get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You agree with %pS.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS agrees with you.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS agrees with %pS.\n", 
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
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You blush.\n", pc); 
         Sprintf(buf, "blushes at %s.\n", get_himself_herself(pc)); 
      emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You blush at the sight of %pS.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS blushes at the sight of you.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS blushes at the sight of %pS.\n", 
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

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You pucker and try to kiss your dream %s!",
              pc.SEX==SEX_MALE?"woman":
              (pc.SEX==SEX_FEMALE?"man":"being of undisclosed gender"));
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You kiss yourself.\n", pc); //change
         Sprintf(buf, "Kisses %s.\n", get_himself_herself(pc)); 
//change
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You kiss %pS.\n", //change
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS kisses you long and passionatly.\n", //change
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS kisses %pS passionately.\n", //change
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

   // Actually toggles the players AFK flag.
   pc.PC_FLAGS.flip(33);

   if ( pc.isAFK() ) {
      show("You go afk.\n", pc);
      Sprintf(buf, "steps away from %s keyboard.", get_his_her(pc));
   } else {
      show("You are no longer afk.\n", pc);
      Sprintf(buf, "returns to %s keyboard.", get_his_her(pc));
   }

   emote(buf, pc, rm, TRUE); 
}//afk


/* Made by Shamu */
void giggle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You know what they say about people who laugh in their sleep?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You giggle uncontrollably!\n", pc);
         Sprintf(buf, "giggles crazily at %s!\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You giggle at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS giggles crazily at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS giggles crazily at %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      pc.show("In your attempt to shrug while asleep, you dig your shoulder into the back of your neck!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You shrug helplessly to yourself.\n", pc);
         Sprintf(buf, "shrugs helplessly to %s.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You shrug in response to %pS's remark.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS shrugs helplessly at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS shrugs at %pS.\n",
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
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You turn your face inside out and WIBBLE!\n", pc);
      else if (crit_ptr == &pc) {
         show("You turn your face inside out and WIBBLE!\n", pc);
         Sprintf(buf, "WIBBLES! %s\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You turn your face inside out and WIBBLE at %pS!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS turns %s face inside out and WIBBLES at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT), get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, 
                 "%pS turns %s face inside out and WIBBLES at %pS.\n",
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
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You yawn sleepily and adjust your pillow.\n", pc);
      else if (crit_ptr == &pc) {
         show("You yawn sleepily and adjust your pillow.\n", pc);
         Sprintf(buf, "yawns to %s.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You yawn at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS yawns sleepily at you. Are you THAT boring?!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS yawns sleepily at %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      pc.show("Oh, that's just not right!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You bonk yourself. What a numbskull.\n", pc);
         Sprintf(buf, "bonks %s. What a weirdo.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You bonk %pS!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS bonks you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS bonks %pS.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You somehow manage to bonk the air.\n", pc);
      emote("bonks the air. What a weirdo.", pc, rm, TRUE);
   }//else
}//bonk

/* Made by Shamu */
void pft(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You might want to try waking first.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You exclaim, 'pftpftpftpft!'\n", pc);
         Sprintf(buf, "exclaims, 'pftpftpftpft!' at %s.\n", 
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You look at %pS and exclaim, 'pftpftpftpft!'.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS looks at you and exclaims, 'pftpftpftpft!'\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS exclaims, 'pftpftpftpft!' to %pS!\n",
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

   if (pc.POS == POS_SLEEP) {
      pc.show("Perhaps you should try and wake first.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You feel too relaxed.\n", pc);
      else if (crit_ptr == &pc) {
         show("You clap appreciatively!\n", pc);
         Sprintf(buf, "claps appreciatively to %s!\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You clap at %pS's performance.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS claps at your performance.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS claps at %pS's performance.\n",
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

   if (pc.POS == POS_SLEEP) {
      pc.show("You twitch slightly in your sleep.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) {
         show("You don't see that person.\n", pc);
      }
      else if (pc.POS > POS_SIT) {
         show("You would probably want to be standing to do that.\n", pc);
      }
      else if (crit_ptr == &pc) {
         targ = &pc;
         show("You shake your head in denial.\n", pc);
         Sprintf(buf, "shakes %s head in denial.!\n", get_his_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         targ = crit_ptr;
         Sprintf(buf, "You shake your head in denial at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS shakes %s head in denial at your claims!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT), get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS shakes %s head in denial at %pS.\n",
                       name_of_crit(pc, ptr->SEE_BIT), get_his_her(pc),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You shake your head in denial.\n", pc);
      emote("shakes in denial.", pc, rm, TRUE);
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

   if (pc.POS == POS_SLEEP) {
      show("You groan in your sleep.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You groan loudly, clutching your stomach.\n", pc);
         Sprintf(buf, "groans loudly, clutching %s stomach.\n",
                 get_his_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You groan at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS groans at you. What could you have done?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS groans at %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      show("You drool a little harder.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You think for a while. Hmmmmm....!\n", pc);
         emote("thinks, 'Hmmm....'\n", pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You look at %pS and think, 'Hmmm....'\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS looks at you and thinks, 'Hmmm....'\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS glances at %pS and goes, 'Hmmmm...'\n", 
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

   if (pc.POS == POS_SLEEP) {
      show("You hum in your sleep.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You hum a tune.\n", pc);
         Sprintf(buf, "hums a tune to %s.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You hum a tune just for %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS hums a tune for you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS hums a tune for %pS.\n",
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
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You grumble in your sleep.\n", pc);
      else if (crit_ptr == &pc) {
         show("You grumble to yourself.\n", pc);
         Sprintf(buf, "grumbles to %s. Is something wrong?\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You grumble at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS grumbles at you. What could you have done?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS grumbles at %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      show("You submit to the voices in your head.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You listen attentively.\n", pc);
         Sprintf(buf, "listens attentively to %s.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You listen to %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS listens to you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS listens attentively to %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      show("You pinch yourself. Yep, you're dreaming.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You pinch yourself. Nope, you're not dreaming.\n", pc);
         Sprintf(buf, "pinches %s on the arm.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You pinch %pS on the arm..\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS pinches you on the arm. Ouch!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS pinches %pS on the arm. Uh-oh!\n",
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

   if (pc.POS == POS_SLEEP) {
      pc.show("Oh yeah, THAT would be effective!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You roll your eyes.\n", pc);
      else if (crit_ptr == &pc) {
         show("You roll your eyes.\n", pc);
         Sprintf(buf, "rolls %s eyes at %s.\n", get_his_her(pc),
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You roll your eyes at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS rolls %s eyes at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT), get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS rolls %s eyes at %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"Why would you need to strut when you have all these %s fawning all over you?",
              pc.SEX==SEX_MALE?"women":
              (pc.SEX==SEX_FEMALE?"men":"beings of undisclosed gender"));
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

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
         Sprintf(buf, "You strut proudly, eyeing %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS struts proudly before you.  Nice, eh?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS struts proudly before %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      show("You whistle a tune in your sleep.\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You whistle a tune to yourself.\n", pc);
         Sprintf(buf, "whistles a tune to %s.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You whistle a special song for %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS whistles a special song for you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS whistles a song for %pS.\n",
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
      crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You would probably want to be standing to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You slap yourself silly.\n", pc);
         Sprintf(buf, "slaps %s silly. What a weirdo.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You slap %pS across the face!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS slaps you across the face. Grrr! That did it!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS slaps %pS across the face.\n",
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

   if (pc.POS == POS_SLEEP) {
      show("You gargle in your sleep!\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You burst forth with a booming 'MUAHAHAHAHA!'\n", pc);
         Sprintf(buf, "bursts forth with a booming 'MUAHAHAHAHA!'\n");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You laugh mercilessly at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS looks at you and yells, 'MUAHAHAHAHAHA!'\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS laughs mercilessly at %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      show("You snarfle in your sleep!\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You giggle like the Pillsbury Doughboy!\n", pc);
         Sprintf(buf, "giggles like the Pillsbury Doughboy at %s! \n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You giggle like the Pillsbury Doughboy at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS giggles strangely at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, 
                 "%pS giggles like the Pillsbury Doughboy at %pS!\n",
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

   if (pc.POS == POS_SLEEP) {
      show("You twitch a little\n", pc);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You say, 'Oh, silly me!\n", pc);
         Sprintf(buf, "exclaims, 'Oh, silly me!'\n");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You say to %pS, 'You're so silly!'\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS says to you, 'You're so silly!'\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS says, 'You're so silly!' to %pS.\n",
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

   if (pc.POS == POS_SLEEP) {
      show("You twitch in your sleep.\n", pc);
      return;
   }
   if (vict->Strlen()) {
      critter* crit_ptr =
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You scold yourself.\n", pc);
         Sprintf(buf, "scolds %s.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You scold %pS!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS scolds you! What did you do?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS scolds %pS.\n",
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
           rm.haveCritNamed(i_th, vict, pc);
      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You twiddle your thumbs.\n", pc); 
         Sprintf(buf, "twiddles %s thumbs.\n", 
                 get_his_her(pc)); 
         emote(buf, pc, rm, TRUE);
      }
      else {
         Sprintf(buf, "You twiddle your thumbs because of %pS.\n", 
         name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS twiddles %s thumbs because of you!!\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS twiddles %s thumbs because of %pS.\n", 
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

/* by khaavren */
void peck(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You kiss your dream %s lightly on the cheek!\n",
              pc.SEX==SEX_MALE?"woman":
              (pc.SEX==SEX_FEMALE?"man":"being of undisclosed gender"));
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You foolishly attempt to kiss yourself on the cheek.\n", pc);
         Sprintf(buf, "kisses %s lightly on the cheek.\n",
                 get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You kiss %pS lightly on the cheek.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS kisses you lightly on the cheek.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS kisses %pS lightly on the cheek.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You can't find the wind's cheek.\n", pc);
      emote("can't seem to find the wind's cheek.", pc, rm, TRUE);
   }//else
}//peck

/* by khaavren */
void trout(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You dream of flying fish.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You slap yourself silly with a wet trout!\n", pc);
         Sprintf(buf, "slaps %s silly with a wet trout! Doh.",
                 get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You slap %pS silly with a wet trout!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS slaps you silly with a wet trout!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS slaps %pS silly with a wet trout!\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You whip out your trout and wave it around wildly.\n", pc);
      emote("whips out a trout and waves it around wildly.", pc, rm, TRUE);
   }//else
}//trout

/* by khaavren */
void tremble(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You dream of hungry sharks and tremble.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You tremble from fear of your own shadow.\n", pc);
         Sprintf(buf, "trembles from fear of %s own shadow.\n",
                 get_his_her(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You tremble at the sight of %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS trembles at the sight of you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS trembles at the sight of %pS.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You tremble.\n", pc);
      emote("trembles in fear.", pc, rm, TRUE);
   }//else
}//tremble

/* by khaavren */
void goo(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You dream of drowning in a vat of yellow goo.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You cover yourself with yellow goo.\n", pc);
         Sprintf(buf, "splashes yellow goo all over %s face.\n",
                 get_his_her(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You dip %pS in a vat of yellow goo.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS dunks you in a vat of yellow goo.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS dunks %pS in a vat of yellow goo.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You play with yellow goo.\n", pc);
      emote("plays with yellow goo.", pc, rm, TRUE);
   }//else
}//goo

/* by khaavren */
void smooch(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You kiss your pillow... Muah!\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You bend over and kiss your feet. Muah!\n", pc);
         Sprintf(buf, "bends over and kisses %s feet.\n",
                 get_his_her(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You pucker up and kiss %pS. Muah!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS gives you a big smooch. Muah!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS gives %pS a big smooch. Muah!\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You pucker up and kiss the air! Muah!\n", pc);
      emote("puckers up and kisses the air. Muah!", pc, rm, TRUE);
   }//else
}//smooch

/* by khaavren */
void swoon(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You have already passed out squilly!\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You fall all over yourself.\n", pc);
         Sprintf(buf, "falls all over %s.\n",
                 get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You collapse in %pS's arms.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS collapses in your arms.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS collapses in %pS's arms.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You collapse in a heap and sigh dreamily!\n", pc);
      emote("collapses in a heap and sighs dreamily!", pc, rm, TRUE);
   }//else
}//swoon

/* by khaavren */
void twirl(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You dream of twirling about in a green field.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You attempt to pick yourself up over your head and... fail!\n",
                 pc);
         Sprintf(buf, "attempts to pick %s up over %s head and... fails!\n",
                 get_himself_herself(pc),
                 get_his_her(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You pick %pS up over your head and twirl.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS picks you up above %s head and twirls.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS picks %pS up above %s head and twirls around.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT),
                 get_his_her(pc));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You twirl about like an exotic dancer.\n", pc);
      emote("twirls about the room like an exotic dancer.", pc, rm, TRUE);
   }//else
}//twirl

/* by khaavren */
void tickle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You start laughing in your sleep.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You tickle yourself... not much fun!\n",
                 pc);
         Sprintf(buf, "tickles %s. Doesn't look like much fun.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You tickle %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS starts to tickle you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS starts to tickle %pS.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("And just who do you plan on tickling?\n", pc);
   }//else
}//tickle

/* by khaavren */
void battlecry(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You dream of the battlefield, of honor and world domination.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You try to inspire yourself with a fierce battlecry!\n",
                 pc);
         Sprintf(buf, "tries to inspire %s with a fierce battlecry!\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You try to inspire %pS with a fierce battlecry!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS tries to inspire you with a fierce battlecry!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS tries to inspire %pS with a fierce battlecry!\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You let out a fearsome battlecry!\n", pc);
      emote("sounds a fearsome battlecry!.", pc, rm, TRUE);
   }//else
}//battlecry

/* by khaavren */
void loser(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You mumble about losers in your sleep.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You're lookin' kinda' dumb with your finger and your thumb in the shape of an L on your forehead.\n",
                 pc);
         Sprintf(buf, "is lookin' kinda' dumb with %s finger and %s thumb in the shape of an L on %s forehead.\n",
                 get_his_her(pc),
                 get_his_her(pc),
                 get_his_her(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You motion that %pS is a loser with your finger and your thumb in the shape of an L on your forehead.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS shows you how big of a loser %s thinks you are by placing %s finger and %s thumb in the shape of an L on %s forehead.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_he_she(pc),
                 get_his_her(pc),
                 get_his_her(pc),
                 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS shows everyone what a loser %pS is by placing %s finger and %s thumb in the shape of an L on %s forehead.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT),
                 get_his_her(pc),
                 get_his_her(pc),
                 get_his_her(pc));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You place your finger and your thumb in the shape of an L on your forehead.\n", pc);
      Sprintf(buf, "places %s finger and %s thumb in the shape of an L on %s forehead.",
              get_his_her(pc),
              get_his_her(pc),
              get_his_her(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//loser

/* by khaavren */
void nestle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You nestle up with yourself and dream in peace.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You look silly trying to nestle with yourself!\n",
                 pc);
         Sprintf(buf, "looks silly trying to nestle with %s!\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You climb into %pS's lap and nestle happily.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS climbs into your lap and nestles there happily.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS climbs into %pS's lap and nestles there happily.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("Who do you plan on nestling with?\n", pc);
   }//else
}//nestle

/* by khaavren */
void sweatdrop(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You feel damp all over. Hope it's sweat.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("An enormous sweatdrop appears on your forehead as your pants fall down. You hurriedly pull them back up.\n",
                 pc);
         Sprintf(buf, "turns bright red, and an enormous sweatdrop appears on %s forehead as %s pants fall down. Hurriedly %s pulls them back up.\n",
                 get_his_her(pc),
                 get_his_her(pc),
                 get_his_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You look over at %pS, and an enormous sweatdrop appears on your forehead.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS looks over at you, and an enormous sweatdrop appears on %s forehead.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS looks over at %pS, and an enormous sweatdrop appears on %s forehead.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT),
                 get_his_her(pc));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You turn bright red, and an enormous sweatdrop appears on your forehead.\n", pc);
      Sprintf(buf, "turns bright red, and an enormous sweatdrop appears on %s forehead.\n",
              get_his_her(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//sweatdrop

/* by khaavren */
void shudder(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You shudder at the thought of facing another day.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("With the realization that you might be insane, you shudder.\n",
                 pc);
         Sprintf(buf, "shudders with the realization that %s may be insane.\n",
                 get_he_she(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You look at %pS and shudder.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS looks at you and shudders.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS shudders at %pS.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You shudder.\n", pc);
      emote("shudders.\n", pc, rm, TRUE);
   }//else
}//shudder

/* by khaavren */
void cower(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You cower away from the evil mage attempting to polymorph you.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You're afraid of your own shadow?\n",
                 pc);
         Sprintf(buf, "is afraid of %s own shadow.\n",
                 get_his_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You cower away from %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS cowers away from you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS cowers away from %pS.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You cower in fear.\n", pc);
      emote("cowers in fear.\n", pc, rm, TRUE);
   }//else
}//cower

/* by khaavren */
void climb(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You climb. hmmmm... you need new dreams.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("In an attempt to climb yourself, you trip and fall.\n",
                 pc);
         Sprintf(buf, "attempting to do... well who knows... trips all over %s.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You climb up %pS's leg and cling to it.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS climbs up your leg and clings to it.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS climbs up %pS's leg and clings to it.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You proclaim to be King of the Mountain.\n", pc);
      emote("proclaims to be King of the Mountain.\n", pc, rm, TRUE);
   }//else
}//climb

void shock(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You suddenly find yourself sitting in the math lecture hall on test day. Eep! You're in your underware!\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You wrap yourself in nylon, rub against the carpet, and reach for a nearby doornob. The resulting blast sends you sailing across the room.\n",
                 pc);
         Sprintf(buf, "wraps %s in nylon, rubs against the carpet, and reaches for a nearby doornob. A massive electrical discharge launches %s across the room!\n",
                 get_himself_herself(pc), get_him_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You rub your feet on the carpet and zap %pS!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS rubs %s feet on the carpet and zaps you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT), get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS rubs %s feet on the carpet and zaps %pS!.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 get_his_her(pc),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You look shocked!\n", pc);
      emote("looks shocked!\n", pc, rm, TRUE);
   }//else
}//shock

/* by khaavren */
void wiggletoes(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"Zzzzz Zzzzz Zzzzz wiggle Zzzzz Zzzz wiggle.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You look down and wiggle your toes to make sure they are all there.\n",
                 pc);
         Sprintf(buf, "looks down and wiggles %s toes to make sure they are all there.\n",
                 get_his_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You gleefully wiggle your toes at %pS. How cute!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS gleefully wiggles %s toes at you. How cute!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS gleefully wiggles %s toes at %pS. How cute!\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 get_his_her(pc),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You gleefully wiggle your toes. How cute!\n", pc);
      Sprintf(buf, "gleefully wiggles %s toes. How cute!\n",
            get_his_her(pc));
      emote(buf , pc, rm, TRUE);
   }//else
}//wiggletoes

void stare(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You stare at the back of your eyelids.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You look yourself right in the eye.  Who will blink first?\n",
                 pc);
         Sprintf(buf, "looks %s right in the eye, daring %s to blink.\n",
                 get_himself_herself(pc), get_him_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You stare blankly at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS stares blankly at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS stares blankly at %pS.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You stare blankly into space.\n", pc);
      Sprintf(buf, "stares blankly into space.\n");
      emote(buf , pc, rm, TRUE);
   }//else
}//stare

/* by khaavren */
void blink(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You're too tired to blink.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You blink.\n",
                 pc);
         Sprintf(buf, "blinks.");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You blink at %pS in disbelief.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS blinks at you in disbelief.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS blinks at %pS in disbelief.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You blink in disbelief.\n", pc);
      emote("blinks in disbelief.", pc, rm, TRUE);
   }//else
}//blink

void eyebrow(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You roll over and twitch.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You raise an eyebrow.\n",
                 pc);
         Sprintf(buf, "raises an eyebrow.\n");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You raise an eyebrow at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS raises an eyebrow at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS raises an eyebrow at %pS.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You raise an eyebrow.\n", pc);
      emote("raises an eyebrow.\n", pc, rm, TRUE);
   }//else
}//eyebrow

void whimper(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You whimper in your sleep.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You whimper.\n",
                 pc);
         Sprintf(buf, "whimpers.\n");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You whimper at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS whimpers at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS whimpers at %pS.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You whimper.\n", pc);
      emote("whimpers.\n", pc, rm, TRUE);
   }//else
}//whimper

void duck(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"Eh?\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You duck.\n",
                 pc);
         Sprintf(buf, "ducks.\n");
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You duck to avoid %pS's imminent attack.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS ducks to avoid you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS ducks to avoid %pS's imminent attack.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You duck.\n", pc);
      emote("ducks.\n", pc, rm, TRUE);
   }//else
}//duck

void pet(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You must be awake to do this.\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You pet yourself and enjoy it greatly.\n",
                 pc);
         Sprintf(buf, "pets %s lasciviously.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You pet %pS fondly.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS pets you affectionately.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS pets %pS affectionately.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You attempt to pet the air.\n", pc);
      emote("pets something that's not there.\n", pc, rm, TRUE);
   }//else
}//pet

void tap(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"zzzz... zzz...\n");
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (crit_ptr == &pc) {
         show("You tap yourself on the shoulder"
               " but all it does is confuse you.\n", pc);
         Sprintf(buf, "taps %s on the should and then spins around in circles"
               " trying to catch up to %s.\n", get_himself_herself(pc),
               get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You walk up to %pS and tap them on the shoulder.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%pS walks up behind you and taps you on the shoulder.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS walks up behind %pS and taps %s on the"
                     " shoulder.\n", name_of_crit(pc, ptr->SEE_BIT), 
                     name_of_crit(*crit_ptr, ptr->SEE_BIT),
                     get_him_her(*crit_ptr));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      //change these next two lines
      show("You tap your foot on the ground as you grow impatient.\n", pc);
      Sprintf(buf, "taps his foot loudly on the ground with an impatient"
            " look on %s face.\n", get_his_her(pc));
      pc.emote(buf);
   }//else
}//tap

void zombie(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You dream of being chased by zombies, run!\n");
      return;
   }
   else if (pc.POS > POS_SIT){
	   pc.show(NOT_IN_POSN);
	   return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

	  if (!crit_ptr) {
         pc.show("You mumble something about brains.\n");
	     pc.emote("mumbles something about brains.\n");
	  }
      else if (crit_ptr == &pc) {
         pc.show("You mumble something about a lack of brains.\n");
         Sprintf(buf, "mumbles something about a lack of brains.\n");
		 emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You stumble up to %pS and start chewing on their brains! Mmmm, tastes like people...\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         pc.show(buf);
         Sprintf(buf, "%pS stumbles up to you and starts chewing on your brains!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         crit_ptr->show(buf);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
			   Sprintf(buf, "%pS stumbles up to %pS and starts chewing on their brains!\n", name_of_crit(pc, ptr->SEE_BIT), name_of_crit(*crit_ptr, ptr->SEE_BIT)); 
               buf.Cap();
               ptr->show(buf);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      pc.show("You stumble about, mumbling something about brains.\n");
      pc.emote("stumbles about, mumbling something about brains.\n");
   }//else
}//zombie

void curl(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You curl up into a ball in your sleep.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

	  if (!crit_ptr) {
         pc.show("You look lonely as you curl up in the corner.\n");
	     pc.emote("looks lonely and curls up in the corner.\n");
	  }
      else if (crit_ptr == &pc) {
         pc.show("You try to curl up around yourself.\n");
         Sprintf(buf, "Tries to curl up around %s.\n", get_himself_herself(pc));
		 emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You curl up next to %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         pc.show(buf);
         Sprintf(buf, "%pS curls up next to you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         crit_ptr->show(buf);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
			   Sprintf(buf, "%pS curls up with %pS.\n", name_of_crit(pc, ptr->SEE_BIT), name_of_crit(*crit_ptr, ptr->SEE_BIT)); 
               buf.Cap();
               ptr->show(buf);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      pc.show("Curls up in the corner.\n");
      pc.emote("curls up in the corner.\n");
   }//else
}//curl

void snuggle(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You snuggle with your teddybear.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) pc.show("You don't see them here.\n");
      else if (crit_ptr == &pc) {
         pc.show("You snuggle up with yourself.\n");
         Sprintf(buf, "snuggles up with %s.\n", get_himself_herself(pc));
		 emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You snuggle up with %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         pc.show(buf);
         Sprintf(buf, "%pS snuggles with you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         crit_ptr->show(buf);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
			   Sprintf(buf, "%pS snuggles with %pS.\n", name_of_crit(pc, ptr->SEE_BIT), name_of_crit(*crit_ptr, ptr->SEE_BIT)); 
               buf.Cap();
               ptr->show(buf);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      pc.show("You snuggle up under your blanket.\n");
     Sprintf(buf, "snuggles up under %s blanket.\n", get_his_her(pc));
	 pc.emote(buf);
   }//else
}//snuggle

void gasp(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You dream of drowning!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
           rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) pc.show("You don't see them here.\n");
      else if (crit_ptr == &pc) {
         pc.show("You gasp in atonishment at your own actions.\n");
         Sprintf(buf, "gasps in astonishment at %s own actions.\n", 
            get_his_her(pc));
		   emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You gasp in astonishment at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         pc.show(buf);
         Sprintf(buf, "%pS gasps in astonishment at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         crit_ptr->show(buf);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
			      Sprintf(buf, "%pS gasps in astonishment at %pS.\n", 
                  name_of_crit(pc, ptr->SEE_BIT), 
                  name_of_crit(*crit_ptr, ptr->SEE_BIT)); 
               buf.Cap();
               ptr->show(buf);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      pc.show("You gasp in astonishment.\n");
      pc.emote("gasps in astonishment.\n");
   }//else
}//gasp

void whee(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("Your dreams elicit a sleepy \"Whee\" to yourself!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) pc.show("You don't see that person here.\n");

      else if (crit_ptr == &pc) { //at self
         pc.show("You \"WheEeEEeeE\" at yourself.\n");
         Sprintf(buf, "shouts \"WheEeEEeeE\" at %s.\n", 
            get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else { //at someone else
         Sprintf(buf, "You shout out \"WheEeEEeeE\" at %pS.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         pc.show(buf);
         Sprintf(buf, "%pS shouts \"WheEeEEeeE\" in your direction.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         crit_ptr->show(buf);

         while ((ptr = cll.next())) { //show everyone else
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%pS shouts \"WheEeEEeeE\" at %pS.\n", 
                  name_of_crit(pc, ptr->SEE_BIT), 
                  name_of_crit(*crit_ptr, ptr->SEE_BIT)); 
               buf.Cap();
               ptr->show(buf);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      //just plain "whee"
      pc.show("You shout \"WheEeEEeeE\" for all the room to hear!\n");
      pc.emote("shouts out \"WheEeEEeeE\"!\n");
   }//else
}//whee
