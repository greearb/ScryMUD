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

#ifndef GRRMUD_TELNET_HANDLER
#define GRRMUD_TELNET_HANDLER

#include "string2.h"
#include "critter.h"
#include "protocol_handler.h"

class TelnetHandler : public ProtocolHandler {

   protected:
      static int _cnt;
      static const char eor_str[];
      static const char ttype_req_str[];

      enum state { ST_TEXT, ST_IAC, ST_DO, ST_DONT, ST_WILL, ST_WONT, ST_SB, ST_SB_IAC };
      enum text_state { ST_NORM, ST_SEMICOLON };

      state current_state;
      text_state current_text_state;
      std::string sb_buf;
      std::string terminal_type;
      std::string out_buf;
      critter* my_critter;

      bool my_offer_states[256];     //things I've requested or offered.
      bool my_request_states[256];  //things I've requested.
      bool my_option_states[256];   //the current state of things.

      void rcv_do(int opt);
      void rcv_dont(int opt);
      void rcv_will(int opt);
      void rcv_wont(int opt);

      void process_subopt();
      void process_out_buf();
      void process_naws();
      void process_ttype();

      //inserts IAC action option into my_critter's outbuf.
      void send(int action, int option);
      //append an arbitrary string to my_critter's outbuf.
      void send(const char* msg);

      bool should_echo() const;

   public:
      static int getInstanceCount() { return _cnt; }

      TelnetHandler(critter* c_ptr);
      ~TelnetHandler();

      void newCritter(critter* c_ptr);
      bool parse(const char* input_buf, size_t len);
      void set_echo(bool on_off);
      const char* end_of_record() const;

};

#endif
