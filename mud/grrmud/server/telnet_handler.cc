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
#include "telnet.h"
#include "LogStream.h"

#include <arpa/inet.h>

extern LogStream mudlog;

int TelnetHandler::_cnt = 0;
const char TelnetHandler::eor_str[2] = { IAC, EOR };

TelnetHandler::TelnetHandler(critter* c_ptr) {

   _cnt++;

   my_critter = c_ptr;
   current_state = ST_TEXT;
   current_text_state = ST_NORM;

   //memset is probably faster, but oh well.
   for(unsigned int i=0;i<sizeof(my_want_states);my_want_states[i++]=false);
   for(unsigned int i=0;i<sizeof(my_option_states);my_option_states[i++]=false);

   send(DO,TELOPT_NAWS);

   my_option_states[TELOPT_EOR] = true;
   send(WILL,TELOPT_EOR);
   send(WONT,TELOPT_ECHO);

}//Constructor: TelnetHandler

void TelnetHandler::send(int action, int option) {

   char msg[4];
   msg[0] = IAC;
   msg[1] = action;
   msg[2] = option;
   msg[3] = '\0';

   my_critter->pc->output += msg;

}//TelnetHandler::send()

//handle DOs
void TelnetHandler::rcv_do(int opt) {

   bool need_to_respond = false;
   bool will = false;

   mudlog << "TelnetHandler::rcv_do received: IAC DO " << opt << endl;

   //If we are already doing what they're asking for, we just ignore that they
   //asked. this is the behaviour required by the rfc.
   if ( ! my_option_states[opt] ) {

      //If we did not start the conversation with a WILL offer, we need to
      //respond with a WILL.
      if ( my_want_states[opt] ) {
         need_to_respond = false;
         my_want_states[opt] = false;
      }  else {
         need_to_respond = true;
      }

      switch ( opt ) {

         case TELOPT_ECHO:
         case TELOPT_EOR:
            my_option_states[opt] = will = true;
         break;

         default:
            need_to_respond = true;
            will = false;
         break;

      };

   }//if not already doing

   if ( need_to_respond ) {

      if ( will ) {
         send(WILL,opt);
      } else {
         send(WONT,opt);
      }

   }

}//TelnetHandler::rcv_do()

//handle DONTs
void TelnetHandler::rcv_dont(int opt) {
   bool need_to_respond = false;

   mudlog << "TelnetHandler::rcv_dont received: IAC DONT " << opt << endl;

   //the rfc requires that we never refuse to turn off a feature and that we
   //be willing to drop down to basics.

   if ( my_option_states[opt] ) {
      my_option_states[opt] = false;
      need_to_respond = true;
   } else {
      need_to_respond = false;
   }

   if ( need_to_respond ) {
      send(WONT,opt);
   }

}//TelnetHandler::rcv_dont()

//handle WILLs
void TelnetHandler::rcv_will(int opt) {
   // I don't expect to have to deal with these, but for now I'll log them to
   // find out if I'm wrong.
   mudlog << "TelnetHandler::rcv_will received: IAC WILL " << opt << endl;
}//TelnetHandler::rcv_will()

//handle WONTs
void TelnetHandler::rcv_wont(int opt) {
   // The only thing that might come back to us here is NAWS as I don't plan
   // on asking the client to do much more than that. so for now, just log the
   // fact that they said they wouldn't do something.
   mudlog << "TelnetHandler::rcv_wont received: IAC WONT " << opt << endl;
}//TelnetHandler::rcv_wont()

bool TelnetHandler::parse(const char* input_buf, size_t len) {

   size_t i=0;
   const char* p;
   bool parsed_full_command = false;

   while ( i < len ) {

      p = input_buf+i;

      switch ( current_state ) {

         case ST_TEXT:
            switch (*p) {

               case IAC:
                  current_state = ST_IAC;
               break;

               default:
                  // we don't directly append to the pc's buf as the command
                  // parser isn't currently smart enough to deal with
                  // incomplete commands. We also need to handle backspace,
                  // etc. These things were previously dealt with in
                  // String::Read()
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

                  }//switch(current_text_state)
               break;
            };//ST_TEXT: switch(*p)
         break;//end ST_TEXT

         case ST_IAC:
            switch (*p) {

               case IAC://escaped IAC.
                  out_buf += *p;
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
                  //invalid transistion
                  current_state = ST_TEXT;
               break;
            };
         break;

         case ST_DO:
            rcv_do(*p);
            current_state = ST_TEXT;
         break;

         case ST_DONT:
            rcv_dont(*p);
            current_state = ST_TEXT;
         break;

         case ST_WILL:
            rcv_will(*p);
            current_state = ST_TEXT;
         break;

         case ST_WONT:
            rcv_wont(*p);
            current_state = ST_TEXT;
         break;

         case ST_SB://processing a sub-option datastream
            switch (*p) {

               case IAC:
                  current_state = ST_SB_IAC;
               break;

               default://default here is to append to sb_buf
                  sb_buf += *p;
                  // sanity check this to prevent resource exhaustion from a
                  // rogue client
                  if ( sb_buf.size() > 256 ) {
                     sb_buf.clear();
                     current_state = ST_TEXT;
                  }
               break;
            };
         break;

         case ST_SB_IAC:
            switch (*p) {

               case IAC://escaped IAC character.
                  sb_buf += *p;
                  current_state = ST_SB;
               break;

               case SE://end of sub-option data
                  process_subopt();
                  sb_buf.clear();
                  current_state = ST_TEXT;
               break;

               default:
                  //this state transistion is not valid
                  current_state = ST_TEXT;
                  sb_buf.clear();
               break;

            };
         break;

      };//switch ( telnet_state )

      i++;
   }//while there's more input

   return(parsed_full_command);

}//TelnetHandler::parse()

bool TelnetHandler::should_echo() const {
   return my_option_states[TELOPT_ECHO];
}//TelnetHandler::should_echo()

void TelnetHandler::set_echo(bool on_off) {

   //do nothing if we're already at the requested state.
   if ( on_off == my_option_states[TELOPT_ECHO] ) {
      return;
   }

   switch ( on_off ) {

      case true:
         my_want_states[TELOPT_ECHO] = true;
         send(WILL,TELOPT_ECHO);
      break;

      case false:
         //not sure if we're supposed to wait for a DONT, but since a WONT is
         //a "refusal" to do, or continue doing, I don't assume we technically
         //need to wait.
         my_option_states[TELOPT_ECHO] = false;
         send(WONT,TELOPT_ECHO);
      break;

   };

}//TelnetHandler::set_echo()

void TelnetHandler::process_subopt() {
   //for now we ignore everything but NAWS, and even then just spew some log
   //information about it as we haven't established a place to store it in the
   //pcdata yet.
   if ( sb_buf.size() == 0 ) {
      return;
   }

   const char* p = sb_buf.data();

   switch ( *p ) {
      case TELOPT_NAWS: {
         short width, height;

         width = height = -1;
         if ( sb_buf.size() != 5 ) {
            //NAWS should be: NAWS WIDTH[1] WIDTH[0] HEIGHT[1] HEIGHT[0]
            mudlog
               << "TelnetHandler::process_subopt() received malformed NAWS of len "
               << sb_buf.size()
               << endl;
            break;
         }

         width = *(short *)(p+1);
         height = *(short *)(p+3);

         width = ntohs(width);
         height = ntohs(height);

         my_critter->pc->lines_on_page = height;
         my_critter->pc->columns_on_page = width;

         mudlog << "TelnetHandler::process_subopt() received NAWS "
            << "width: " << width << " height: " << height << endl;
      }//TELOPT_NAWS
      break;

      default:
      break;

   }//switch(sb_buf[0])

   sb_buf.clear();

}//TelnetHandler::process_subopt()

const char* TelnetHandler::end_of_record() const {

   if ( my_option_states[TELOPT_EOR] ) {
      return(TelnetHandler::eor_str);
   } else {
      return("");
   }

}//TelnetHandler::end_of_record()

void TelnetHandler::newCritter(critter* c_ptr) {
   my_critter = c_ptr;
}//TelnetHandler::newCritter()
