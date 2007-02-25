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

#include "hegemon_handler.h"

int HegemonHandler::_cnt = 0;

HegemonHandler::HegemonHandler(critter* c_ptr) {

   _cnt++;
   my_critter = c_ptr;
   current_text_state = ST_NORM;

}//HegemonHandler::HegemonHandler()

bool HegemonHandler::parse(const char* input_buf, size_t len) {
   size_t i = 0;
   const char* p;
   bool parsed_full_command = false;

   while ( i < len ) {

      p = input_buf+i;

      switch ( current_text_state ) {
         case ST_NORM:
            switch ( *p ) {

               case 0x08://backspace
                  if ( out_buf.size() > 0 ) {
                     out_buf.resize(out_buf.size()-1);
                  }
               break;

               case 0x0D://carriage return
                  //do nothing, we don't like \r's around here.
               break;

               case 0x0A://newline
                  out_buf += *p;
                  my_critter->pc->input += out_buf.c_str();
                  out_buf.clear();
                  parsed_full_command = true;
               break;

               case ';':
                  current_text_state = ST_SEMICOLON;
               break;

               default:
                  out_buf += *p;
               break;
            };
         break;

         case ST_SEMICOLON:
            switch ( *p ) {

               case ';':
                  out_buf += ';';
               break;

               case 0x08://backspace
                  if ( out_buf.size() > 0 ) {
                     out_buf.resize(out_buf.size()-1);
                  }
                  current_text_state = ST_NORM;
               break;

               default:
                  out_buf += '\n';
                  out_buf += *p;
               break;

            };
            current_text_state = ST_NORM;
         break;
      };//switch(current_text_state)

      i++;
   }//while there's more input

   return(parsed_full_command);
}
