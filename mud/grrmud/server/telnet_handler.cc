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

#include "telnet_handler.h"

TelnetHandler::TelnetHandler(critter* c_ptr) {

   TelnetHandler::_cnt++;

   my_critter = c_ptr;
   current_state = ST_TEXT;

   //memset is probably faster, but oh well.
   for(int i=0;i<sizeof(my_want_states);my_want_states[i++]=false);
   for(int i=0;i<sizeof(my_option_states);my_option_states[i++]=false);

}//Constructor: TelnetHandler

//handle DOs
void TelnetHandler::rcv_do(const int opt) {

   if ( my_want_states[opt] ) {

   }

   switch ( opt ) {

      case TELOPT_ECHO:
      break;

      case TELOPT_EOR:
      break;

      default:
      break;

   };

}//TelnetHandler::rcv_do()

//handle DONTs
void TelnetHandler::rcv_dont(const int opt) {
}//TelnetHandler::rcv_dont()

//handle WILLs
void TelnetHandler::rcv_will(const int opt) {
}//TelnetHandler::rcv_will()

//handle WONTs
void TelnetHandler::rcv_wont(const int opt) {
   // The only thing that might come back to us here is NAWS
   // as I don't plan on asking the client to do much more than
   // that.
}//TelnetHandler::rcv_wont()

void TelnetHandler::parse(const char* input_buf) {
   using namespace telnet;

   const char* p = input_buf;

   while ( *p ) {

      switch ( current_state ) {

         case ST_TEXT:
            switch (*p) {

               case IAC:
                  current_state = ST_IAC;
               break;

               default:
                  my_critter->pc->input += *p;
               break;
            };
         break;

         case ST_IAC:
            switch (*p) {

               case IAC://escaped IAC
                  my_critter->pc->input += *p;
               break;

               case DO:
                  current_state = ST_DO;
               break;

               case DONT:
                  current_state = ST_DONT;
               break;

               case WILL:
                  current_state = ST_WILL;
               break;

               case WONT:
                  current_state = ST_WONT;
               break;

               case SB://start option sub-negotiation
                  current_state = ST_SB;
               break;

               default:
               break;
            };
         break;

         case ST_DO:
            rcv_do(*p);
         break;

         case ST_DONT:
            rcv_dont(*p);
         break;

         case ST_WILL:
            rcv_will(*p);
         break;

         case ST_WONT:
            rcv_wont(*p);
         break;

         case ST_SB://processing a sub-option datastream
            switch (*p) {

               case IAC:
                  current_state = ST_SB_IAC;
               break;

               default://default here is to append to sb_buf
                  // sanity check this to prevent resource exhaustion from a
                  // rogue client
                  sb_buf += *p;
                  if ( sb_buf.Strlen() > 256 ) {
                     current_state = ST_TEXT;
                     sb_buf.clear();
                  }
               break;
            };
         break;

         case ST_SB_IAC:
            switch (*p) {

               case IAC:
                  sb_buf += *p;
               break;

               case SE://end of sub-option data
                  sb_buf.clear();
               break;

               default:
                  //this state transistion is not valid
               break;

            };
         break;

      };//switch ( telnet_state )

      p++;
   }//while there's more input

}//TelnetHandler::parse()
