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

#ifndef GRRMUD_HEGEMON_HANDLER
#define GRRMUD_HEGEMON_HANDLER

#include "protocol_handler.h"

class HegemonHandler : public ProtocolHandler {

   protected:
      static int _cnt;

      critter* my_critter;

   public:

      static int getInstanceCount() { return HegemonHandler::_cnt; }

      HegemonHandler(critter *c_ptr) : my_critter(c_ptr) { HegemonHandler::_cnt++; };
      ~HegemonHandler() { HegemonHandler::_cnt--; };

      void parse(const char* input_buf) { my_critter->pc->input += input_buf; }
      void toggle_echo() { }
      const char* end_of_record() const { return(""); }
};


#endif
