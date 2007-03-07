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
// To contact the Author, Edward Roper: edro <-@-> wanfear.net
//

#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "critter.h"
#include "command2.h"

//loads all pfiles and saves them again. Useful for upgrading pfile versions
//and making sure sanitization gets applied to everyone.
int resave_all_pfiles() {

   DIR* d_ptr = opendir("./Pfiles");
   std::vector<struct dirent> file_v;

   struct dirent *dent_ptr;

   if ( ! d_ptr ) {
      return -1;
   }

   errno = 0;
   while ( ( dent_ptr = readdir(d_ptr) ) ) {
      if ( dent_ptr->d_type != DT_REG ) continue;//only do regular files.
      file_v.push_back(*dent_ptr);
   }

   if ( errno != 0 ) {
      return -1;
   }

   std::vector<struct dirent>::iterator dent_iter;

   for ( dent_iter = file_v.begin(); dent_iter < file_v.end(); dent_iter++ ) {
      string filepath("./Pfiles/");
      filepath.append(dent_iter->d_name);
      ifstream pfile(filepath.c_str());
      if ( !pfile )  {
         return -1;
      }
      cout << "Checking " << filepath << "." << endl;
      critter* c = new critter;
      c->fileRead(pfile, TRUE);

      //prior to version 1, stats were saved with eq modifiers in place.
      if ( c->pc->file_format_version == 0 ) {
         cout << "Upgrading " << (const char *)(*(c->getName())) << " from version 0." << endl;

         // if it's the old version remove everything so we can force
         // correct nasties that ran away with us.
         int i;
         for(i=0;i<MAX_EQ;i++) {
            if (c->EQ[i]) {
               remove_eq_effects(*(c->EQ[i]), *c, FALSE, FALSE, i);
            }
         }

      }//if pfile version 0

      //Other sanitizing steps.
      c->BH_DICE_COUNT = 2;
      c->BH_DICE_SIDES = 3;

      // re-gain effects from worn gear
      // save removes them again.
      {
         int i;
         for(i=0;i<MAX_EQ;i++) {
            if (c->EQ[i]) {
               wear_eq_effects(*(c->EQ[i]), *c, i, FALSE);
            }
         }
      }

      save(*c);
      delete c;
   }

   closedir(d_ptr);

   return 0;
}//resave_all_pfiles()
