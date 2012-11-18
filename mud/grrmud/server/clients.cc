#include "clients.h"
#include <LogStream.h>

extern LogStream mudlog;


/** could make this work more like the translations strings,
 * but for now this looks pretty good to me. 
 * there might be some trouble adding new clients(due to
 * NUM_TAGS, obviously you can increase, but that might
 * waste memory), it may be neccesary to setup the tags arrays
 * separatly then set client_tags[x] to point
 * to the right array, for now, it'll do perfectly. 
 */

// Don't worry about memory!!  Unless you are talking 500k+, it will
// be lost in the noise! --Ben 

/** yet another neat idea from the brain of the bouncing one:
 * why don't we simply make pc.client point to the right array,
 * and then we can go further and have a local array store the
 * color data(same way it is now i suppose), but make it setup
 * like the client tags array, the we can just do pc.client[CTAG]
 * and not worry about having to make client checks. 
 */

/* things converted thus far: battle.cc commands.cc, plus the marked ones below*/
const char* client_tags[NUM_CLIENTS][NUM_TAGS] = {
	
   /** no client, this isn't really needed right now, but may
    * be needed later., change this to default colors if above idea works out 
    */

   // NOTE: (BUG) (TODO):  If you pass these NULL strings into places that expect
   // the HEGEMON strings with arguments (%i...), then it will definately screw
   // up the server, probably crashing it.  Either add all of the %? things here
   // (the rest can be empty), or take out the option to have this 'client'.
   // For now, I'm going to dis-allow this client. --Ben
   {
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0",
      "\0"
   },

        
        
   //hegemon
   {
      "<ENGAGE_HEGEMON>\n", //converted
      "<PROMPT %i %i %i %i %i %i> ",
      "<BEEP>", //converted
      "<BATTLE>", //converted
      "</BATTLE>", //converted
      
      //channels
      "<SAY>",
      "</SAY>",
      "<TELL>",
      "</TELL>",
      "<YELL>", /* 10, 9_th index */
      "</YELL>",
      "<GOSSIP>",
      "</GOSSIP>",
      "<AUCTION>",
      "</AUCTION>",
      
      //descriptions and such
      "<RM_SHORT_DESC %i>",
      "</RM_SHORT_DESC>",
      "<RM_DESC>",
      "</RM_DESC>",
      "<KEYWORD_DESC>",
      "</KEYWORD_DESC>",
      "<EXITS >",
      "<MOB_LIST>",
      "</MOB_LIST>",
      "<ITEM_LIST>",
      "</ITEM_LIST>",
      "<DOOR_LIST>",
      "</DOOR_LIST>",
      
      //stat_script stuff
      "<SCRIPT_DATA>",
      "</SCRIPT_DATA>",
      "<MOB_SCRIPT %S %i %i %i %i %i>",
      "<ROOM_SCRIPT %S %i %i %i %i %i>",
      "<OBJ_SCRIPT %S %i %i %i %i %i>",
      "<DISCRIM %S>",
      
      //not too sure what these are... i'm guessing LD is link ead though
      "<STAT>",
      "</STAT>",
      "<STAT_SD>",
      "<STAT_ND>",
      "<STAT_LD>",
      "<NAMES %S>",
      
      "\0",//value_list should go here, but...
      "<VALUE_ITEM %i %i %i %i> ",
      "<VALUE_INAME %i %S> ",
      
      "<BUG_ENTRY %i %s \"%S\" %S %i %S %s>",
      "</BUG_ENTRY>"
      "<BUG_TITLE>",
      "</BUG_TITLE>",
      
      "<OSTAT %i>",
      "<MSTAT %i>",
      
      //not too sure what these are...
      "<PSTAT %i %i>",
      "<PATH_DIR %i %S> ",
      "<PATH_IS_DEST %i> "
      
   }
};

       
const char* getClientTag(int client_num, int client_tag_num) {
   if (client_num != HEGEMON) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "ERROR:  Cannot support clients other than HEGEMON right now, was: "
                << client_num << "\n";
      }
      client_num = HEGEMON;
   }

   if ((client_tag_num < 0) || (client_tag_num >= NUM_TAGS)) {
      if (mudlog.ofLevel(LS_ERROR)) {
         mudlog << "ERROR:  Client tag: " << client_tag_num << " is out of range!!\n";
      }
      return ""; //This could still crash us, if the tag should have had a %? in it!!
   }

   // good to go
   return client_tags[client_num][client_tag_num];
}
        

