// $Id: Markup.cc,v 1.4 1999/09/08 06:11:36 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 1999/09/08 06:11:36 $

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


#include "Markup.h"
#include "misc.h"
#include "misc2.h"


/** Static method */
String Markup::makeSafeForHegTag(const char* str) {
   int len = strlen(str);
   char c;
   String retval(len + 50);

   for (int i = 0; i<len; i++) {
      c = str[i];
      if (c == '>') {
         retval += ">>";
      }
      else {
         retval += c;
      }
   }//for
   return retval;
}//convertToHeg


void Markup::toString(const bitfield& flags, const LstrArray& flag_names,
                      critter* pc, String& rslt) {
   toString(NULL, flags, flag_names, pc, NULL, rslt);
}


void Markup::toString(const char* pre, const bitfield& flags,
                      const LstrArray& flag_names, critter* pc,
                      const char* post, String& rslt) {

   int k = flags.max_bit();
   String tmp(100);
   int sofar = 0;

   if (pc && !pc->isUsingClient()) {
      Sprintf(rslt, "%s (SET)\n\t", flag_names.getHeader(pc));

      for (int i = 0; i <= k; i++) {
         if (flags.get(i)) {
            Sprintf(tmp, "[%i] %s,  ", i, flag_names.getName(i, pc));
            if ((sofar + tmp.Strlen()) > 80) {
               rslt += "\n\t";
               sofar = tmp.Strlen();
            }
            else {
               sofar += tmp.Strlen();
            }
            rslt += tmp;
         }//if
      }//for
      rslt.Append("\n");
   }
   else {
      rslt = pre;
      
      rslt.append("<BITFIELD> ");

      String val(100);
      for (int i = 0; i <= k; i++) {
         if (flags.get(i)) {
            val = makeSafeForHegTag(flag_names.getName(i, pc));
            Sprintf(tmp, "%i=\"%S\" ", i, &val);
            if ((sofar + tmp.Strlen()) > 100) {
               rslt += "\n";
               sofar = tmp.Strlen();
            }
            else {
               sofar += tmp.Strlen();
            }
            rslt += tmp;
         }//if
      }//for
      rslt.append("</BITFIELD>\n");
      rslt.append(post);
   }
}//toString (bitfield)


void Markup::toString(SafeList<critter*>* lst, critter* viewer, String& rslt) {
   do_out_crit_list(*lst, viewer, rslt);
}

void Markup::toString(SafeList<object*>* lst, critter* viewer, 
                      ObjListTypeE list_type, String& rslt) {
   do_out_obj_list(*lst, *viewer, list_type, rslt);
}

void Markup::toString(PtrList<StatBonus>* lst, critter* viewer,
                      const LstrArray& names, String& rslt) {
   Cell<StatBonus*> cll(*lst);
   StatBonus* ptr;
   String buf(100);

   LanguageE lang = English;
   if (viewer)
      lang = viewer->getLanguage();
   
   if (lst->isEmpty()) {
      rslt = cstr(CS_NONE, lang);
      return;
   }
   else {
      rslt.clear();
   }

   while ((ptr = cll.next())) {
      Sprintf(buf, "(%s %i) ", names.getName(ptr->stat, viewer),
              ptr->bonus);
      rslt.append(buf);
   }//while
   rslt.append("\n");
}

void Markup::toString(PtrList<SpellDuration>* lst, critter* viewer, String& rslt) {
   Cell<SpellDuration*> cll(*lst);
   SpellDuration* ptr;
   String buf(100);

   LanguageE lang = English;
   if (viewer)
      lang = viewer->getLanguage();

   if (lst->isEmpty()) {
      rslt = cstr(CS_NONE, lang);
      return;
   }
   else {
      rslt.clear();
   }

   while ((ptr = cll.next())) {
      Sprintf(buf, "(%s %i) ",
              SSCollection::instance().getNameForNum(ptr->spell, viewer),
              ptr->duration);
      rslt.append(buf);
   }//while
   rslt.append("\n");
}//out_spell_list


