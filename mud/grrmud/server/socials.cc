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


void whip(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   
   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);
      
      if (!crit_ptr) 
         show("eh, who?!\n", pc);
      else if (pc.POS > POS_SIT)
         show("You are definitely not in a good position to try that!\n", pc);
      else if (crit_ptr == &pc) {
         show("You crack your whip, catching the tip of your left ear!!\n", pc);
         Sprintf(buf, "cracks a leather whip at %s!\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You crack your leather whip at %S!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S cracks a leather whip at your butt!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S cracks a leather whip at %S!\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You crack your leather whip like Indiana Jones and nearly cut yourself on the chin!\n", pc);
      emote("cracks a leather whip like Indiana Jones and nearly looses a chin!", pc, rm, TRUE);
   }//else
}//whip



/* By Shamu */
void mutter(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You mutter about your foolish actions.\n", pc);
         Sprintf(buf, "mutters to %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You mutter under your breath about %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S mutters something under %s breath about you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT), get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S mutters about %S.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You mutter.\n", pc);
      emote("mutters.", pc, rm, TRUE);
   }//else
}//mutter


/* By Shamu */
void cough(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You politely cough at your own actions.\n", pc);
         Sprintf(buf, "coughs politely to %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You cough politely at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S coughs politely at you.  Are you missing something?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S coughs in %S's direction..\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You cough politely.\n", pc);
      emote("coughs politely.", pc, rm, TRUE);
   }//else
}//cough


/* By Shamu */
void point(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You point at yourself.\n", pc);
         Sprintf(buf, "points accusingly at %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You point accusingly at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S points at you accusingly!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S points at %S.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You point at nowhere in particular.\n", pc);
      emote("points at nowhere in particular.", pc, rm, TRUE);
   }//else
}//point


/* By Shamu */
void snap(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You snap your fingers in front of your face.\n", pc);
         Sprintf(buf, "snaps at %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You snap your fingers in front of %S's face.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S snaps %s fingers in front of your face.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT), get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S tries to snap %S's neck.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You snap your fingers.\n", pc);
      Sprintf(buf, "snaps %s fingers.\n", get_his_her(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//snap


/* By Shamu */
void whine(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You whine to yourself.\n", pc);
         Sprintf(buf, "grins at %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You whine at %S.  Perhaps you'd like some cheese with that?\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S whines at you, claiming it's all your fault.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S whines at %S.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You whine at everyone!\n", pc);
      emote("whines at everyone!", pc, rm, TRUE);
   }//else
}//whine


/* By Shamu */
void bah(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You grin at yourself.\n", pc);
         Sprintf(buf, "makes sheep-like noises at %s.\n",
                 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You look at %S and exclaim, 'Bah!'.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S gets right up in your face and yells, 'Bah!' Weird, 'eh?\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S looks at %S and shouts, 'Bah!' in perfect Quadcaster style.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You exclaim, 'Bah!'\n", pc);
      emote("suddenly shouts, 'Bah!'.", pc, rm, TRUE);
   }//else
}//bah


/* By Shamu */
void punt(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You kick yourself in the behind!\n", pc);
         Sprintf(buf, "kicks %s in the bottom!.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You kick %S in the rear end, sending %s flying!.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT), get_him_her(*crit_ptr));
         show(buf, pc);
         Sprintf(buf, "%S runs up behind you and punts you in the behind!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S punts %S at some imaginary target.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You try and punt the air!\n", pc);
      emote("takes a big swing at the air and misses!  Ouch, that had to hurt.", pc, rm, TRUE);
   }//else
}//punt


/* By Shamu */
void yuck(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You look at yourself and say, 'Yuck.'.\n", pc);
         Sprintf(buf, "looks at %s and mutters, 'Yuck.'.\n",
                 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You look at %S and say, 'Yuck!'.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S lets you know just how disgusting you really are!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S tells everyone that %S is all yucky.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You look up to the sky and yell, 'Ew!  Yucky!'.\n", pc);
      emote("looks to the sky and yells, 'Ewwww!  Yucky!'.", pc, rm, TRUE);
   }//else
}//yuck


/* By Shamu */
void announce(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You let yourself know just who you are!\n", pc);
         Sprintf(buf, "introduces %s to no one in particular.\n",
                 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You announce to %S that you are here!.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S announces to you that he is here.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S announces himself to %S.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You announce yourself!\n", pc);
      emote("boldly lets everyone know just who he is.", pc, rm, TRUE);
   }//else
}//announce


/* By Shamu */
void greet(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   
   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);
      
      if (!crit_ptr) 
         show("You don't see that person.\n", pc);
      else if (pc.POS > POS_REST)
         show("You are not in a position to do that.\n", pc);
      else if (crit_ptr == &pc) {
         show("You wave hello to yourself.  Lonely?\n", pc);
         Sprintf(buf, "waves to %s.\n", get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You wave hello to %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S gives you a big welcome wave!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S waves in greetings to %S.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You greet everyone!\n", pc);
      emote("greets everyone around!", pc, rm, TRUE);
   }//else
}//greet


/* By everyone's favorite marine mammal */
void doze(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You doze off.\n", pc);
         Sprintf(buf, "dozes off, having bored %s to pieces.\n",
                 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You doze off whilst trying to pay attention to %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S dozes off while looking at you.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S dozes off in the middle of %S's boring diatribe.\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You begin to doze off.\n", pc);
      emote("lies down to take a nap.", pc, rm, TRUE);
   }//else
}//doze


/* Not by Shamu.  Definitely not by Shamu. */
void megawibble(int i_th, const String* vict, critter& pc, room& rm) {
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
         show("You turn yourself downside up and outside in and try to wibble at yourself!\n", 
              pc);
         Sprintf(buf, "twists all around and tries to wibble at %s!\n", 
                 get_hisself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You turn yourself inside out and upside down and MEGAWIBBLE at %S!!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S turns %s outside in and downside up and MEGAWIBBLES at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_himself_herself(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S rearranges %s in impossible ways and MEGAWIBBLES at %S!\n",
                       name_of_crit(pc, ptr->SEE_BIT), 
                       get_himself_herself(pc),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You turn yourself inside out and upside down and MEGAWIBBLE!\n", pc);
      emote("turns downside out and upside in and MEGAWIBBLES!", pc, rm, TRUE);
   }//else
}//mega





/* By kat_skan */
void social_jump(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   object* obj_ptr;

   if(vict->Strlen()) {
      if (pc.POS >= POS_SIT) {
	pc.show("You can't jump from this position.\n");
	return;
      }
      if( (obj_ptr=have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT, rm)) ) {

         Sprintf(buf, "You throw %S to the ground and start jumping up and down on it.\n",
                 long_name_of_obj(*obj_ptr, ~0));
         show(buf, pc);

         Sprintf(buf, "throws %S to the ground and starts jumping up and down on it.",
                 long_name_of_obj(*obj_ptr, ~0));
         pc.emote(buf);
      }
     
      else if((obj_ptr = ROOM.haveObjNamed(i_th, vict, pc.SEE_BIT))){
         Sprintf(buf, "You jump around on %S.\n", long_name_of_obj(*obj_ptr, ~0));
         pc.show(buf);
         Sprintf(buf, "jumps up and down on %S.", long_name_of_obj(*obj_ptr, ~0));
         pc.emote(buf);
      }
      else pc.show(CS_DONT_SEEM_TO_HAVE_THAT);
   } else if (pc.POS >= POS_SIT) {
	pc.show("You can't jump from this position.\n");
   } else {
      pc.show("You jump up and down in frustration.\n");
      pc.emote("jumps up and down in frustration.");
   }
}//social_jump

/* by shamu */
void lag(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.emote("How can you tell?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (crit_ptr == &pc) {
         show("You complain about the lag, your hitpoints, your mana...\n",
              pc);
         Sprintf(buf, "complains on and on about the lag.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You complain to %S about the lag.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         pc.show(buf);
         Sprintf(buf, "%S whines to you about the lag.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         crit_ptr->show(buf);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S whines about the lag to %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               ptr->show(buf);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      pc.show("You complain about the lag, your hitpoints, your mana...\n");
      pc.emote("complains on and on about the lag.");
   }//else
}//lag


/* by shamu */
void sigh(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You sigh in your sleep");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (crit_ptr == &pc) {
         show("You sigh dejectedly to yourself.\n", pc);
         Sprintf(buf, "sighs dejectedly to %s.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You sigh dejectedly to %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S sighs and looks pittifully in your direction.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S sighs dejectedly at %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You sigh dejectedly.\n", pc);
      emote("sighs dejectedly.\n", pc, rm, TRUE);
   }//else
}//sigh


/* by Grock */
void pout(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You pout in your sleep, but no one seems to notice.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (crit_ptr == &pc) {
         show("You pout pitifully to yourself.\n", pc);
         Sprintf(buf, "pouts to %s.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You pout to %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S pouts at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S pouts at %S.\n",
                       name_of_crit(pc, ptr->SEE_BIT),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You pout pitifully.\n", pc);
      emote("pouts pitifully.\n", pc, rm, TRUE);
   }//else
}//pout


/* by Grock */
void bite(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You bite your tongue in your sleep, but no one seems to notice.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (crit_ptr == &pc) {
         show("You bite yourself on your arm.\n", pc);
         Sprintf(buf, "bites %s.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You bite %S on the arm!!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S bites you on the arm with vigor!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S bites %S on the arm playfully!\n",
                       name_of_crit(pc, ptr->SEE_BIT),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You bite a hand that feeds you..\n", pc);
      emote("bites a hand that feeds it.\n", pc, rm, TRUE);
   }//else
}//bite


/* by Grock */
void nibble(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You nibble your fingers in your sleep, but no one seems to notice.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (crit_ptr == &pc) {
         show("You nibble delicately on your fingers.\n", pc);
         Sprintf(buf, "nibles on %s fingers.\n", get_his_her(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You nibble on %S about 8 inches below %s left ear!!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT), get_his_her(*crit_ptr));
         show(buf, pc);
         Sprintf(buf, "%S nibbles about 8 inches below your left ear!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S nibbles %S gently, embarrassing all around!!\n",
                       name_of_crit(pc, ptr->SEE_BIT),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You nibble on yourself pittifully.\n", pc);
      emote("bites nibbles pittifully all alone.\n", pc, rm, TRUE);
   }//else
}//nibble


/* by Grock */
void chomp(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You chomp on your leg, at least you think you do!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (crit_ptr == &pc) {
         show("You chomp on your leg.\n", pc);
         Sprintf(buf, "chomps %s on the leg.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You chomp %S on the leg!!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S takes a big chomp out of your leg!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S takes a big chomp out of the leg of %S!\n",
                       name_of_crit(pc, ptr->SEE_BIT),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("You chomp the thin air.\n", pc);
      emote("looks around for something to chomp.\n", pc, rm, TRUE);
   }//else
}//chomp


void rofl(int i_th, const String* vict, critter& pc, room& rm) {
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
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         pc.show("You would probably want to be standing to do that.\n");
      else if (crit_ptr == &pc) {
         pc.show("You roll on the floor, laughing hysterically!\n");
         pc.emote("rolls on the floor, laughing hysterically!\n");
      }//if targ and agg is same
      else {
         Sprintf(buf, "You roll on the floor laughing your guts out at %S!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         pc.show(buf);
         Sprintf(buf, "%S rolls on the floor laughing at you!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         crit_ptr->show(buf);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S rolls on the floor laughing at %S!\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               ptr->show(buf);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      pc.show("You roll on the floor laughing hysterically!\n");
      pc.emote("rolls on the floor laughing hysterically!");
   }//else
}//rofl


/* shamu */
void insane(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (vict->Strlen()) {
      critter* crit_ptr =
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         pc.show("You would probably want to be standing to do that.\n");
      else if (crit_ptr == &pc) {
         pc.show("Your sanity is slowly slipping away...\n");
         pc.pemote("sanity is slowly slipping away, right before your very eyes!\n");
      }//if targ and agg is same
      else {
         Sprintf(buf, "You whisper to %S to call the men in white coats.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         pc.show(buf);
         Sprintf(buf, "%S tells you to call the men in white coats.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         crit_ptr->show(buf);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, 
                 "%S whispers to %S to call the men in white coats.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               ptr->show(buf);
            }//if
         }//while
      }//else
   }//if a victim
   else {
      show("Your sanity is slowly slipping away...\n", pc);
      pc.pemote("sanity is slowly slipping away, right before your very eyes!");
   }//else
}//insane



/* Social By Acrwyn */
void flex(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You flex for all the %s fawning all over you.\n",
              pc.SEX==SEX_MALE?"women":
              (pc.SEX==SEX_FEMALE?"men":"beings of undisclosed gender"));
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You flex impressively for yourself.\n", pc);
         Sprintf(buf, "flexes impressively for %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("Perhaps you should try to wake first.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You hug yourself in a reassuring way.\n", pc);
         Sprintf(buf, "hugs %s warmly.\n",
                 get_himself_herself(pc)); 
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You can't poke yourself in the eye if your eyelids are closed!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You poke yourself ruefully.\n", pc);
         Sprintf(buf, "pokes %s in the eye.\n",
                 get_himself_herself(pc)); 
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

         while ((ptr = cll.next())) {
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


void wink(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("Ha ha ha ha! Nice try.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You wink slyly at yourself.\n", pc);
         Sprintf(buf, "winks at %s.\n",
                 get_himself_herself(pc)); 
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You wink slyly at %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S winks at you slyly.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S winks at %S with a sly grin.\n",
                 name_of_crit(pc, ptr->SEE_BIT),
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You wink.\n", pc);
      emote("winks.", pc, rm, TRUE);
   }//else
}//wink

void hop(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You twitch slightly in your sleep.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS != POS_STAND)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You hop in little circles around yourself.\n", pc);
         Sprintf(buf, "hops gleefully in little circles around %s.\n",
                 get_himself_herself(pc)); 
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You smile sleepily.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You smile happily to yourself.\n", pc);
         Sprintf(buf, "smiles happily at %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("Your stomach growls, perhaps you should get a midnight snack.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You growl quietly to yourself.\n", pc);
         Sprintf(buf, "growls menacingly at %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      Sprintf(buf,"You dance around with %s of your dreams!\n",
              pc.SEX==SEX_MALE?"woman":
              (pc.SEX==SEX_FEMALE?"man":"being of undisclosed gender"));
      pc.show(buf);
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You dance romantically with yourself.\n", pc);
         Sprintf(buf, "dances about merrily with %s.\n",
                 get_himself_herself(pc)); 
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("Thank the stars you finally fell asleep!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You thank yourself.\n", pc); 
         Sprintf(buf, "thanks %s profusely.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("Perhaps you'd better try that while you're awake.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You curtsey to yourself and nearly fall over.\n",
              pc); 
         Sprintf(buf, "curtseys gracefully to %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You know what they say about people who laugh in their sleep?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You laugh at yourself.  Silly you!\n", pc); 
         Sprintf(buf, "laughs loudly at %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You know what they say about people who laugh in their sleep?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You chuckle to yourself.\n", pc);
         Sprintf(buf, "chuckles to %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S chuckles at %S.\n", 
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You gag in your sleep.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You ACK and spit at yourself.\n", pc); 
         Sprintf(buf, "ACKS and spits on %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
      show("You ACK and spit.  ACK!  PTHHH!\n", pc);
      emote("ACKS and spits.  ACK!  PTHHH!", pc, rm, TRUE);
   }//else
}//ack

/* Social By Acrwyn */
void bounce(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You twitch slightly in your sleep.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS >= POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You bounce around yourself and get dizzy.\n", pc);
         Sprintf(buf, "bounces excitedly around %s.\n",
                 get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You bounce excitedly around %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S bounces around you excitedly!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You belch softly in your sleep.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You burp loudly at yourself.  PHEW!.\n", pc);
         Sprintf(buf, "burps loudly at %s.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You burp loudly at %S, sending %s reeling.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT), get_his_her(*crit_ptr));
         show(buf, pc);
         Sprintf(buf, "%S sends you reeling with a horrific burp!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
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
      emote("burps loudly and happily.", pc, rm, TRUE);
   }//else
}//burp

/* Social By Acrwyn */
void snicker(int i_th, const String* vict, critter& pc, room& rm)
{
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You know what they say about people who laugh in their sleep?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You snicker knowingly to yourself.\n", pc);
         Sprintf(buf, "snickers to %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You know what they say about people who scream in their sleep?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You scream in frustration at yourself.\n", pc);
         Sprintf(buf, "screams deafeningly at %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("Can't that wait until morning?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You panic at the thought of yourself.\n", pc);
         emote("panics and runs screaming in erratic circles.\n",
               pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You panic and run in circles around %S while screaming.\n", 
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S panics and runs in circles around you, screaming.\n",  
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);

         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S panics and runs screaming in circles around %S.\n",
                       name_of_crit(pc, ptr->SEE_BIT),
                       name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You panic, running about in circles and screaming.\n", pc);
      emote("panics, running about in circles and screaming.", pc, rm, TRUE);
   }//else
}//panic

/* Social By Acrwyn */
void grunt(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You grunt and roll over in your sleep.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You grunt to yourself.\n", pc);
         Sprintf(buf, "grunts to %s.\n", get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You ponder for a moment, and suddenly realize the meaning of life!  You should write it down in the morning!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         if (strcasecmp(*(pc.names.peekFront()), "levander") == 0) {
            show("You ARE a Ponder!!.\n", pc);
         }//if
         else {
            show("You ponder your own existence.\n", pc);
         }//else
         Sprintf(buf, "ponders %s.\n", get_himself_herself(pc));  
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You scratch sleepily.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You scratch yourself thoughtfully.\n", pc);
         Sprintf(buf, "scratches %s thoughtfully.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You dream of puppies, thousands of puppies! Oh god, they're coming this way! FOR THE LOVE OF GOD! KEEP THE PUPPIES AWAY!!!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You lick yourself. Mmmm.\n", pc);
         Sprintf(buf, "licks %s.\n", get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You know what they say about people who laugh in their sleep?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You cackle gleefully at yourself.\n", pc); 
         Sprintf(buf, "cackles gleefully at %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You know what they say about people who cry in their sleep?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
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

         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You dream of a simpler time.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr =
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr)
         show(DONT_SEE_PERSON, pc);
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

         while ((ptr = cll.next())) {
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
      Sprintf(buf, "flips up %s collar and does the disco!!", get_his_her(pc));
      pc.emote(buf);
   }//else
}//disco


void puke(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You're not going to appreciate waking up in that!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You puke on your shoes.\n", pc);
         Sprintf(buf, "pukes on %s.\n", get_himself_herself(pc));
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
         
         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("Better not, that would wake you up for sure!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You cheer yourself..  What a hoser!.\n", pc);
         Sprintf(buf, "cheers %s.\n", get_himself_herself(pc));
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
         
         while ((ptr = cll.next())) {
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
      Sprintf(buf, "throws up %s hands and gives a big cheer!!", get_his_her(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//cheer



void nod(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   critter* targ = NULL;

   if (pc.POS == POS_SLEEP) {
      pc.show("You've already nodded off.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You nod to yourself.\n", pc);
         Sprintf(buf, "nods to %s.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
         targ = &pc;
      }//if targ and agg is same
      else {
         targ = crit_ptr;
         Sprintf(buf, "You nod your head in agreement with %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S nods in agreement with you!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
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
      targ = NULL;
   }//else

   String cmd = "nod";
   if (targ) {
      rm.checkForProc(cmd, NULL_STRING, pc, targ->getIdNum());
   }
   else {
      rm.checkForProc(cmd, NULL_STRING, pc, 0);
   }
}//nod


void nog(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   critter* targ = NULL;

   if (pc.POS == POS_SLEEP) {
      pc.show("Quit that, damn it!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You nog sagely to yourself.\n", pc);
         Sprintf(buf, "nogs sagely to %s.\n", get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
         targ = &pc;
      }//if targ and agg is same
      else {
         targ = crit_ptr;
         Sprintf(buf, "You nog in sagely agreement with %S.\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S nogs in sagely agreement with you!!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S nogs in sagely agreement with %S.\n",
                 name_of_crit(pc, ptr->SEE_BIT), 
                 name_of_crit(*crit_ptr, ptr->SEE_BIT));
               buf.Cap();
               show(buf, *ptr);
            }//if
         }//while
      }//else
   }//if a victim
   else {      
      show("You nog sagely.\n", pc);
      emote("nogs sagely!!", pc, rm, TRUE);
      targ = NULL;
   }//else

//   String cmd = "nod";
//   if (targ) {
//      rm.checkForProc(cmd, NULL_STRING, pc, targ->getIdNum());
//   }
//   else {
//      rm.checkForProc(cmd, NULL_STRING, pc, 0);
//   }
}//nog

void grin(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   
   if (pc.POS == POS_SLEEP) {
      pc.show("You smile in your sleep?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
         rm.haveCritNamed(i_th, vict, pc);
      
      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You grin at yourself.\n", pc);
         Sprintf(buf, "grins at %s.\n", get_himself_herself(pc));
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
         
         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You dream you've won the Miss Universe pagent. Wave to your adoring public!?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You wave goodbye to yourself.\n", pc);
         Sprintf(buf, "waves goodbye to  %s.\n", get_himself_herself(pc));
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
         
         while ((ptr = cll.next())) {
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("And just how do you plan to pull that off?\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You attempt to bow to yourself.\n", pc);
         Sprintf(buf, "tries to bow to %s.\n", get_himself_herself(pc));
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
         
         while ((ptr = cll.next())) {
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
               ptr = mob_to_smob(*crit_ptr, rm.getRoomNum());
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
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("You scream profanities in your sleep!\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
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
         
         while ((ptr = cll.next())) {
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
             ptr = mob_to_smob(*crit_ptr, rm.getRoomNum());
           }//if
           do_domob_curse_proc(*ptr, pc);
         }//if
      }//else
   }//if a victim
   else {      
      show("You curse darkly at the world.", pc);
      Sprintf(buf, "curses the situation %s finds %s in.", get_he_she(pc),
              get_himself_herself(pc));
      emote(buf, pc, rm, TRUE);
   }//else
}//curse


void disdain(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("That might be more effective if you were awake.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_REST)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You wallow in self pity.\n", pc);
         Sprintf(buf, "turns a glowering eye upon %s.\n",
                 get_himself_herself(pc));
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

         while ((ptr = cll.next())) {
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




/*by Gingon*/
void thpppt(int i_th, const String* vict, critter& pc, room& rm) {
   String buf(100);
   Cell<critter*> cll(rm.getCrits());
   critter* ptr;

   if (pc.POS == POS_SLEEP) {
      pc.show("That might be more effective if you were awake.\n");
      return;
   }

   if (vict->Strlen()) {
      critter* crit_ptr = 
            rm.haveCritNamed(i_th, vict, pc);

      if (!crit_ptr) 
         show(DONT_SEE_PERSON, pc);
      else if (pc.POS > POS_SIT)
         show(NOT_IN_POSN, pc);
      else if (crit_ptr == &pc) {
         show("You try to stick your tongue out at yourself and thpppt!\n", pc);
         Sprintf(buf, "tries to sticks %s tongue out at %s and thpppt!\n",
         get_his_her(pc),
         get_himself_herself(pc));
         emote(buf, pc, rm, TRUE);
      }//if targ and agg is same
      else {
         Sprintf(buf, "You stick you tongue at %S and thpppt!\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, "%S sticks %s tongue out at you and thpppts!\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 get_his_her(pc));
         buf.Cap();
         show(buf, *crit_ptr);
         
         while ((ptr = cll.next())) {
            if ((ptr != &pc) && (ptr != crit_ptr)) {
               Sprintf(buf, "%S sticks %s tongue out at %S and thpppts!\"\n",
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
      
      show("You stick your tongue out and thpppt!\n", 
                pc);
        Sprintf(buf,"sticks %s tongue out, and thpppts!\n",
        get_his_her(pc));        
        emote(buf,pc,rm, TRUE);
   }//else
}//thpppt






