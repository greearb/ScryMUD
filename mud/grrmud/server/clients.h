#ifndef __ClientsInclude__H
#define __ClientsInclude__H

#define NO_CLIENT 0  // See comments in .cc file, this is effectively disabled for now.
#define HEGEMON 1
#define NUM_CLIENTS 2 //hegemon and the non-client
#define NUM_TAGS 51


const char* getClientTag(int client_num, int client_tag_num);


#define CTAG_ENGAGE_CLIENT(a) getClientTag(a, 0)
#define CTAG_PROMPT(a) getClientTag(a, 1)
#define CTAG_BEEP(a) getClientTag(a, 2)
#define CTAG_BATTLE(a) getClientTag(a, 3)
#define CTAG_END_BATTLE(a) getClientTag(a, 4)
#define CTAG_SAY(a) getClientTag(a, 5)
#define CTAG_END_SAY(a) getClientTag(a, 6)
#define CTAG_TELL(a) getClientTag(a, 7)
#define CTAG_END_TELL(a) getClientTag(a, 8)
#define CTAG_YELL(a) getClientTag(a, 9)
#define CTAG_END_YELL(a) getClientTag(a, 10)
#define CTAG_GOSSIP(a) getClientTag(a, 11)
#define CTAG_END_GOSSIP(a) getClientTag(a, 12)
#define CTAG_AUCTION(a) getClientTag(a, 13)
#define CTAG_END_AUCTION(a) getClientTag(a, 14)
#define CTAG_RM_SHORT_DESC(a) getClientTag(a, 15)
#define CTAG_END_RM_SHORT_DESC(a) getClientTag(a, 16)
#define CTAG_RM_DESC(a) getClientTag(a, 17)
#define CTAG_END_RM_DESC(a) getClientTag(a, 18)
#define CTAG_KEYWORD_DESC(a) getClientTag(a, 19)
#define CTAG_END_KEYWORD_DESC(a) getClientTag(a, 20)
#define CTAG_EXITS(a) getClientTag(a, 21) 
#define CTAG_MOB_LIST(a) getClientTag(a, 22)
#define CTAG_END_MOB_LIST(a) getClientTag(a, 23)
#define CTAG_ITEM_LIST(a) getClientTag(a, 24)
#define CTAG_END_ITEM_LIST(a) getClientTag(a, 25)
#define CTAG_DOOR_LIST(a) getClientTag(a, 26)
#define CTAG_END_DOOR_LIST(a) getClientTag(a, 27)
#define CTAG_SCRIPT_DATA(a) getClientTag(a, 28)
#define CTAG_END_SCRIPT_DATA(a) getClientTag(a, 29)
#define CTAG_MOB_SCRIPT(a) getClientTag(a, 30)
#define CTAG_ROOM_SCRIPT(a) getClientTag(a, 31)
#define CTAG_OBJECT_LIST(a) getClientTag(a, 32)
#define CTAG_DISCRIM(a) getClientTag(a, 33)
#define CTAG_STAT(a) getClientTag(a, 34)
#define CTAG_END_STAT(a) getClientTag(a, 35)
#define CTAG_STAT_SD(a) getClientTag(a, 36)
#define CTAG_STAT_ND(a) getClientTag(a, 37)
#define CTAG_STAT_LD(a) getClientTag(a, 38)
// If it's broken, plz don't allow it in the code just yet. --Ben
//#define CTAG_NAMES(a) getClientTag(a, 39)
#define CTAG_VALUE_ITEM(a) getClientTag(a, 40)
#define CTAG_VALUE_INAME(a) getClientTag(a, 41)
#define CTAG_BUG_ENTRY(a) getClientTag(a, 42)
#define CTAG_END_BUG_ENTRY(a) getClientTag(a, 43)
#define CTAG_BUG_TITLE(a) getClientTag(a, 44)
#define CTAG_END_BUG_TITLE(a) getClientTag(a, 45)
#define CTAG_OSTAT(a) getClientTag(a, 46)
#define CTAG_MSTAT(a) getClientTag(a, 47)
#define CTAG_PSTAT(a) getClientTag(a, 48)
#define CTAG_PATH_DIR(a) getClientTag(a, 49)
#define CTAG_PATH_IS_DEST(a) getClientTag(a, 50)




extern const char* client_tags[NUM_CLIENTS][NUM_TAGS];
#endif
