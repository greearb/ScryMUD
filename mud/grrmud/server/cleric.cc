// $Id$

//
//ScryMUD Server Code
//Copyright (C) 2007  Edward Roper
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

#include "cleric.h"
#include "skills.h"
#include "spells.h"
#include "misc.h"
#include "misc2.h"

void cast_divine_retribution(int i_th, const String *victim, critter &pc)
{
   critter *vict = NULL;
   int spell_num = DIVINE_RETRIBUTION_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      pc.show("Whose do you wish to protect?", HL_DEF);
      return;
   }

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim, pc.SEE_BIT);
   } // if

   if (!ok_to_do_action(vict, "KMVN", spell_num, pc)) {
      return;
   }

   do_cast_divine_retribution(*vict, pc, FALSE, 0);
}

void do_cast_divine_retribution(critter &vict, critter &agg, int is_canned, int lvl)
{
   String buf(100);
   int spell_num = DIVINE_RETRIBUTION_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned)
      lvl = agg.getLevel();

   // Time to rock n' roll.
   if ( is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      stat_spell_cell *ptr = is_affected_by(spell_num, vict);

      if ( ! is_canned )
          agg.adjMana(-spell_mana);

      if ( ptr ) {
         ptr->bonus_duration += lvl / 2;
      } else {
         vict.affected_by.append(new stat_spell_cell(spell_num, lvl));
          
         // Cast on self
         if ( &vict == &agg ) {
            vict.show("You are surrounded by a crackling aura.\n", HL_DEF);
            Sprintf(buf, "surrounds %s with a crackling aura.", get_himself_herself(vict));
            vict.emote(buf);
         } else {

            Sprintf(buf, "You surround %pS with a crackling aura.\n",
                  name_of_crit(vict, agg.SEE_BIT));
            agg.show(buf, HL_DEF);

            Sprintf(buf, "%pS surrounds you with a crackling aura.\n",
                  name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            vict.show(buf, HL_DEF);

            Sprintf(buf, "surrounds %pS with a crackling aura.",
                  name_of_crit(vict, ~0));
            emote(buf, agg, *(agg.getCurRoom()), TRUE, &vict);
         }
      }
   }// end of "if - it - worked"
   else { // !canned && lost concentration
      agg.show(LOST_CONCENTRATION_MSG_SELF, HL_DEF);
      Sprintf(buf, "seems to have lost %s concentration.", get_his_her(agg));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
      if ( ! is_canned )
          agg.adjMana(-spell_mana/2);
   }

}

