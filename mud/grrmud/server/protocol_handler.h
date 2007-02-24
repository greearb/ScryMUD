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

// --------------------------------------------------------------------------
// The general idea here is that every connection starts with a NullHandler.
// This handler essentially implements the ProtocolHandler interface but
// passes things along as they are. Once a client type is determined during
// the login process the NullHandler is delete'd and replaced with a new
// handler of the more specific and appropriate type.
//
// Also worth mentioning is that the interface is currently only designed to
// provide input (to the server) filtering. The lack of an output filter is
// the result of having not yet finalized a replacement socket class and
// therefore not being certain what said interface should look like. This is
// obviously important to implement at a later date. In particular both
// Hegemon and Telnet expect certain characters/sequences to be escaped.
// --------------------------------------------------------------------------

#ifndef GRRMUD_PROTOCOL_HANDLER
#define GRRMUD_PROTOCOL_HANDLER

class ProtocolHandler;

#include "critter.h"

class ProtocolHandler {
   public:

      virtual ~ProtocolHandler() { };

      // do any necessary parsing, stick the resulting data (if any) in the pc's
      // input buffer.
      virtual void parse(const char* input_buf) = 0;

      // toggle server-side echoing of input
      virtual void set_echo(bool on_off) = 0;

      // return a string used to mark end-of-record (player prompt)
      virtual const char* end_of_record() const = 0;
};//class ProtocolHandler

class NullHandler : public ProtocolHandler {
   protected:
      static int _cnt;

      critter* my_critter;

   public:

      static int getInstanceCount() { return NullHandler::_cnt; }

      NullHandler(critter *c_ptr) : my_critter(c_ptr) { my_critter = c_ptr; NullHandler::_cnt++; };
      ~NullHandler() { NullHandler::_cnt--; };

      void parse(const char* input_buf);
      void set_echo(bool on_off) { return; }
      const char* end_of_record() const { return(""); }
};//class NullHandler

#endif
