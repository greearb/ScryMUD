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

///**************************  olc.cc  ****************************///

#include "commands.h"
#include "misc.h"  
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "command5.h"
#include "misc2.h"  
#include <stdio.h>
#include "battle.h"
#include "classes.h"
#include "olc.h"
#include "olc2.h"
#include <PtrArray.h>
#include "vehicle.h"
#include "load_wld.h"


void start_olc(critter& pc) {
   pc.show(CS_OLC_INTRO);

   if (pc.isUsingClient()) {
      /** Last char sent to the client in any
       * given transaction should not be a closing tag, thus the space 
       * here.
       */
      show("<Phase 999> ", pc);
   }

   O_COUNT = 0; //start at the beginning
   pc.pc->mode = MODE_OLC; //turn on OLC for parsing

   String nname(*(name_of_crit(pc, ~0)));
   nname.Tolower();
   nname.Prepend("./World/OLC_DIR/");
   String ntst(nname);
   ntst.Append("/TST_OBJ");
   ofstream ofile(ntst);
   if (!ofile) {
      nname.Prepend("mkdir ");
      system(nname);
   }//if
}//olc

void restart_olc(critter& pc) {
   String buf(100);
   show("\n\nRe-entering OLC.\n", pc);
   pc.pc->mode = MODE_OLC;
   if (pc.pc) {
      Sprintf(buf, "<Phase %i> ", O_COUNT); 
      show(buf, pc);
   }
   show(olc_prompts[O_COUNT], pc);
}//restart_olc

void quit_olc(critter& pc) {
   show("Exiting OLC, type 'olc' to re-enter.\n", pc);
   pc.pc->mode = MODE_NORMAL;
}//quit_olc


void do_olc(critter& pc) {
   String string(100);
   String buf(100);
   const char* quitter = "quit_olc"; //will save a teensy bit of memory i think :)
   int i, j, z;
   String* tmp_str;
   stat_spell_cell *sp_ptr;
   short eos, is_done, tp;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In do_olc, O_COUNT is:  " << O_COUNT << ".\n";

      if (OLC_ROOM) {
         Sprintf(buf, "\nDEBUGGING:  OLC ROOM's id number: %i\n",
                 OLC_ROOM->getIdNum());
         pc.show(buf);
      }
   }

   if (!pc.doesOwnRoom(ROOM)) {
     show("You must be in your own zone in order to build.\n", pc);
     show("You will now be exited from OLC.  Go back to your zone!\n", pc);
     pc.pc->input = NULL_STRING; //discard all input
     quit_olc(pc);
   }//if
   
   switch (O_COUNT) {
      case 0: //decide which type of thing to create
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
               switch (i) {
               case 1:
                  O_COUNT = 1; // obj
                  Sprintf(string, "\n\nHow about using: %i.\n",
                          get_next_obj());
                  show(string, pc);
                  break;
               case 2:
                  O_COUNT = 15; // critter
                  Sprintf(string, "\n\nHow about using: %i.\n",
                          get_next_mob());
                  show(string, pc);
                  break;
               case 3:
                  if (!ROOM.isZlocked()) 
                     show("You must first LOCK the zone.\n", pc);
                  else if (!ROOM.isTotalLoaded())  //if not totally_read
                     show("You must first 'total_rezone' to read ALL inv.\n",
                          pc);
                  else 
                    O_COUNT = 38; // room
                  break;
               case 4:
                  O_COUNT = 47; // door
                  Sprintf(string, "\n\nHow about using: %i.\n",
                          get_next_door());
                  show(string, pc);
                  break;
               case 5: 
                  quit_olc(pc);
                  reset_olc(pc);
                  break;
               case 6:
                  if (!ROOM.isZlocked()) 
                     show("You must first LOCK the zone.\n", pc);
                  else if (!ROOM.isTotalLoaded())  //if not totally_read
                     show("You must first 'total_rezone' to read ALL inv.\n",
                          pc);
                  else 
                     O_COUNT = 54; //vehicle
                  break;
               default:
                  break;
            }//switch i
         }//if
         else if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            reset_olc(pc); //deals with locked flag (!write)
            break;
         }//if
         else {
            show("You need to enter a number here.\n", pc);
         }//else
         break;


//**********************************************************************//
///************************  object creation  *************************///
//**********************************************************************//

      case 1:   //obj number
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
                if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
               i = get_next_obj();
            }
            else if (obj_list[i].OBJ_FLAGS.get(10)) {
               i = get_next_obj(); //choose automatically fer them
            }
               
            if (!obj_list[i].isInUse()) {
               OLC_OBJ = &(obj_list[i]); 
               OLC_OBJ->OBJ_NUM = i; //assign obj_num
               OLC_OBJ->OBJ_FLAGS.turn_on(10);
               OLC_OBJ->setIncomplete();
               O_COUNT = 2;
            }//if
            else {
               show("OOps, you tried to write over an existing obj!!\n",
                    pc);
               Sprintf(string, "How about using: %i.\n", get_next_obj());
               show(string, pc);
               break;
            }//else
         }//if its a number
         else if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
            show("A number is needed here...\n", pc);
         }//else
         break;

      case 2:                /* names */
         eos = FALSE;
         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0) {
               break;
            }//if
            if (isnum(string)) {
               show("Numbers don't work so well for names...\n", pc);
               break;
            }//if
            else {
               if (string == quitter) {
                  quit_olc(pc);  //removes bit, outputs txt
                  break;
               }//if
            }//else
            j = string.Strlen();
            if (!check_l_range(j, 0, 45, pc, FALSE)) {
               show("The name must be less than 45 characters.\n", pc);
               break;
            }//if
            else {
               if (string == "~") {
                  if (!OLC_OBJ->names.isEmpty())
                       O_COUNT = 3;
               }//if
               else {
                  tmp_str = new String(string);
                  OLC_OBJ->names.append(tmp_str);
               }//else
            }//else
         }//while
         break;

      case 3:         /* short desc */
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (isnum(string)) {
            show("A number for a short desc??\n", pc);
            break;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
         }//else
         if (!check_l_range(j, 0, 79, pc, FALSE)) {
            show("The short_desc should be less than 80 characters.\n", pc);
            break;
         }//if
         else {
            (OLC_OBJ->short_desc) = string;
            O_COUNT = 4; 
         }//else
         break;

      case 4:                /* in room desc */
         string = pc.pc->input.Get_Rest();
         if ((j = string.Strlen()) == 0)
            break;
         if (isnum(string)) {
            show("A number for an in_room_desc??\n", pc);
            break;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
         }//else
         if (!check_l_range(j, 0, 79, pc, FALSE)) {
            show("The in_room_desc must be less than 80 characters.\n", pc);
            break;
         }//if
         else {
            (OLC_OBJ->in_room_desc) = string;
            O_COUNT = 5; 
         }//else
         break;

      case 5:                /* long desc */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
            if (OLC_OBJ->long_desc.Strlen() > 0) {
                  O_COUNT = 6;
                break;
            }//if
         }//if
         else {
            (OLC_OBJ->long_desc) += string;
            (OLC_OBJ->long_desc) += "\n"; 
         }//else
        }//while TRUE
         break;

      case 6:  //obj flags
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  O_COUNT = 7; //move on to next
                  break;
               }//if
               if (check_l_range(i, 0, 250, pc, TRUE)) {
                  if ((i == 10) || (i == 70) || (i == 71)) {
                     pc.show("Can't set that flag.");
                  }//if
                  else {
                     OLC_OBJ->OBJ_FLAGS.flip(i);
                  }
               }//if
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
     
         OLC_OBJ->OBJ_FLAGS.turn_off(67); //no components here
         OLC_OBJ->OBJ_FLAGS.turn_off(68); //no concoction here
         //OLC_OBJ->OBJ_FLAGS.turn_off(72); //not used

         if (OLC_OBJ->obj_flags.get(59)) {
            OLC_OBJ->obj_flags.turn_on(54); //canteens are containers
         }//if
         if (OLC_OBJ->obj_flags.get(64)) {
            OLC_OBJ->obj_flags.turn_on(54); //toolboxes are containers
         }//if

         if (OLC_OBJ->obj_flags.get(40)) {
            OLC_OBJ->obj_flags.turn_off(57);
         }
         if (OLC_OBJ->obj_flags.get(57)) {
            OLC_OBJ->obj_flags.turn_off(40);
         }
         if ((OLC_OBJ->obj_flags.get(52)) ||
             (OLC_OBJ->obj_flags.get(51)) ||
             (OLC_OBJ->obj_flags.get(53))) { //wands, potions, scrolls

            OLC_OBJ->obj_flags.turn_on(63); //potions have spec_proc_data
         }
         if (OLC_OBJ->obj_flags.get(65)) {
            OLC_OBJ->obj_flags.turn_on(54); //cauldrons are containers
         }//if
         if (OLC_OBJ->obj_flags.get(73)) {
            OLC_OBJ->obj_flags.turn_on(54); //vend machines are containers
         }//if
         if (OLC_OBJ->obj_flags.get(74)) {
            OLC_OBJ->obj_flags.turn_on(54); //bulletin boards are bags
         }//if
         show("These flags are set so far:\n", pc);
         out_field(OLC_OBJ->obj_flags, pc, OBJ_FLAGS_NAMES);
         break;

      case 7:  //extras
         z = 0;
         is_done = FALSE;
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
   
            if (string.Strlen() == 0)
               break;         

              if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (z == 3) {
                  OLC_OBJ->setCurInGame(0);
                  z++;
               }//if
               OLC_OBJ->extras[z] = i;
               z++;
               if (z >= OBJ_MAX_EXTRAS) { //only way to get out!
                  O_COUNT = 8; //move on to next
                  is_done = TRUE;
                  break;
               }//if
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         if (!is_done)
            show("You didn't manage to enter the extras correctly..\n", pc);
         break;
         
      case 8:        //cur_stats:  vis_bit, price, in_zone
                    /* vis_bit */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            OLC_OBJ->OBJ_VIS_BIT = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

                      /* price */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            OLC_OBJ->PRICE = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

         i = ROOM.getZoneNum();
         if (!check_l_range(i, 0, NUMBER_OF_ZONES, pc, FALSE)) {
           mudlog.log(LS_ERROR, "ERROR:  zone out of range, case 8, olc.\n");
           i = 10; //set to practice zone
         }//if

         /* in zone, set by game */
         OLC_OBJ->OBJ_IN_ZONE = ROOM.getZoneNum();

                        /* check if we should do bag next */
         if (OLC_OBJ->isContainer()) {
            O_COUNT = 9; //read in a bag
         }//if
         else {
            O_COUNT = 11; // skip to affected by
         }//else
         break;

      case 9: //bag  flags
         if (!OLC_OBJ->bag) {
            OLC_OBJ->bag = new bag_struct;
         }//if
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

             if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if
               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  O_COUNT = 10; //move on to next
                  break;
               }//if
               if (check_l_range(i, 0, 100, pc, TRUE))
                  OLC_OBJ->bag->bag_flags.flip(i);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         out_field(OLC_OBJ->bag->bag_flags, pc, BAG_FLAGS_NAMES);
         break;

      case 10:  //other bag data, numbers only
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
               break;
            OLC_OBJ->bag->key_num = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            else {
               show(HUH_MSG, pc);
            }//else
            break;
         }//else

         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 32000, pc, TRUE))
               break;
            OLC_OBJ->bag->max_weight = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            else {
               show(HUH_MSG, pc);
            }//else
            break;
         }//else

         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 32000, pc, TRUE))
               break;
            OLC_OBJ->bag->percentage_weight = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            else {
               show(HUH_MSG, pc);
            }//else
            break;
         }//else

         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, -1, 32000, pc, TRUE))
               break;
            OLC_OBJ->bag->time_till_disolve = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            else {
               show(HUH_MSG, pc);
            }//else
            break;
         }//else
         O_COUNT = 11; //done w/bag
         break;

      case 11: //affected_by
         j = TRUE; //is first
         eos = FALSE;
         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("Need a number here.\n", pc);
               break;
            }//if
            i = atoi(string);

            if (j == TRUE) { //ie read in first in pair
               if (i == -1) {
                 if (OLC_OBJ->obj_flags.get(54)) { //if container
                   O_COUNT = 12; //do inventory
                 }//if
                 else {
                   O_COUNT = 13; //stat_affects
                 }//else
               }//if
               else {
                 sp_ptr = new stat_spell_cell;
                 sp_ptr->stat_spell = i;
                 sp_ptr->bonus_duration = 0; //init just in case
                 OLC_OBJ->affected_by.prepend(sp_ptr);
                 j = FALSE; //now looking for the second
               }//else
            }//if
            else {
              sp_ptr = OLC_OBJ->affected_by.peekFront();
              sp_ptr->bonus_duration = i;
              j = TRUE; //once again looking for first..
            }//if
         }//while         
         break;

      case 12:        //inventory
         eos = FALSE;
         while (!eos) {
             string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("You need to input a number here.\n", pc);
               break;
            }//if
            i = atoi(string);
            if (!check_l_range(i, -1, NUMBER_OF_ITEMS, pc, TRUE)) {
               break;
            }//if
            else {
               if (i == -1) {
                  O_COUNT = 13;
               }//if
               else if (obj_list[i].OBJ_FLAGS.get(10)) {
                  OLC_OBJ->gainInv(&(obj_list[i]));
               }//else
               else {
                  show("That object has not been created yet.\n", pc);
               }//else
            }//else
         }//while         
         break;

      case 13: //stat affects
         j = TRUE; //is first
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("Need a number here.\n", pc);
               break;
            }//if
            i = atoi(string);

            if (j == TRUE) { //ie read in first in pair
               if (i == -1) {
                  if (OLC_OBJ->OBJ_FLAGS.get(63)) { //if has spec data
                     O_COUNT = 14;
                  }//if
                  else {
                     finish_olc_obj(pc);
                     return;
                  }//else
               }//if
               else {
                  sp_ptr = new stat_spell_cell;
                  sp_ptr->stat_spell = i;
                  sp_ptr->bonus_duration = 0; //init just in case
                  OLC_OBJ->stat_affects.prepend(sp_ptr);
                  j = FALSE; //now looking for the second
               }//else
            }//if
            else {
               sp_ptr = OLC_OBJ->stat_affects.peekFront();
               sp_ptr->bonus_duration = i;
               j = TRUE; //once again looking for first..
            }//if
         }//while         
         break;
       
      case 14:  //obj spec data flags
         eos = FALSE;

         if (!(OLC_OBJ->obj_proc)) {
            OLC_OBJ->obj_proc = new obj_spec_data;
         }//if

         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  if (OLC_OBJ->OBJ_SPEC_FLAGS.get(2)) {
                     O_COUNT = 78; // go do get_skin
                     return;
                  }//if
                  else {
                     O_COUNT = 79;
                     return;
                  }//else
               }//if
               OLC_OBJ->OBJ_SPEC_FLAGS.flip(i);
               OLC_OBJ->OBJ_SPEC_FLAGS.turn_off(1);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         out_field(OLC_OBJ->OBJ_SPEC_FLAGS, pc, OBJ_SPEC_DATA_FLAGS_NAMES);
         break;


//************************************************************************//
///********************  Here begins critter creation *******************///
//************************************************************************//


      case 15:   //crit number
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
                if (!check_l_range(i, 0, NUMBER_OF_MOBS, pc, TRUE)) {
               i = get_next_mob();
            }
            else if (mob_list[i].CRIT_FLAGS.get(18)) {
               i = get_next_mob();
            }
               
            if (!mob_list[i].CRIT_FLAGS.get(18)) {
               mob_list[i].CRIT_FLAGS.turn_on(18);  //now in use
               mob_list[i].setNotComplete();

               OLC_MOB = &(mob_list[i]); 
               if (!OLC_MOB->mob) {
                  OLC_MOB->mob = new mob_data;
               }//if
               OLC_MOB->setIdNum(i); //assign obj_num
               O_COUNT = 16;
            }//if
            else {
               show("OOps, you tried to write over an existing mob!!\n", pc);
               Sprintf(string, "How about using: %i.\n", get_next_mob());
               show(string, pc);
               break;
            }//else
         }//if its a number
         else if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
            show("A number is needed here...\n", pc);
         }//else
         break;

      case 16:                /* names */
         eos = FALSE;
         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0) {
               break;
            }//if
            if (isnum(string)) {
               show("Numbers don't work so well for names...\n", pc);
               break;
            }//if
            else {
               if (string == quitter) {
                  quit_olc(pc);  //removes bit, outputs txt
                  break;
               }//if
            }//else
            j = string.Strlen();
            if (!check_l_range(j, 0, 45, pc, FALSE)) {
               show("The name must be less than 45 characters.\n", pc);
               break;
            }//if
            else {
               if (string == "~") {
                  if (!OLC_MOB->names.isEmpty())
                      O_COUNT = 17;
               }//if
               else {
                  tmp_str = new String(string);
                  OLC_MOB->names.append(tmp_str);
               }//else
            }//else
         }//while
         break;

      case 17:         /* short desc */
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (isnum(string)) {
            show("A number for a short desc??\n", pc);
            break;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
         }//else
         if (!check_l_range(j, 0, 79, pc, FALSE)) {
            show("The short_desc should be less than 80 characters.\n", pc);
            break;
         }//if
         else {
            (OLC_MOB->short_desc) = string;
            O_COUNT = 18; 
         }//else
         break;

      case 18:                /* in room desc */
         string = pc.pc->input.Get_Rest();
         if ((j = string.Strlen()) == 0)
            break;
         if (isnum(string)) {
            show("A number for an in_room_desc??\n", pc);
            break;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
         }//else
         if (!check_l_range(j, 0, 79, pc, FALSE)) {
            show("The in_room_desc must be less than 80 characters.\n", pc);
            break;
         }//if
         else {
            (OLC_MOB->in_room_desc) = string;
            O_COUNT = 19; 
         }//else
         break;

      case 19:                /* long desc */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
            O_COUNT = 20;
            break;
         }//if
         else {
            (OLC_MOB->long_desc) += string;
            (OLC_MOB->long_desc) += "\n"; 
         }//else
        }//while TRUE
         break;

      case 20:  //crit flags
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  O_COUNT = 21; //move on to next
                  break;
               }//if
               if (!check_l_range(i, 0, 250, pc, TRUE))
                  break;
               if (i != 18) //in_use should not be changeable
                  OLC_MOB->crit_flags.flip(i);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         out_field(OLC_MOB->crit_flags, pc, CRIT_FLAGS_NAMES);
         break;

      case 21:  //long data
         z = 0;
         is_done = FALSE;
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
   
            if (string.Strlen() == 0)
               break;         

              if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (!check_l_range(i, 0, 123456789, pc, TRUE))
                  break; 
               OLC_MOB->long_data[z] = i;
               z++;
               if (z >= MOB_LONG_DATA) { //only way to get out!
                  O_COUNT = 22; //move on to next
                  is_done = TRUE;
                  OLC_MOB->long_data[1] *= config.experienceDivisor;
                  break;
               }//if
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         if (!is_done)
            show("You didn't manage to enter the data correctly..\n", pc);
         break;

      case 22:        //short_cur_stats:, will enter 15 at a time, 0-14
         z = 0;
         is_done = FALSE;
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
   
            if (string.Strlen() == 0)
               break;         

              if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               OLC_MOB->short_cur_stats[z] = i;
               z++;
               if (z > 14) { //only way to get out!
                  O_COUNT = 23; //move on to next
                  is_done = TRUE;
                  pc.pc->input.clear();
                  break;
               }//if
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         if (!is_done)
            show("You didn't manage to enter the data correctly..\n", pc);
         break;

      case 23:        //short_cur_stats:, will enter 15 at a time, 15-29
         z = 15;
         is_done = FALSE;
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
   
            if (string.Strlen() == 0)
               break;         

              if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               OLC_MOB->short_cur_stats[z] = i;
               z++;
               if (z > 29) { //only way to get out!
                  O_COUNT = 24; //move on to next
                  is_done = TRUE;
                  pc.pc->input.clear();
                  break;
               }//if
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         if (!is_done)
            show("You didn't manage to enter the data correctly..\n", pc);
         break;

      case 24:        //short_cur_stats:, 30-END
         z = 30;
         is_done = FALSE;
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
   
            if (string.Strlen() == 0)
               break;         

              if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               OLC_MOB->short_cur_stats[z] = i;
               z++;
               if (z >= MOB_SHORT_CUR_STATS) { //only way to get out!
                  i = ROOM.getZoneNum();
                  if (!check_l_range(i, 0, NUMBER_OF_ZONES, pc, FALSE)) {
                    mudlog.log(WRN,
                               "ERROR:  zone# out of range, case 24, olc.\n");
                    i = 10; //goes to test zone upon error
                  }//if

                  OLC_MOB->setNativeZoneNum(i); //doesn't matter WHAT they enter!

                  O_COUNT = 25; //move on to next
                  pc.pc->input.clear();
                  is_done = TRUE;
                  break;
               }//if
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         if (!is_done)
            show("You didn't manage to enter the data correctly..\n", pc);
         break;

      case 25: //cur_stats
         z = 0;
         is_done = FALSE;
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
   
            if (string.Strlen() == 0)
               break;         

              if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               OLC_MOB->cur_stats[z] = i;
               z++;
               if (z >= MOB_CUR_STATS) { //only way to get out!
                  O_COUNT = 26; //move on to next
                  is_done = TRUE;
                  break;
               }//if
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         if (!is_done)
            show("You didn't manage to enter the data correctly..\n", pc);
         break;


      case 26: //eq
         j = TRUE; //is first
         eos = FALSE;
         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("Need a number here.\n", pc);
               break;
            }//if
            i = atoi(string);
            if (!check_l_range(i, -1, NUMBER_OF_ITEMS, pc, TRUE)) {
               break;
            }//if
            else {
               if (i == -1) {
                  if (j == TRUE) { //ie read in first in pair; item#
                     O_COUNT = 27; //affected_by
                  }//if
                  else {
                     show("You entered a -1 at the wrong place.\n", pc);
                     break;
                  }//else
               }//if
               else {
                  if (j == TRUE) { //ie read in first of the pair
                     if (!obj_list[i].OBJ_FLAGS.get(10)) {
                        show("That object has not been created yet.\n", pc);
                        break;
                     }//if
                     TEMP_OLC_INT = i;  //save obj num for next iteration
                     j = FALSE; //now looking for the second
                  }//if
                  else {
                            if ((!check_l_range(i, -1, MAX_EQ, pc, TRUE))
                         || (i == 0)) {
                        break;
                     }//if
                     OLC_MOB->EQ[i] = &(obj_list[TEMP_OLC_INT]);
                     j = TRUE; //once again looking for first..
                  }//else
               }//else
            }//else
         }//while         
         break;

      case 27: //affected_by
         j = TRUE; //is first
         eos = FALSE;
         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("Need a number here.\n", pc);
               break;
            }//if
            i = atoi(string);
            if (!check_l_range(i, -1, 32000, pc, TRUE)) {
               break;
            }//if
            else {
              if (j == TRUE) { //ie read in first in pair
                if (i == -1) {
                  O_COUNT = 28;
                }//if
                else {
                  sp_ptr = new stat_spell_cell;
                  sp_ptr->stat_spell = i;
                  sp_ptr->bonus_duration = 0; //init just in case
                  OLC_MOB->affected_by.prepend(sp_ptr);
                  j = FALSE; //now looking for the second
                }//else
              }//if
              else {
                sp_ptr = OLC_MOB->affected_by.peekFront();
                sp_ptr->bonus_duration = i;
                j = TRUE; //once again looking for first..
              }//if
            }//else
         }//while         
         break;

      case 28:        //inventory
         eos = FALSE;
         while (!eos) {
             string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("You need to input a number here.\n", pc);
               break;
            }//if
            i = atoi(string);
            if (!check_l_range(i, -1, NUMBER_OF_ITEMS, pc, TRUE)) {
               break;
            }//if
            else {
               if (i == -1) {
                  O_COUNT = 29;
               }//if
               else if (obj_list[i].OBJ_FLAGS.get(10)) {
                  OLC_MOB->gainInv(&(obj_list[i]));
               }//else
               else {
                  show("That object has not been created yet.\n", pc);
               }//else
            }//else
         }//while         
         break;

      case 29: //mob data, max_in_game
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if
         i = atoi(string);
             if (!check_l_range(i, 0, 300, pc, TRUE))
              break;
         OLC_MOB->setMaxInGame(i);

         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if
         i = atoi(string);
             if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
              break;
         OLC_MOB->mob->skin_num = i;

         if (i != 0) 
            OLC_MOB->MOB_FLAGS.turn_on(16);
         else
            OLC_MOB->MOB_FLAGS.turn_off(16);

         O_COUNT = 30;
         break;

       
      case 30:  //mob data flags
         eos = FALSE;

         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if

            if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  if (OLC_MOB->mob->mob_data_flags.get(0)) {
                     O_COUNT = 31; //proc_data
                     break;
                  }//if
                  else { 
                     if (OLC_MOB->mob->proc_data) {
                        delete OLC_MOB->mob->proc_data;
                        OLC_MOB->mob->proc_data = NULL;
                     }//if
                     finish_olc_mob(pc); //all done
                     return;
                  }//else
               }//if
               if (!check_l_range(i, 0, 500, pc, TRUE)) {
                  break;
               }//if
               if (((i >= 0) && (i <= 2)) || ((i >= 5) && (i <= 8)) || (i == 10)) {
                  OLC_MOB->mob->mob_data_flags.flip(i);
               }
               else {
                  pc.show("You cannot change that flag here.\n");
               }
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         out_field(OLC_MOB->mob->mob_data_flags, pc, MOB_DATA_FLAGS_NAMES);
         break;
      
      case 31: //spec_data: int1
         if (!OLC_MOB->mob->proc_data) {
            OLC_MOB->mob->proc_data = new spec_data;
         }//if

         /* int1 */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if
         i = atoi(string);
             if (!check_l_range(i, 0, 32000, pc, TRUE))
              break;
         OLC_MOB->mob->proc_data->int1 = i;

         /* skill_violence */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if

         i = atoi(string);
             if (!check_l_range(i, -10, 10, pc, TRUE))
              break;
         OLC_MOB->mob->setSkillViolence(i);

         /* agg_benevolence */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if

         i = atoi(string);
             if (!check_l_range(i, -10, 10, pc, TRUE))
              break;
         OLC_MOB->mob->setBenevolence(i);

         /* defensiveness */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if

         i = atoi(string);
             if (!check_l_range(i, -10, 10, pc, TRUE))
              break;
         OLC_MOB->mob->setDefensiveness(i);

         /* bad_assedness */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if

         i = atoi(string);
             if (!check_l_range(i, -10, 10, pc, TRUE))
              break;
         OLC_MOB->mob->setBadAssedness(i);

         /* social_awareness */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if

         i = atoi(string);
             if (!check_l_range(i, -10, 10, pc, TRUE))
              break;
         OLC_MOB->mob->setSocialAwareness(i);

         O_COUNT = 32; /* we made it!! */
         break;


      case 32: //mob: proc_data: flag1
         if (!(OLC_MOB->mob->proc_data)) {
            OLC_MOB->mob->proc_data = new spec_data;
         }//if

         eos = FALSE;

         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  if (OLC_MOB->FLAG1.get(1)) {
                     O_COUNT = 33; //shopkeeper
                     break;
                  }//if
                  else {
                     delete OLC_MOB->mob->proc_data->sh_data;
                     OLC_MOB->mob->proc_data->sh_data = NULL;
                  }//else
                  if (OLC_MOB->FLAG1.get(2)) {
                     O_COUNT = 36; //teacher
                     break;
                  }//if
                  else {
                     delete OLC_MOB->mob->proc_data->teach_data;
                     OLC_MOB->mob->proc_data->teach_data = NULL;
                  }//else
                  if (OLC_MOB->FLAG1.get(5)){ // give proc
                    O_COUNT = 55;
                    break;
                  }//if
                  else {
                     delete OLC_MOB->mob->proc_data->give_proc;
                     OLC_MOB->mob->proc_data->give_proc = NULL;
                  }//else
                  if (OLC_MOB->FLAG1.get(6)) {// say proc
                    O_COUNT = 56;
                    break;
                  }//if
                  else {
                     OLC_MOB->mob->proc_data->topics.clearAndDestroy();
                  }//else
                  if (OLC_MOB->FLAG1.get(7)) { // bow proc
                    O_COUNT = 57;
                    break;
                  }//if
                  else {
                     delete OLC_MOB->mob->proc_data->bow_proc;
                     OLC_MOB->mob->proc_data->bow_proc = NULL;
                  }//else
                  if (OLC_MOB->FLAG1.get(8)) { // curse proc
                    O_COUNT = 58;
                    break;
                  }//if
                  else {
                     delete OLC_MOB->mob->proc_data->curse_proc;
                     OLC_MOB->mob->proc_data->curse_proc = NULL;
                  }//else

                  finish_olc_mob(pc);
                  return;
               }//if
                   if (!check_l_range(i, 0, 500, pc, TRUE)) {
                    break;
               }//if
               OLC_MOB->mob->proc_data->flag1.flip(i);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         out_field(OLC_MOB->FLAG1, pc, MOB_PROC_DATA_FLAGS_NAMES);
         break;

      case 33: //shopkeeper: markup, buy%
         if (!OLC_MOB->mob->proc_data->sh_data) {
            OLC_MOB->mob->proc_data->sh_data = new shop_data;
         }//if

                        /* markup */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 1, 32000, pc, TRUE))
               break;
            OLC_MOB->MARKUP = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

                        /* buy percentage */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 1, 32000, pc, TRUE))
               break;
            OLC_MOB->BUY_PERCENTAGE = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

                        /* open time */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 23, pc, TRUE))
               break;
            OLC_MOB->OPEN_TIME = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

                        /* close time */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 23, pc, TRUE))
               break;
            OLC_MOB->CLOSE_TIME = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

         O_COUNT = 34; //shop_data_flags
         break;

      case 34: //shop_data_flags
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  O_COUNT = 35; //shopkeeper: perm_inv
                  break;
               }//if
                   if (!check_l_range(i, 0, 500, pc, TRUE)) {
                    break;
               }//if
               OLC_MOB->SHOP_DATA_FLAGS.flip(i);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         out_field(OLC_MOB->SHOP_DATA_FLAGS, pc, SHOP_DATA_FLAGS_NAMES);
         break;

      case 35: //sh_data: perm_inv
         eos = FALSE;
         while (!eos) {
             string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("You need to input a number here.\n", pc);
               break;
            }//if
            i = atoi(string);
            if (!check_l_range(i, -1, NUMBER_OF_ITEMS, pc, TRUE)) {
               break;
            }//if
            else {
               if (i == -1) {
                  if (OLC_MOB->FLAG1.get(2)) 
                       O_COUNT = 36; //teach_data
                  else if (OLC_MOB->FLAG1.get(5)) // give proc
                    O_COUNT = 55;
                  else if (OLC_MOB->FLAG1.get(6)) // say proc
                    O_COUNT = 56;
                  else if (OLC_MOB->FLAG1.get(7)) // bow proc
                    O_COUNT = 57;
                  else if (OLC_MOB->FLAG1.get(8)) // curse proc
                    O_COUNT = 58;
                  else {
                    finish_olc_mob(pc);
                    return;
                  }
               }//if
               else if (obj_list[i].OBJ_FLAGS.get(10)) {
                  OLC_MOB->PERM_INV.append(&(obj_list[i]));
               }//else
               else {
                  show("That object has not been created yet.\n", pc);
               }//else
            }//else
         }//while         
         break;

      case 36: //teach_data: teach_data_flags
         if (!OLC_MOB->mob->proc_data->teach_data) {
            OLC_MOB->mob->proc_data->teach_data = new teacher_data;
         }//if
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  if (OLC_MOB->FLAG1.get(5)) // give proc
                    O_COUNT = 55;
                  else if (OLC_MOB->FLAG1.get(6)) // say proc
                    O_COUNT = 56;
                  else if (OLC_MOB->FLAG1.get(7)) // bow proc
                    O_COUNT = 57;
                  else if (OLC_MOB->FLAG1.get(8)) // curse proc
                    O_COUNT = 58;
                  else {
                    finish_olc_mob(pc);
                    return;
                  }
               }//if
                   if (!check_l_range(i, 0, 500, pc, TRUE)) {
                    break;
               }//if
               OLC_MOB->TEACH_DATA_FLAGS.flip(i);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         out_field(OLC_MOB->TEACH_DATA_FLAGS, pc, TEACH_DATA_FLAGS_NAMES);
         break;

      case 37: //was construct data
         mudlog << "ERROR:  In case 37 of OLC." << endl;
         break;

///***********************************************************************///
///**********************  Room Construction  ****************************///
///***********************************************************************///

      case 38:   //room number
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            j = ROOM.getZoneNum();
            room* tmp_rm;

            if (j < 0) {
               mudlog << "ERROR:  zone num is negative." << endl;
               pc.show("Something is bad wrong, talk to your IMP.\n");
               quit_olc(pc);
               break;
            }

            mudlog << "OLC case 38, i:  " << i << "  j:  " << j
                   << endl;

                if ((!check_l_range(i, ZoneCollection::instance().elementAt(j).getBeginRoomNum(),
                                ZoneCollection::instance().elementAt(j).getEndRoomNum(),
                                pc, TRUE) ||
                 room_list[i].isInUse())) {
               tmp_rm = get_next_room(j);
               if (tmp_rm) {
                  i = tmp_rm->getIdNum();
               }
               else {
                  pc.show("There are no more rooms available in this zone.\n");
                  quit_olc(pc);
                  break;
               }
            }//if

            if (i <= 0) {
               pc.show("A bug has been encountered, tell the IMP.  Rclone should work though>\n");
               quit_olc(pc);
               break;
            }

            mudlog << "OLC:  using room#  " << i << endl;

            room_list[i].setFlag(23, TRUE);  //now in use
            room_list[i].setNotComplete();

            OLC_ROOM = &(room_list[i]); 
            OLC_ROOM->setRoomNum(i);

            Sprintf(buf, "Using room number %i\n", OLC_ROOM->getIdNum());
            pc.show(buf);

            O_COUNT = 39;
         }//if its a number
         else if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
            show("A number is needed here...\n", pc);
         }//else
         break;

      case 39:                /* names */
         eos = FALSE;
         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0) {
               break;
            }//if
            if (isnum(string)) {
               show("Numbers don't work so well for names...\n", pc);
               break;
            }//if
            else {
               if (string == quitter) {
                  quit_olc(pc);  //removes bit, outputs txt
                  break;
               }//if
            }//else
            j = string.Strlen();
            if (!check_l_range(j, 0, 45, pc, FALSE)) {
               show("The name must be less than 45 characters.\n", pc);
               break;
            }//if
            else {
               if (string == "~") {
                  if (!OLC_ROOM->names.isEmpty())
                       O_COUNT = 40;
               }//if
               else {
                  tmp_str = new String(string);
                  OLC_ROOM->names.append(tmp_str);
               }//else
            }//else
         }//while
         break;

      case 40:         /* short desc */
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (isnum(string)) {
            show("A number for a short desc??\n", pc);
            break;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
         }//else
         if (!check_l_range(j, 1, 79, pc, FALSE)) {
            show("The short_desc should be less than 80 characters.\n", pc);
            break;
         }//if
         else {
            OLC_ROOM->short_desc = string;
            O_COUNT = 41; 
         }//else
         break;

      case 41:                /* long desc */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
            if (OLC_ROOM->long_desc.Strlen() > 0) {
                  O_COUNT = 42;
               break;
            }//if
         }//if
         else {
            OLC_ROOM->long_desc += string;
            OLC_ROOM->long_desc += "\n"; 
         }//else
        }//while TRUE
         break;

      case 42:  //room flags
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  O_COUNT = 43; //move on to next
                  break;
               }//if
               if (!check_l_range(i, 0, 50, pc, TRUE))
                  break;
               if ((i != 22) && (i != 23) && (i != 29))
                          //in_use, total_loaded, used_in_track, !CHANGE
                  OLC_ROOM->flipFlag(i);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         out_field(OLC_ROOM->getFlags(), pc, ROOM_FLAGS_NAMES);
         break;

      case 43:  //cur stats
         z = 0;
         is_done = FALSE;
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
   
            if (string.Strlen() == 0)
               break;         

              if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (!check_l_range(i, 0, 2000000, pc, TRUE))
                  break; 
               if (z == 0)
                  OLC_ROOM->setVisBit(i);
               else if (z == 1)
                  OLC_ROOM->setMovCost(i);

               z++;
               if (z >= 2) { //only way to get out!
                  O_COUNT = 44; //move on to next
                  is_done = TRUE;
                  break;
               }//if
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         if (!is_done)
            show("You didn't manage to enter the data correctly..\n", pc);
         break;

      case 44: //affected_by
         j = TRUE; //is first
         eos = FALSE;
         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("Need a number here.\n", pc);
               break;
            }//if
            i = atoi(string);
            if (!check_l_range(i, -1, 32000, pc, TRUE)) {
               break;
            }//if
            else {
              if (j == TRUE) { //ie read in first in pair
                if (i == -1) {
                  O_COUNT = 45;
                }//if
                else {
                  sp_ptr = new stat_spell_cell;
                  sp_ptr->stat_spell = i;
                  sp_ptr->bonus_duration = 0; //init just in case
                  OLC_ROOM->affected_by.prepend(sp_ptr);
                  j = FALSE; //now looking for the second
                }//else
              }//if
              else {
                sp_ptr = OLC_ROOM->affected_by.peekFront();
                sp_ptr->bonus_duration = i;
                j = TRUE; //once again looking for first..
              }//if
            }//else
         }//while         
         break;

      case 45:        //inventory
         eos = FALSE;
         while (!eos) {
             string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("You need to input a number here.\n", pc);
               break;
            }//if
            i = atoi(string);
            if (!check_l_range(i, -1, NUMBER_OF_ITEMS, pc, TRUE)) {
               break;
            }//if
            else {
               if (i == -1) {
                  O_COUNT = 46;
               }//if
               else if (obj_list[i].OBJ_FLAGS.get(10)) {
                  OLC_ROOM->gainInv(&(obj_list[i]));
               }//else
               else {
                  show("That object has not been created yet.\n", pc);
               }//else
            }//else
         }//while         
         break;

      case 46: //choose direction to place room
            string = pc.pc->input.Get_Command(eos, tp);
         if (string.Strlen() == 0) {
            break;
         }//if
         if (!isnum(string)) {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            show("INPUT:  <distance> <direction>\n", pc);
            break;
         }//if
         j = atoi(string);
         if (!check_l_range(j, 0, 25, pc, TRUE)) {
            break;
         }//if
         /* distance is good to go now */

            string = pc.pc->input.Get_Command(eos, tp);
         if (string.Strlen() == 0) {
            break;
         }//if
         if (isnum(string)) {
            show("INPUT:  <distance> <direction>\n", pc);
            break;
         }//if
         else {

            OLC_ROOM->lock(); // make it locked
            OLC_ROOM->setTotalLoaded(TRUE); //make it totally_loaded

            if (door_to(OLC_ROOM->getRoomNum(), j, &string, pc) >= 0) {
               if (!OLC_ROOM->isVehicle()) {
                  finish_olc_room(pc);
               }
               else {
                  O_COUNT = 80;  //go do vehicle
               }
               
               break;
            }//if
         }//else

         break;


///***********************************************************************///
///**********************  Door Construction  ****************************///
///***********************************************************************///

      case 47:   //door number
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
                if (!check_l_range(i, 11, NUMBER_OF_DOORS, pc, TRUE)) {
               i = get_next_door();
            }//if

            if (door_list[i].door_data_flags.get(10)) {
               i = get_next_door();
            }
               
            if (!door_list[i].door_data_flags.get(10)) {
               OLC_DOOR = &(door_list[i]); 
               OLC_DOOR->door_data_flags.turn_on(10); //now its in use
               OLC_DOOR->setNotComplete();
               OLC_DOOR->door_num = i; //assign door_num
               O_COUNT = 48;
            }//if
            else {
               show("OOps, you tried to write over an existing door!!\n",
                    pc);
               i = get_next_door(); 
               Sprintf(string, "How about using: %i.\n", i);
               show(string, pc);
               break;
            }//else
         }//if its a number
         else if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
            show("A number is needed here...\n", pc);
         }//else
         break;

      case 48:                /* names */
         eos = FALSE;
         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0) {
               break;
            }//if
            if (isnum(string)) {
               show("Numbers don't work so well for names...\n", pc);
               break;
            }//if
            else {
               if (string == quitter) {
                  quit_olc(pc);  //removes bit, outputs txt
                  break;
               }//if
            }//else
            j = string.Strlen();
            if (!check_l_range(j, 0, 45, pc, FALSE)) {
               show("The name must be less than 45 characters.\n", pc);
               break;
            }//if
            else {
               if (string == "~") {
                  if (!OLC_DOOR->names.isEmpty())
                       O_COUNT = 49;
               }//if
               else {
                  tmp_str = new String(string);
                  OLC_DOOR->names.append(tmp_str);
               }//else
            }//else
         }//while
         break;

      case 49:                /* long desc */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen())== 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
            if (OLC_DOOR->long_desc.Strlen() > 0) {
                  O_COUNT = 50;
               break;
            }//if
         }//if
         else {
            OLC_DOOR->long_desc += string;
            OLC_DOOR->long_desc += "\n"; 
         }//else
        }//while TRUE
         break;

      case 50:  //door flags
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  O_COUNT = 51;
                  break;
               }//if
               if (!check_l_range(i, 0, 50, pc, TRUE))
                  break;
               if (i != 10) //this one should be static
                  OLC_DOOR->door_data_flags.flip(i);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         if (OLC_DOOR->door_data_flags.get(12))
            OLC_DOOR->door_data_flags.turn_on(11);

         out_field(OLC_DOOR->door_data_flags, pc, DOOR_DATA_FLAGS_NAMES);
         break;


      case 51: //vbit, token_num, key_num%

                        /* vbit */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 32000, pc, TRUE))
               break;
            OLC_DOOR->vis_bit = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

                        /* token_num */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
               break;
            OLC_DOOR->token_num = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

                        /* key num */
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
               break;
            OLC_DOOR->key_num = i;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

         OLC_DOOR->in_zone = ROOM.getZoneNum();
         finish_olc_door(pc);
         break;

      case 52:
         //second vehicle only case, in room descriptions case:80/81 are first
         //lets do vehicle flags...
         eos = FALSE;
         while (!eos) {
            string = pc.pc->input.Get_Command(eos, tp);
            if (string.Strlen() == 0)
               break;

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
                }//if

               if (isnum(string)) {
               i = atoi(string);
               if (i == -1) {
                  O_COUNT = 53;
                  break;
               }//if
               if (!check_l_range(i, 0, 50, pc, TRUE))
                  break;
               if (i != 10) //this one should be static
                  ((vehicle*)(OLC_ROOM))->toggleVehicleFlag(i);
            }//if
            else {
               show("Need a number here.\n", pc);
               break;  //if its not a number, it aint right for us!
            }//else
         }//while
         show("These flags are set so far:\n", pc);
         ((vehicle*)(OLC_ROOM))->showVehicleFlags(pc);
         break;

      case 53: /* vehicle, integer data:
                * ticks_between_stops, cur_fuel,
                * max_fuel (automatically starts in current room.),
                * ticks_at_stops
                */

         //ticks between stops 
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 23, pc, TRUE))
               break;
            ((vehicle*)(OLC_ROOM))->setTicksBetweenStops(i);
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

         //ticks at stops
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 23, pc, TRUE))
               break;
            ((vehicle*)(OLC_ROOM))->setTicksAtStops(i);
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         
         // current fuel
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 32000, pc, TRUE))
               break;
            ((vehicle*)(OLC_ROOM))->setCurFuel(i);
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else

         // max fuel
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 32000, pc, TRUE))
               break;
            ((vehicle*)(OLC_ROOM))->setMaxFuel(i);
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else


         finish_olc_room(pc);
         break;

      case 54:   //vehicle number
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            i = bound(2, NUMBER_OF_ROOMS, i);
            j = ROOM.getZoneNum();
            room* tmp_rm;
            
                if ((!check_l_range(i, 
                                ZoneCollection::instance().elementAt(j).getBeginRoomNum(),
                                ZoneCollection::instance().elementAt(j).getEndRoomNum(),
                                pc, TRUE) ||
                 room_list[i].isInUse())) {
               tmp_rm = get_next_room(j);
               if (tmp_rm) {
                  i = tmp_rm->getIdNum();
               }
               else {
                  pc.show("There are no more rooms available in this zone.\n");
                  quit_olc(pc);
                  break;
               }
            }//if

            tmp_rm = room_list.elementAtNoCreate(i);
            if (tmp_rm) {
               room_list.setAndDestroy(new vehicle(i), i);
            }
            else {
               room_list.set(new vehicle(i), i);
            }
         
             
            room_list[i].setFlag(23, TRUE);  //now in use
            room_list[i].setNotComplete();

            OLC_ROOM = &(room_list[i]); 
            OLC_ROOM->setRoomNum(i);
            O_COUNT = 39; //create it as you would a room, for the first steps
         }//if its a number
         else if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
            show("A number is needed here...\n", pc);
         }//else
         break;


      case 55: // give_proc: trigger item number
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 32000, pc, TRUE))
               break;
            if (!OLC_MOB->GIVE_PROC)
              OLC_MOB->GIVE_PROC = new action_proc_data;
            OLC_MOB->GIVE_PROC->test_num = i;
            O_COUNT = 60; //go get correct_gift_msg
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 56:                /* say proc, discuss topic */
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
           OLC_MOB->TOPICS.prepend(new say_proc_cell); //add a new one
           (OLC_MOB->TOPICS.peekFront())->topic = string;
           O_COUNT = 59; //go get topic message
         }//else
         break;


      case 57:                /* bow proc:  message */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if

         if (!OLC_MOB->BOW_PROC)
            OLC_MOB->BOW_PROC = new action_proc_data;

         if (string == "~") {
           O_COUNT = 61;
           break;
         }//if
         else {
            OLC_MOB->BOW_MSG += string;
            OLC_MOB->BOW_MSG += "\n"; 
         }//else
        }//while TRUE
         break;

      case 58:                /* curse proc:  message */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if

         if (!OLC_MOB->CURSE_PROC)
            OLC_MOB->CURSE_PROC = new action_proc_data;

         if (string == "~") {
           O_COUNT = 62;
           break;
         }//if
         else {
            OLC_MOB->CURSE_MSG += string;
            OLC_MOB->CURSE_MSG += "\n"; 
         }//else
        }//while TRUE
         break;

     case 59:                /* say proc:  response */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
           O_COUNT = 63;
           break;
         }//if
         else {
           (OLC_MOB->TOPICS.peekFront())->msg += string;
           (OLC_MOB->TOPICS.peekFront())->msg += "\n"; 
         }//else
        }//while TRUE
         break;

     case 60:                /* give proc:  correct_msg */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
           O_COUNT = 64; // 
           break;
         }//if
         else {
           OLC_MOB->GIVE_RIGHT_MSG += string;
           OLC_MOB->GIVE_RIGHT_MSG += "\n"; 
         }//else
        }//while TRUE
         break;

      case 61:                /* bow proc, skill_name */
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
           if (strcasecmp(string, "na") == 0) {
             string = NULL_STRING;
           }//if
           OLC_MOB->BOW_TEACH = string;
           O_COUNT = 65; //obj to give to pc
         }//else
         break;

     case 62:                /* curse proc, skill_name */
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
           if (strcasecmp(string, "na") == 0) {
             string = NULL_STRING;
           }//if
           OLC_MOB->CURSE_TEACH = string;
           O_COUNT = 66; //obj to give to pc
         }//else
         break;

     case 63:                /* say proc, skill_name */
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
           (OLC_MOB->TOPICS.peekFront())->skill_name = string;
           O_COUNT = 67; //obj to give to pc
         }//else
         break;

    case 64:                /* give proc, skill_name */
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         else {
           if (strcasecmp(string, "na") == 0) {
             string = NULL_STRING;
           }//if
           OLC_MOB->GIVE_TEACH = string;
           O_COUNT = 68; //obj to give to pc
         }//else
         break;

      case 65: // bow_proc: obj_num
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
               break;
            OLC_MOB->BOW_PROC->obj_num = i;
            O_COUNT = 69; //go get trans to room
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 66: // curse_proc: obj_num
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
               break;
            OLC_MOB->CURSE_PROC->obj_num = i;
            O_COUNT = 70; //go get trans to room
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 67: // say_proc: obj_num
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
               break;
            (OLC_MOB->TOPICS.peekFront())->obj_num = i;
            O_COUNT = 71; //go get trans to room
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 68: // give_proc: obj_num
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
               break;
            OLC_MOB->GIVE_PROC->obj_num = i;
            O_COUNT = 72; //go get trans to room
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 69: // bow_proc: trans_to_room
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ROOMS, pc, TRUE))
               break;
            OLC_MOB->BOW_PROC->trans_to_room = i;
            if (OLC_MOB->FLAG1.get(8)) // curse proc
              O_COUNT = 58;
            else {
              O_COUNT = 75; //wrong_align_msg
            }
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 70: // curse_proc: trans_to_room
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ROOMS, pc, TRUE))
               break;
            OLC_MOB->CURSE_PROC->trans_to_room = i;
            O_COUNT = 75; //wrong align msg
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 71: // say_proc: trans_to_room
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ROOMS, pc, TRUE))
               break;
            (OLC_MOB->TOPICS.peekFront())->trans_to_room = i;
            O_COUNT = 73; //go see if should do another
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 72: // give_proc: trans_to_room
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, NUMBER_OF_ROOMS, pc, TRUE))
               break;
            OLC_MOB->GIVE_PROC->trans_to_room = i;
            O_COUNT = 74; //get wrong gift msg
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

      case 73: // say_proc: do another??
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (isnum(string)) {
            i = atoi(string);
            if (!check_l_range(i, 0, 1, pc, TRUE))
               break;
            if (i == 0) { //no
              if (OLC_MOB->FLAG1.get(7)) // bow proc
                O_COUNT = 57;
              else if (OLC_MOB->FLAG1.get(8)) // curse proc
                O_COUNT = 58;
              else {
                O_COUNT = 75; //get wrong align msg
              }
            }//if
            else if (i == 1) { //yes
              O_COUNT = 56;
            }//else
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
            }//if
            break;
         }//else
         break;

     case 74:                /* give proc:  wrong_msg */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
                   if (OLC_MOB->FLAG1.get(6)) // say proc
              O_COUNT = 56;
            else if (OLC_MOB->FLAG1.get(7)) // bow proc
              O_COUNT = 57;
            else if (OLC_MOB->FLAG1.get(8)) // curse proc
              O_COUNT = 58;
            else {
              O_COUNT = 75; //get wrong align msg
            }
         }//if
         else {
           OLC_MOB->GIVE_WRONG_MSG += string;
           OLC_MOB->GIVE_WRONG_MSG += "\n"; 
         }//else
        }//while TRUE
         break;

     case 75:                /* give proc:  wrong_align_msg */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
             O_COUNT = 76; //wrong_class_msg
         }//if
         else {
           OLC_MOB->WRONG_ALIGN_MSG += string;
           OLC_MOB->WRONG_ALIGN_MSG += "\n"; 
         }//else
        }//while TRUE
         break;

     case 76:                /* give proc:  wrong_class_msg */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
             O_COUNT = 77; //wrong_race_msg
         }//if
         else {
           OLC_MOB->WRONG_CLASS_MSG += string;
           OLC_MOB->WRONG_CLASS_MSG += "\n"; 
         }//else
        }//while TRUE
         break;

     case 77:                /* give proc:  wrong_race_msg */
        while (TRUE) {
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if
         if (string == "~") {
            finish_olc_mob(pc);
            return;
         }//if
         else {
           OLC_MOB->WRONG_RACE_MSG += string;
           OLC_MOB->WRONG_RACE_MSG += "\n"; 
         }//else
        }//while TRUE
         break;

      case 78: //obj_spec_data: skin_num
         if (!OLC_OBJ->obj_proc) {
            OLC_OBJ->obj_proc = new obj_spec_data;
         }//if            
         string = pc.pc->input.Get_Command(eos, tp);

         if (string.Strlen() == 0)
            break;         

         if (string == quitter) {
            quit_olc(pc);  //removes bit, outputs txt
            break;
         }//if

         if (!isnum(string)) {
            show("Need a number here...\n", pc);
            break;
         }//if
         i = atoi(string);
             if (!check_l_range(i, 0, NUMBER_OF_ITEMS, pc, TRUE))
              break;
         OLC_OBJ->obj_proc->skin_ptr = &(obj_list[i]);

         if (OLC_OBJ->OBJ_SPEC_FLAGS.get(10)) {
           O_COUNT = 79;
         }//if
         else {
           finish_olc_obj(pc);
         }
         break;

      case 79: //casts_these_spells
         j = TRUE; //is first
         eos = FALSE;

         // Make sure the flag is set appropriately:

         OLC_OBJ->OBJ_SPEC_FLAGS.turn_on(10);

         while (!eos) {
               string = pc.pc->input.Get_Command(eos, tp);

            if (string.Strlen() == 0)
               break;         

            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
            if (!isnum(string)) {
               show("Need a number here.\n", pc);
               break;
            }//if
            i = atoi(string);
            if (!check_l_range(i, -1, 32000, pc, TRUE)) {
               break;
            }//if
            else {
              if (j == TRUE) { //ie read in first in pair
                if (i == -1) {
                  finish_olc_obj(pc);
                }//if
                else {
                  sp_ptr = new stat_spell_cell;
                  sp_ptr->bonus_duration = i;
                  sp_ptr->stat_spell = 0; //init just in case
                  OLC_OBJ->CASTS_THESE_SPELLS.prepend(sp_ptr);
                  j = FALSE; //now looking for the second
                }//else
              }//if
              else {
                sp_ptr = OLC_OBJ->CASTS_THESE_SPELLS.peekFront();
                sp_ptr->stat_spell = i;
                j = TRUE; //once again looking for first..
              }//if
            }//else
         }//while         
         break;
         
      case 80: //vehicle stopped description
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (isnum(string)) {
            show("A number for a vehicle stopped desc??\n", pc);
            break;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
         }//else
         if (!check_l_range(j, 0, 79, pc, FALSE)) {
            show("The veh_stopped desc should be less than 80 characters.\n", pc);
            break;
         }//if
         else {
            static_cast<vehicle*>(OLC_ROOM)->veh_stopped = string;
            O_COUNT = 81; 
         }//else
         break;

      case 81: //vehicle moving description
         string = pc.pc->input.Get_Rest();

         if ((j = string.Strlen()) == 0)
            break;         

         if (isnum(string)) {
            show("A number for a vehicle moving desc??\n", pc);
            break;
         }//if
         else {
            if (string == quitter) {
               quit_olc(pc);  //removes bit, outputs txt
               break;
            }//if
         }//else
         if (!check_l_range(j, 0, 79, pc, FALSE)) {
            show("The veh_moving desc should be less than 80 characters.\n", pc);
            break;
         }//if
         else {
            static_cast<vehicle*>(OLC_ROOM)->veh_moving = string;
            O_COUNT = 52; 
         }//else
         break;


      default:
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR:  default called in big switch of OLC, switch number:  "
                   << O_COUNT << endl;
         }
         String buf(200);
         Sprintf(buf, "You are screwed!  You found an invalid OLC case:  %d\n",
                 O_COUNT);
         show(buf, pc);
         break;

   }//switch
   mudlog.log(TRC, "Done w/switch in do_olc.\n");

}//do_olc()


void finish_olc_obj(critter& pc) {
   mudlog.log(TRC, "In finish_olc_obj.\n");
   
   show("Object creation completed, thanks!! :)\n", pc);
   show("The object can be found in your inv..\n", pc);

   OLC_OBJ->setComplete();

   String ofile_buf("./World/OLC_DIR/");
   String temp_buf(*(name_of_crit(pc, ~0)));
   temp_buf.Tolower();
   ofile_buf += temp_buf;

   String tdir;
   tdir += "mkdir -p ";
   tdir += ofile_buf;
   system(tdir);

   ofile_buf += "/";
   ofile_buf += *(name_of_obj(*OLC_OBJ, ~0));
   ofile_buf.Append(OLC_OBJ->OBJ_NUM);
   ofstream ofile(ofile_buf);
   if (!ofile) {
      show("ERROR, ofile not opened correctly...tell your imp.\n", pc);
      mudlog.log(LS_ERROR, "ERROR:  ofile not opened, olc crit writeout.\n");
      mudlog.log(LS_ERROR, ofile_buf);
      OLC_OBJ = NULL;
      return;
   }//if
   else {

      /* first normalize this puppy as much as possible */
      if(!(pc.pc && pc.pc->pc_data_flags.get(2)
           && pc.pc->imm_data->imm_level >= 9)) {
             normalize_obj(*(OLC_OBJ));
           }
      ofile << OLC_OBJ->cur_stats[2];
      ofile << "  // Begin of an object\n";
      OLC_OBJ->Write(ofile);
      ofile << "\n" << flush; 
         //make sure there are enuf lines to cat em together

                /* make changes permanent */
   }// else

   aosave(OLC_OBJ->OBJ_IN_ZONE, pc);

   quit_olc(pc);
   O_COUNT = 0; //reset it for next creation
   pc.gainInv(OLC_OBJ);

   //Fix cur_in_game problem.
   recursive_init_loads(*(OLC_OBJ), 0);

   OLC_OBJ = NULL;

   mudlog.log(TRC, "Done w/finish_olc_obj.\n");
}//finish_olc_obj


void finish_olc_door(critter& pc) {
   mudlog.log(TRC, "In finish_olc_door.\n");

   OLC_DOOR->setComplete();

   Cell<String*> cll(OLC_DOOR->names);   
   String* ptr;
   while ((ptr = cll.next())) {
      if (*ptr == "#") {
         OLC_DOOR->door_data_flags.turn_on(9);  //now its flippable
         break;
      }//if
   }//while

   normalize_door(*(OLC_DOOR));

   show("Door creation completed, thanks!! :)\n", pc);
   String ofile_buf("./World/OLC_DIR/");
   String temp_buf(*(name_of_crit(pc, ~0)));
   temp_buf.Tolower();

   String tdir;
   tdir.Append("mkdir -p ");
   tdir.Append(ofile_buf);
   system(tdir);

   ofile_buf += temp_buf;
   ofile_buf += "/";
   ofile_buf += *(name_of_dr_data(*OLC_DOOR, ~0, 1));
   ofile_buf.Append(OLC_DOOR->door_num);
   ofstream ofile(ofile_buf);
   if (!ofile) {
      mudlog.log(LS_ERROR, "ERROR:  olc_buf, door--not opened correctly:");
      mudlog.log(LS_ERROR, ofile_buf);
      show("ERROR: olc_buf, door--not opened correctly...\n", pc);
      OLC_DOOR = NULL;
      return;
   }//if
   else {
      ofile << OLC_DOOR->door_num;
      ofile << "  // Begin of a Door\n";
      OLC_DOOR->Write(ofile);
      ofile << "\n" << flush; 
         //make sure there are enuf lines to cat em together

                /* make changes permanent */
   }// else

   adsave(OLC_DOOR->getZoneNum(), pc);

   quit_olc(pc);
   O_COUNT = 0; //reset it for next creation
   OLC_DOOR = NULL;

   mudlog.log(TRC, "Done w/finish_olc_door.\n");
}//finish_olc_door


// finish up an olc room or vehicle
void finish_olc_room(critter& pc) {
   mudlog.log(TRC, "In finish_olc_vehicle.\n");
   
   OLC_ROOM->normalize();
   OLC_ROOM->lock(); // make it locked
   OLC_ROOM->setTotalLoaded(TRUE); // make it totally_loaded
   OLC_ROOM->setComplete();

   if (OLC_ROOM->isVehicle()) {
      /* vehicle starts in pc's room */
      ((vehicle*)(OLC_ROOM))->setInRoom(pc.getCurRoomNum());
      pulsed_proc_rooms.append(OLC_ROOM);
   }//if

   if (OLC_ROOM->isVehicle()) {
      show("The OLC portion of vehicle creation has been completed.\n", pc);
      show("However, it has no path programmed into it.  You will need\n", pc);
      show("to add paths to the vehicle with the:  add_path_cell\n", pc);
      show("command.  You will also need to modify the door on the vehicle.\n",
           pc);
   }
   else {
      show("Room creation completed, thanks!! :)\n", pc);
   }

   String ofile_buf("./World/OLC_DIR/");
   String temp_buf(*(name_of_crit(pc, ~0)));
   temp_buf.Tolower();
   ofile_buf += temp_buf;

   String tdir;
   tdir.Append("mkdir -p ");
   tdir.Append(ofile_buf);
   system(tdir);

   ofile_buf += "/";
   ofile_buf += *(name_of_room(*OLC_ROOM, ~0));
   ofile_buf.Append(OLC_ROOM->getRoomNum());
   switch (config.useMySQL) {
   #ifdef USEMYSQL
      case true:
         OLC_ROOM->dbWrite();
         break;
   #endif
      case false:
         ofstream ofile(ofile_buf);
         if (!ofile) {
            mudlog.log(LS_ERROR, "ERROR:  olc_buf not opened correctly:");
            mudlog.log(LS_ERROR, ofile_buf);
            show("ERROR: olc_buf not opened correctly...\n", pc);
            OLC_ROOM = NULL;
            return;
         }//if
         else {
            if (OLC_ROOM->isVehicle()) { //vehicle number HACK
               ofile << (OLC_ROOM->getRoomNum() | 0x01000000) << " " 
                     << "  // Vehicle Number (high byte is identifier)\n";
            }
            else {
               ofile << OLC_ROOM->getRoomNum() << " " 
                     << "  // Room Number\n";
            }

            OLC_ROOM->fileWrite(ofile);
            ofile << "\n" << flush; 
         }// else
         break;
   }

   write_zone(OLC_ROOM->getZoneNum(), pc);

   quit_olc(pc);
   O_COUNT = 0; //reset it for next creation
   OLC_ROOM = NULL;

   mudlog.log(TRC, "Done w/finish_olc_room.\n");
}//finish_olc_vehicle


void finish_olc_mob(critter& pc) {
   mudlog.log(TRC, "In finish_olc_mob.\n");
   
   OLC_MOB->setComplete();

   normalize_mob(*(OLC_MOB));

   OLC_MOB->BANK_GOLD = 0;
   OLC_MOB->CRITTER_TYPE = 2;
   OLC_MOB->setCurInGame(1);
   OLC_MOB->setCurRoomNum(0);

   show("Critter creation completed, thanks!! :)\n", pc);
   show("The mob can be found in your present room..\n", pc);

   if (!pc.PC_FLAGS.get(13)) { //redo, used for discuss procs and the like
      pc.PC_FLAGS.turn_off(13);
      String ofile_buf("./World/OLC_DIR/");
      String temp_buf(*(name_of_crit(pc, ~0)));
      temp_buf.Tolower();
      ofile_buf += temp_buf;
      
      String tdir;
      tdir.Append("mkdir -p ");
      tdir.Append(ofile_buf);
      system(tdir);
      
      ofile_buf += "/";
      ofile_buf += *(OLC_MOB->names.peekFront());
      ofile_buf.Append(OLC_MOB->MOB_NUM);
      ofstream ofile(ofile_buf);
      if (!ofile) {
         mudlog.log(LS_ERROR, "ERROR:  olc_buf not opened correctly:");
         mudlog.log(LS_ERROR, ofile_buf);
         show("ERROR: olc_buf not opened correctly...\n", pc);
         OLC_MOB = NULL;
         return;
      }//if
      else {
         ofile << OLC_MOB->MOB_NUM;
         ofile << "  // Begin of a critter\n";
         OLC_MOB->Write(ofile);
         ofile << "\n" << flush;
         
      }// else

      amsave(OLC_MOB->getNativeZoneNum(), pc);
   }//if not a redo
   else {
      show("To make these changes permanent, you must do an amsave.\n", pc);
   }//else
   
   quit_olc(pc);
   O_COUNT = 0; //reset it for next creation
   ROOM.gainCritter(OLC_MOB);
   OLC_MOB = NULL;
   
   mudlog.log(TRC, "Done w/finish_olc_mob.\n");
}//finish_olc_mob

















