// $Id$
// $Revision$  $Author$ $Date$

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

///**********************  grrmud.cc  ***************************///
/* holds main function, drives the rest of the MUD */

#ifdef USEMYSQL
#include <mysql/mysql.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <limits.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <time.h>

#include <unistd.h>

#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <strings.h>
#include <string2.h>
#include <list2.h>
#include <PtrArray.h>
#include "classes.h"
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "spec_prc.h"
#include "const.h"
#include "grrmud.h"
#include "load_wld.h"
#include "login.h"
#include <LogStream.h>
#include "SkillSpell.h"
#include "BuildInfo.h"
#include "BugEntry.h"
#include "Filters.h"
#include "ServerConfig.h"
#include "spells.h"
#include <new>

#include "telnet.h"
#include "telnet_handler.h"

#define MAX_HOSTNAME    256

int initCmdsCollection(); //found in parse_gen.cc
int initCmdsArray();      //found in parse_gen.cc

/* local globals */
int     do_shutdown = FALSE;        /* clean shutdown */
int     grr_reboot;                 /* reboot status */
int     mother_desc = 0;            /* file desc of the mother connection */
int     avail_descs;                /* max descriptors available */
int     sockets_connected = 0;
int     First_Room = 0;
int     Last_Room = 100;
int     BOOT_TIME = 0;

//Global Variables!!

// These are used to track usage of the List<T> classes.
int __node_cnt = 0;
int __list_cnt = 0; 
int __cell_cnt = 0;

#ifdef USEMYSQL
MYSQL* database;
#endif

ServerConfig config;

LogStream            mudlog("./log/logfile.txt", ERROR | DIS | WRN | LSEC | DB | DBG);
//LogStream            mudlog("./log/logfile.txt", ERROR | DIS | WRN | LSEC | DB | DBG, LogStream::APPEND);
//LogStream            obj_ptr_log("./log/obj_ptr.log", ALL, LogStream::APPEND);
LogStream            obj_ptr_log("./log/obj_ptr.log", ERROR | DIS); // turn this off, for now.

PtrList<String>        banned_hosts;
class critter        mob_list[NUMBER_OF_MOBS + 1];
class object         obj_list[NUMBER_OF_ITEMS + 1];
class door_data      door_list[NUMBER_OF_DOORS + 1];
LazyPtrArray<room>   room_list;


BugCollection bl_ideas("./World/BC_IDEAS", "Idea Listing",
                       BugCollection::IDEAS);
BugCollection bl_bugs("./World/BC_BUGS", "Bug Listing",
                      BugCollection::BUGS);

PtrList<critter>        linkdead_list;
PtrList<critter>        pc_list;
PtrList<critter>        new_pc_list;
PtrList<room>           embattled_rooms; 

PtrList<door>              affected_doors;
PtrList<critter>        affected_mobs; 
PtrList<room>           affected_rooms; 
PtrList<object>         affected_objects; 


   /* a list of mobs currently doing
      mob procs, will get a command 
      off of their queues untill queues are empty. */
PtrArray<critter>        proc_action_mobs;
PtrArray<room>           proc_action_rooms;
PtrArray<object>         proc_action_objs;


PtrList<room>           pulsed_proc_rooms; 
PtrList<object>         pulsed_proc_objects; 
PtrList<object>               obj_to_be_disolved_list;

int Cur_Max_Obj_Num = 0; //these are useful for OLC and saving SOBJs 
int Cur_Max_Crit_Num = 0;
int Cur_Max_Door_Num = 0;
int Cur_Max_Room_Num = 0; 

bitfield Obj_Wear_Flags_Mask;
//bitfield Obj_Wear_Procs_Mask;
//bitfield Obj_Remove_Procs_Mask;
bitfield Obj_Consume_Procs_Mask;
//bitfield Shop_Data_Buy_Procs_Mask;
//bitfield Shop_Data_Sell_Procs_Mask;
//bitfield Shop_Data_Offer_Procs_Mask;

/* these make certain functions constant time instead of linear or log(n) */

int
   ABSORB_BLOWS_SKILL_NUM,
   ACROBATICS_SKILL_NUM,
   ANCIENT_LANGUAGES_SKILL_NUM,
   ALCHEMY_SKILL_NUM,
   ARCHERY_SKILL_NUM,
   ARMOR_SKILL_NUM,
   BACKSTAB_SKILL_NUM,
   BALLISTICS_SKILL_NUM,
   BASH_DOOR_SKILL_NUM,
   BASH_SKILL_NUM,
   BERZERK_SKILL_NUM,
   BIND_WOUND_SKILL_NUM,
   BLACKSMITHING_SKILL_NUM,
   BLEND_SKILL_NUM,
   BLESS_SKILL_NUM,
   BLINDNESS_SKILL_NUM,
   BLOCK_SKILL_NUM,
   BLOCK_DOOR_SKILL_NUM,
   BODYSLAM_SKILL_NUM,
   BOW_SKILL_NUM,
   BRAWLING_SKILL_NUM,
   BREW_SKILL_NUM,
   BURNING_HANDS_SKILL_NUM,
   BUTCHER_SKILL_NUM,
   CALM_SKILL_NUM,
   CAMOUFLAGE_SKILL_NUM,
   CAMPING_SKILL_NUM,
   CAUSE_CRITICAL_SKILL_NUM,
   CAUSE_SICKNESS_SKILL_NUM,
   CHARM_SKILL_NUM,
   CHANNELLING_SKILL_NUM,
   CIRCLE_SKILL_NUM,
   CLAW_SKILL_NUM,
   CLIMBING_SKILL_NUM,
   COMMERCE_SKILL_NUM,
   CONSTRUCT_SKILL_NUM,
   CONJURING_SKILL_NUM,
   CONJURE_HORDE_SKILL_NUM,
   CONJURE_MINION_SKILL_NUM,
   CURE_SKILL_NUM,
   CREATE_GOLEM_SKILL_NUM,
   CREATE_GREATER_GOLEM_SKILL_NUM,
   CREATE_LIGHT_SKILL_NUM,
   CREATE_FOOD_SKILL_NUM,
   CREATE_WATER_SKILL_NUM,
   CREATION_SKILL_NUM,
   CRITICAL_STRIKE_SKILL_NUM,
   CURE_BLINDNESS_SKILL_NUM,
   CURE_CRITICAL_SKILL_NUM,
   CURE_SERIOUS_SKILL_NUM,
   CURSE_SKILL_NUM,
   DAGGER_SKILL_NUM,
   DETECTION_SKILL_NUM,
   DETECT_ALIGNMENT_SKILL_NUM,
   DETECT_HIDDEN_SKILL_NUM, 
   DETECT_INVISIBILITY_SKILL_NUM,
   DETECT_MAGIC_SKILL_NUM,
   DETECT_POISON_SKILL_NUM,
   DETECT_RESISTANCES_SKILL_NUM,
   DISARM_SKILL_NUM,
   DISTORTION_WALL_SKILL_NUM,
   DISPEL_MAGIC_SKILL_NUM,
   DISPEL_GOOD_SKILL_NUM,
   DISPEL_EVIL_SKILL_NUM,
   DIVINE_PROTECTION_SKILL_NUM,
   DODGE_SKILL_NUM,
   DOD_SKILL_NUM,
   DOOR_BASH_SKILL_NUM,
   DUAL_WIELD_SKILL_NUM,
   EARTHMELD_SKILL_NUM,
   ELEMENTALISM_SKILL_NUM,
   ENCHANTMENT_SKILL_NUM,
   ENCHANT_ARMOR_SKILL_NUM,
   ENCHANT_WEAPON_SKILL_NUM,
   ENHANCED_DAMAGE_SKILL_NUM,
   ENTANGLE_SKILL_NUM,
   FAERIE_FIRE_SKILL_NUM,
   FENCING_SKILL_NUM,
   FIRE_BLADE_SKILL_NUM,
   FIREBALL_SKILL_NUM,
   FIREPROOF_SKILL_NUM,
   FIRESTORM_SKILL_NUM,
   FIREWALL_SKILL_NUM,
   FLAME_STRIKE_SKILL_NUM,
   FLESH_TO_STONE_SKILL_NUM,
   FLY_SKILL_NUM,
   FORESTRY_SKILL_NUM,
   FROST_BLADE_SKILL_NUM,
   GATE_SKILL_NUM,
   GROUP_HEAL_SKILL_NUM,
   GUARD_SKILL_NUM,
   HARM_SKILL_NUM,
   HARMING_SKILL_NUM,
   HASTE_SKILL_NUM,
   HEAL_SKILL_NUM,
   HEALING_SKILL_NUM,
   HERBALISM_SKILL_NUM,
   HEROS_FEAST_SKILL_NUM,
   HIDE_SKILL_NUM,
   HONOR_CODE_SKILL_NUM,
   HOLY_WORD_SKILL_NUM,
   HURL_SKILL_NUM,
   ICESTORM_SKILL_NUM,
   IDENTIFY_SKILL_NUM,
   ILLUMINATE_SKILL_NUM,
   ILLUSION_SKILL_NUM,
   INFRAVISION_SKILL_NUM,
   INVISIBILITY_SKILL_NUM,
   KICK_SKILL_NUM,
   LEADERSHIP_SKILL_NUM,
   LIGHTNING_SKILL_NUM,
   LIGHTNING_STORM_SKILL_NUM,
   LITERACY_SKILL_NUM,
   LOCATE_SKILL_NUM,
   LOGIC_SKILL_NUM,
   LORE_SKILL_NUM,
   MACE_SKILL_NUM,
   MAGIC_SHIELD_SKILL_NUM,
   MANA_SKILL_NUM,
   MARTIAL_ARTS_SKILL_NUM,
   MASS_CHARM_SKILL_NUM,
   MEDITATION_SKILL_NUM,
   METEORSTORM_SKILL_NUM,
   MIRROR_IMAGE_SKILL_NUM,
   NECROMANCY_SKILL_NUM,
   ORB_OF_POWER_SKILL_NUM,
   PARRY_SKILL_NUM,
   PASSDOOR_SKILL_NUM,
   PHYSICAL_ARTS_SKILL_NUM,
   PHYSIK_SKILL_NUM,
   PFE_SKILL_NUM,
   PFG_SKILL_NUM,
   PHILOSOPHY_SKILL_NUM,
   PICKLOCK_SKILL_NUM,
   PLAGUE_SKILL_NUM,
   POISON_SKILL_NUM,
   PORTAL_SKILL_NUM,
   PRISMATIC_GLOBE_SKILL_NUM,
   PROTECTION_SKILL_NUM,
   QUAKE_SKILL_NUM,
   QUICKFOOT_SKILL_NUM,
   RAINBOW_SKILL_NUM,
   RAISE_UNDEAD_SKILL_NUM,
   RECALL_SKILL_NUM,
   RECHARGE_SKILL_NUM,
   RELIGION_SKILL_NUM,
   REMOVE_CURSE_SKILL_NUM,
   REMOVE_POISON_SKILL_NUM,
   RESCUE_SKILL_NUM,
   RESTORE_SKILL_NUM,
   RUNE_EDGE_SKILL_NUM,
   SANCTUARY_SKILL_NUM,
   SCAN_SKILL_NUM,
   SCRIBE_SKILL_NUM,
   SCROLLS_SKILL_NUM,
   SCRYING_SKILL_NUM,
   SECOND_ATTACK_SKILL_NUM, 
   SHADOWS_BLESSING_SKILL_NUM,
   SHIELD_SKILL_NUM,
   SHOCKING_GRASP_SKILL_NUM,
   SKIN_SKILL_NUM,
   SLEEP_SKILL_NUM,
   SNEAK_SKILL_NUM,
   SOBER_SKILL_NUM,
   SOD_SKILL_NUM,
   STEAL_SKILL_NUM,
   STRENGTH_SKILL_NUM,
   STRENGTH_CONDITIONING_SKILL_NUM,
   STONE_SKIN_SKILL_NUM,
   SUMMON_SKILL_NUM,
   SWORD_SKILL_NUM,
   SWORDBOND_SKILL_NUM,
   TAIL_SKILL_NUM,
   TAMMUZ_SKILL_NUM,
   TELEPORT_SKILL_NUM,
   THROWING_SKILL_NUM,
   TORNADO_SKILL_NUM,
   TRACK_SKILL_NUM,
   TRIP_SKILL_NUM,
   TYPHOON_SKILL_NUM,
   WEAKEN_SKILL_NUM,
   WEAPON_MASTERY_SKILL_NUM,
   WEB_SKILL_NUM,
   WHIP_SKILL_NUM,
   WIZARD_EYE_SKILL_NUM,
   WRESTLING_SKILL_NUM,
// New skills for the Avian/Dragon race
// [RJY] - Trice
   HOVER_SKILL_NUM,
   WING_POWER_SKILL_NUM,
   SECRET_FEATHER_SKILL_NUM,
   SHRIEK_SKILL_NUM,
   CARRY_SKILL_NUM,
   PECK_SKILL_NUM,
   TAILSWEEP_SKILL_NUM,
   VISION_SKILL_NUM,
   DIVE_SKILL_NUM,
   GLARE_SKILL_NUM,
   WINDZONE_SKILL_NUM,
   PLUCK_SKILL_NUM,
   BIRDSEYE_SKILL_NUM,
   // New skills for the Necromancer class
   CURSING_SKILL_NUM,
   RUST_SKILL_NUM,
   DISFAVOR_SKILL_NUM,
   REMOVE_SOUL_SKILL_NUM,
   REMOVE_HOPE_SKILL_NUM,
   REMOVE_KARMA_SKILL_NUM,
   SANCTUM_OF_THE_VICTIM_SKILL_NUM,
   FEAR_SKILL_NUM,
   NECROPHILIA_SKILL_NUM;


               /* end of global variables */



/* *********************************************************************
*  main game loop and related stuff                                    *
********************************************************************* */

char* extra_memory = NULL;

void scry_new_handler() {
   free(extra_memory);
   core_dump("scry_bad_alloc.");
}


//hopefully trap the TERM signal and exit gracefully
void sig_term_handler(int signo) { 
   //reestablish signal handler
   if ((signo != SIGSEGV) && (signo != SIGBUS) && (signo != SIGIOT)) {
      cerr << "Got signal: " << signo << " NOT setting to default handler." << endl;
      signal(signo, (&sig_term_handler));
   }
   else {
      cerr << "Got signal: " << signo << " setting to default handler." << endl;
      signal(signo, SIG_DFL);
   }

   if (signo == SIGTERM) {
      mudlog << "Got SIGTERM, shutting down.\n";
      mudlog << flush;
      do_shutdown = 1;
   }
   else if (signo == SIGALRM) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGALRM, ignoring!!\n";
      mudlog << flush;
   }
   else if (signo == SIGHUP) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGHUP, ignoring!!\n";
      mudlog << flush;
   }
   else if (signo == SIGIO) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGIO, ignoring!!\n";
      mudlog << flush;
   }
   else if (signo == SIGPIPE) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGPIPE, ignoring!!\n";
      mudlog << flush;
   }
//   else if (signo == SIGPOLL) {
//      if (mudlog.ofLevel(DIS))
//         mudlog << "Got SIGPOLL, ignoring!!\n";
//      mudlog << flush;
//   }
   else if (signo == SIGSTOP) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGSTOP, shutting down..";
      mudlog << flush;
      do_shutdown = 1;
   }
   else if (signo == SIGINT) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGINT, shutting down..";
      mudlog << flush;
      do_shutdown = 1;
   }
   else if (signo == SIGTTIN) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGTTIN, ignoring!!\n";
      mudlog << flush;
   }
   else if (signo == SIGTTOU) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGTTOU, ignoring!!\n";
      mudlog << flush;
   }
   else if ((signo == SIGUSR1) || (signo == SIGUSR2)) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGUSR1 or SIGUSR2, ignoring!!\n";
      mudlog << flush;
   }
   else if (signo == SIGVTALRM) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGVTALRM, ignoring!!\n";
      mudlog << flush;
   }
   else if (signo == SIGXCPU) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGXCPU, shutting down..!!\n";
      mudlog << flush;
      do_shutdown = TRUE;
   }
   else if (signo == SIGXFSZ) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGXFSZ, shutting down..!!\n";
      mudlog << flush;
      do_shutdown = TRUE;
   }
   else if ((signo == SIGSEGV) || (signo == SIGIOT)) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGSEGV or SIGIOT (" << signo << "), shutting down..!!\n";
      mudlog << flush;
      char buf[100];
      sprintf(buf, "%s.CRASH.%i", mudlog.getFileName(), getpid());
      mudlog.cpFileTo(buf);
      sprintf(buf, "%s.CRASH.%i", obj_ptr_log.getFileName(), getpid());
      obj_ptr_log.cpFileTo(buf);
      abort();
      //do_shutdown = TRUE;
   }
   else if (signo == SIGBUS) {
      if (mudlog.ofLevel(DIS))
         mudlog << "Got SIGBUS, shutting down..!!\n";
      mudlog << flush;
      char buf[100];
      sprintf(buf, "%s.CRASH.%i", mudlog.getFileName(), getpid());
      mudlog.cpFileTo(buf);
      sprintf(buf, "%s.CRASH.%i", obj_ptr_log.getFileName(), getpid());
      obj_ptr_log.cpFileTo(buf);
      abort();
      //do_shutdown = TRUE;
   }
   else {
      mudlog << "Got unknown SIGNAL:  " << signo 
             << " shutting down..." << endl;
      mudlog << flush;
      do_shutdown = TRUE;
   }      
      
}//



int main(int argc, char** argv) {
   String dummy;
   dummy.setLogFile(&mudlog);
   //String::setLogFile(&mudlog);
   extra_memory = (char*)(malloc(20000));

   String buf(50);

   room_list.ensureCapacity(NUMBER_OF_ROOMS + 1);

   BOOT_TIME = time(NULL);

   cout << "ScryMUD version: " << BuildInfo::getVersion() 
      << " (" << BuildInfo::getRevision() << ")" << endl;
   cout << "Built on:        " << BuildInfo::getBuildDate() << endl;
   cout << "On machine:      " << BuildInfo::getBuildMachine() << endl;
   cout << "By:              " << BuildInfo::getBuilder() << endl << endl;

   cout << "Welcome to SryMUD server by Ben Greear.\n"
      "This server and it's accompanying client, Hegemon, is distributed\n"
      "under the GNU General Public License (GPL).  See the License file\n"
      "for more information.\n"
      "\n"
      "Be sure to check the log files in the ./log directory if you have\n"
      "any strange occurances.\n"
      "\n"
      "--Ben Greear (greear@cyberhighway.net, greearb@agcs.com)\n\n";

   srand(time(0));

   config.read("grrmud.cfg");
   config.readDynamic("dynamic.cfg");
   CmdLineInfo::init(argc, argv);

   // Daemonize if we're supposed to.
   if ( config.daemonize ) {
      pid_t     pid;
      if ( ( pid = fork() ) == -1 ) {
         cout << "Couldn't Fork. Quitting." << endl;
         exit(-1);
      } 
      if ( pid > 0 ) {
         // Kill the parent
         do_shutdown = TRUE;
         return(0);
      }
      // The child
      setsid();
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);
   } else {
   } // config.daemonize

   // Write out our PID to grrmud.pid
   std::ofstream pid_file("grrmud.pid");
   pid_file << getpid();
   pid_file.close();
   pid_file.clear();

   #ifdef USEMYSQL
   if (config.useMySQL) {
      mysql_init(database);
      mysql_options(database, MYSQL_READ_DEFAULT_GROUP, "gmud");
      if (!mysql_real_connect(database, config.mySQLhost, config.mySQLuser,
                              config.mySQLpassword, config.mySQLdatabase,
                              config.mySQLport, NULL, 0)) {
         cout << "Couldn't connect to mySQL: " << mysql_error(database) << endl;
         cout << "Shutting down.\n";
         exit(0);
      }
   }
   #endif

   // run the auto-generated methods that load all of our commands
   // into the commands collection and arrays. (see parse_gen.cc)
   initCmdsCollection();
   initCmdsArray();

   readSiteBanned();

   // setup signal handling AFTER we (potentially) daemonize
   signal(SIGTERM, (&sig_term_handler));
   signal(SIGIO, (&sig_term_handler));
   signal(SIGPIPE, (&sig_term_handler));
   signal(SIGQUIT, (&sig_term_handler));
   /* Trap a few beasties, but only once!! */
   signal(SIGSEGV, (&sig_term_handler));
   std::set_new_handler(scry_new_handler);

   // Drop into our endless loop.
   run_the_game(config.port);

   config.writeDynamic("dynamic.cfg");

   if (grr_reboot >= 1) {
      system("touch .reboot");
   }//if
   else {
      system("rm -f .reboot");
   }

   do_shutdown = TRUE;


   // Start cleaning up.

   // room destructors handle any smobs in the rooms.
   //
   // critter destructors will wack objects in smob
   // inventories.
   //
   // object destructors will clean up their SOBJ's
   // (and thus recursively cleanup)
   //
   // And lastly... I don't think LazyPtrArray implements
   // clearAndDestroy() nor does it have a "real" destructor,
   // at least not one that I found in my quick survey.
   for(int i=0;i<NUMBER_OF_ROOMS;i++) {
       room* r_ptr = room_list.elementAtNoCreate(i);
       if ( r_ptr ) {
           delete r_ptr;
       }
   }

   pc_list.clearAndDestroy();

   return (grr_reboot);
}


/* Init sockets, run game, and cleanup sockets */

void run_the_game(int port) {
   int  s;

   if (mudlog.ofLevel(DIS))
      mudlog << "Opening mother connection.\n";

   mother_desc = s = init_socket(port);

   // If configured to do so, drop permissions as soon as we open the socket.
   if ( config.suid ) {
      if ( setgid(config.suid_group) == -1 ) {
         cout << "Couldn't setgid. Quitting." << endl;
         exit(-1);
      }
      if ( setuid(config.suid_user) == -1 ) {
         cout << "Couldn't setuid. Quitting." << endl;
         exit(-1);
      }
   }
   
   system("rm -f .reboot"); //if it crashes in this next bit stay down,
                         //the world files are corrupt probably

   config.currentLoadModifier = config.bootLoadModifier;
   load_wld(); //read in universe

   ZoneList::instance().readSelf();
   ZoneList::instance().execute(); //modify zones as needed
   // ZoneCollection::instance().createMapFiles();

   // Read in the bug and idea list.
   bl_bugs.read();
   bl_ideas.read();

   config.currentLoadModifier = config.regularLoadModifier;

   system("touch .reboot");

   if (config.convertWorldFromDev) {
      config.convertWorldFromDev = false;
      write_all_zones();

      // Write out un-directed graphs for all zones.
      // This is currently broken. --Ben
      //ZoneCollection::instance().createNeatoFiles();

      ofstream ps_dot_script("./World/ss_ps.dot");
      ofstream gif_dot_script("./World/ss_gif.dot");
   
      if (ps_dot_script) {
         ps_dot_script << SSCollection::instance().generatePsDotScript();
      }
      else {
         mudlog << "ERROR: could not open ./World/ss_ps.dot" << endl;
      }

      if (gif_dot_script) {
         gif_dot_script << SSCollection::instance().generateGifDotScript();
      }
      else {
         mudlog << "ERROR: could not open ./World/ss_gif.dot" << endl;
      }
      
      ofstream ss_html("./World/ss_html.html");
      
      if (ss_html) {
         ss_html << SSCollection::instance().generateHtml();
      }
      else {
         mudlog << "ERROR: could not open ./World/ss_html.html" << endl;
      }

      grr_reboot = 1;
      return;
   }//if


   init_masks();   //set all the masks for miscelaneous bitfields


   mudlog.log(DBG, "Entering game loop.\n");


   config_spells(); // configure spells
   game_loop(s);

   close_sockets(s);
   mudlog.log(DIS, "Normal termination of game.\n");
}//run_the_game



/* Accept new connects, relay commands, and call 'heartbeat-functs' */

void game_loop(int s)  {
   fd_set input_set, output_set, exc_set;
   struct timeval last_time, now, timespent, timeout, null_time;
   struct timeval time_since_pulse;
   static struct timeval opt_time;
   Cell<critter*> pc_cell;
   critter* pc_ptr;
   critter* tmp_ptr;
   int i, pulse = 1;
   int maxdesc = s;
   short did_pulse = FALSE;
   String buf(100);
   String prompt(100);

//   log("Beginning of game_loop\n");

   /* initialize the timezone tz */

   //init.tz_minuteswest = 6*60;
   //init.tz_dsttime = DST_USA;

   null_time.tv_sec = 0;
   null_time.tv_usec = 0;

   time_since_pulse.tv_sec = 0;
   time_since_pulse.tv_usec = 0;

   opt_time.tv_usec = config.optUsec;  /* Init time values */
   opt_time.tv_sec = 0;
   
   // see the manpage, gettimeofday() should always be passed NULL for
   // struct timezone *tz
   gettimeofday(&last_time, NULL);

   avail_descs = config.maxPlayers;

   /* Main loop */
   while (!do_shutdown) {

      /* add new pc's to game */
      new_pc_list.head(pc_cell);
      pc_ptr = pc_cell.next();
      while (pc_ptr) {
         pc_list.gainData(pc_ptr);
         pc_ptr = new_pc_list.lose(pc_cell);
      }//while

      FD_ZERO(&input_set);
      FD_ZERO(&output_set);
      FD_ZERO(&exc_set);
      FD_SET(s, &input_set);

      maxdesc = s;
      pc_list.head(pc_cell);
      int desc;
      while ((pc_ptr = pc_cell.next())) {
         if ((pc_ptr->MODE == MODE_GO_LINKDEAD_PLEASE) ||
             (pc_ptr->MODE == MODE_LOGOFF_NEWBIE_PLEASE) ||
             (pc_ptr->MODE == MODE_QUIT_ME_PLEASE)) {
            continue;
         }

         desc = pc_ptr->getDescriptor();
         if ((desc < 0) || (desc > config.maxPlayers)) {
            mudlog << "ERROR:  descriptor out of range:  " << desc
                   << " on critter:  " << *(pc_ptr->getName())
                   << endl;
         }
         else {
            if (desc > maxdesc) {
               maxdesc = desc;
            }
            FD_SET(desc, &input_set);
            FD_SET(desc, &exc_set);
            FD_SET(desc, &output_set);
         }//else
      }//while

      //log("check out the time.\n");

      gettimeofday(&now, NULL);
      timespent = timediff(&now, &last_time);
      timeout = timediff(&opt_time, &timespent);
      last_time.tv_sec = now.tv_sec + timeout.tv_sec;
      last_time.tv_usec = now.tv_usec + timeout.tv_usec;
      if (last_time.tv_usec >= 1000000) {
         last_time.tv_usec -= 1000000;
         last_time.tv_sec++;
      }//if

      time_since_pulse.tv_usec += last_time.tv_usec;
      time_since_pulse.tv_sec += last_time.tv_sec;
      if (time_since_pulse.tv_usec >= 1000000) {
         time_since_pulse.tv_usec -= 1000000;
         time_since_pulse.tv_sec++;
      }//if

      if (time_since_pulse.tv_sec >= 1) {
         pulse++;
         time_since_pulse.tv_usec = 0;
         time_since_pulse.tv_sec = 0;
         did_pulse = TRUE;
      }//if

      //log("Doing log-offs.\n");

                       /* do log_offs of all types */

      pc_list.head(pc_cell);
      pc_ptr = pc_cell.next();
      while (pc_ptr) {
         if (pc_ptr->getMode() == MODE_GO_LINKDEAD_PLEASE) {
            mudlog << "Logging off via MODE_GO_LINKDEAD_PLEASE." << endl;
            tmp_ptr = pc_ptr;
            pc_ptr = pc_list.lose(pc_cell);
            tmp_ptr->doGoLinkdead(); //do not delete the critter!
         }//if
         else if (pc_ptr->getMode() == MODE_LOGOFF_NEWBIE_PLEASE) {
            mudlog << "Logging off via MODE_LOGOFF_NEWBIE_PLEASE." << endl;
            tmp_ptr = pc_ptr;
            pc_ptr = pc_list.lose(pc_cell);
            tmp_ptr->doLogOffNewLogin();
            delete tmp_ptr->pc->p_handler;
            delete tmp_ptr;
         }//if
         else if (pc_ptr->getMode() == MODE_QUIT_ME_PLEASE)  {
            mudlog << "Logging off via MODE_QUIT_ME_PLEASE." << endl;
            tmp_ptr = pc_ptr;
            pc_ptr = pc_list.lose(pc_cell);
            if (tmp_ptr->pc->link_condition == CON_LOGGING_IN) { //special case
               tmp_ptr->doLogOffNewLogin();
            }
            else {
               tmp_ptr->doLogOffActive();
            }
            delete tmp_ptr->pc->p_handler;
            delete tmp_ptr;
         }//else
         else {
            pc_ptr = pc_cell.next();
         }//else
      }//while pc_ptr

      linkdead_list.head(pc_cell);
      pc_ptr = pc_cell.next();
      while (pc_ptr) {
         if ((pc_ptr->getMode() == MODE_LOG_OFF_LINKDEAD_PLEASE) ||
             (pc_ptr->getMode() == MODE_QUIT_ME_PLEASE)) {
            mudlog << "Logging off via MODE_LOG_OFF_LINKDEAD_PLEASE." << endl;
            tmp_ptr = pc_ptr;
            pc_ptr = linkdead_list.lose(pc_cell);
            if (tmp_ptr->pc->link_condition == CON_LOGGING_IN) { //special case
               tmp_ptr->doLogOffNewLogin();
            }
            else {
               tmp_ptr->doLogOffInactive();
            }
            delete tmp_ptr->pc->p_handler;
            delete tmp_ptr;
         }//else
         else {
            pc_ptr = pc_cell.next();
         }//else
      }//while pc_ptr

      //log("Doing pulsed procs.\n");
      /* pulsed processes */

      if (did_pulse) {
         if ((pulse % 11) == 0) {
            if (!embattled_rooms.isEmpty()) { 
               do_battle();
            }
            do_mini_tick(); //decrements pause ect.
         }//if

         // Will try to pulse the entire MUD ever 20 seconds.
         // We will pulse one-tenth at a time (room wise).
         if ((pulse % 10) == 0) {
           do_pulsed_spec_procs(First_Room, Last_Room);
         }
         if ((pulse % 63) == 0) {
            do_regeneration_pcs();
            do_regeneration_smobs();
         }
         if ((pulse % 379) == 0) {
            do_tick(); //takes care of zones too
         }
         
         if (((pulse + 1) % 1000) == 0) {
           save_all();  //save all, about every 5 ticks
         }//if

         // trim the log files to the last 1000 lines, start adding from there.
         if (((pulse + 1) % 10000) == 0) {
            //mudlog << "WARNING:  About to truncate.\n" << endl;
            mudlog.rollFile();
         }
      }//if
      
       //log("Calling select, poll\n");
      
      null_time.tv_sec = 0;
      null_time.tv_usec = 0;

      if (select(maxdesc + 1, &input_set, &output_set, &exc_set, &null_time)
           < 0) {
         if (errno == EBADF) {
            mudlog << "ERROR:  bad file desc. given in a set to select.\n";
            do_shutdown = TRUE;
            exit(100);
         }//if
         else if (errno == EINTR) {
            mudlog.log(DIS, "ERROR:  A non blocked signal was caught.\n");
         }//if
         else {
               if (mudlog.ofLevel(DIS)) {
               mudlog << "ERROR:  select, poll:  " << strerror(errno) << endl;
            }
            do_shutdown = TRUE;
            exit(100);
         }//else
      }//if

       //log("calling select, sleep\n");

      if (select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout) < 0) {
         if (errno == EBADF) {
            mudlog << "ERROR:  bad file desc. given in a set to select.\n";
            do_shutdown = TRUE;
            exit(100);
         }//if
         else if (errno == EINTR) {
            mudlog.log(DIS, "ERROR:  A non blocked signal was caught.\n");
         }//if
         else {
               if (mudlog.ofLevel(DIS)) {
               mudlog << "ERROR:  select, sleep:  " << strerror(errno) << endl;
            }
            do_shutdown = TRUE;
            exit(100);
         }//else
      }//if


      /* New connection? */
      if (FD_ISSET(s, &input_set)) {
         if (critter::createWithDescriptor(s) < 0) { //set password and descriptor
            mudlog << "ERROR:  new connection:  " << strerror(errno) << endl;
         }//if
      }//if

         //log("Losing link freaks.\n");

                       /* kick out link freaks */
      pc_list.head(pc_cell);
      while ((pc_ptr = pc_cell.next())) {
         if (FD_ISSET(pc_ptr->getDescriptor(), &exc_set)) {
            FD_CLR(pc_ptr->getDescriptor(), &input_set);
            FD_CLR(pc_ptr->getDescriptor(), &output_set);
            mudlog << "WARNING:  pc -:" << pc_ptr->getName() << " going linkdead because of exc_set.\n";
            pc_ptr->pc->SocketProblem();
         }//if
      }//while

      //log("Get input.\n");
      /* get input */
      pc_list.head(pc_cell);
      while ((pc_ptr = pc_cell.next())) {
         if ((pc_ptr->MODE != MODE_GO_LINKDEAD_PLEASE) &&
               (pc_ptr->MODE != MODE_LOGOFF_NEWBIE_PLEASE) &&
               (pc_ptr->MODE != MODE_QUIT_ME_PLEASE)) {
            if (FD_ISSET(pc_ptr->getDescriptor(), &input_set)) {
               pc_ptr->readInput();
            }//if
         }//if
      }//while

      int len;
                        /* process input */
      pc_list.head(pc_cell);
      while ((pc_ptr = pc_cell.next())) {
         if ((pc_ptr->MODE != MODE_GO_LINKDEAD_PLEASE) && 
             (pc_ptr->MODE != MODE_LOGOFF_NEWBIE_PLEASE) &&
             (pc_ptr->MODE != MODE_QUIT_ME_PLEASE)) {

            len = pc_ptr->getInput()->Strlen();
            if (len && ((pc_ptr->getInput()->charAt(len - 1) == '\n') ||
                     pc_ptr->getInput()->Contains('\n')) || (len >= MAX_INPUT_LEN)) {
               if (mudlog.ofLevel(DBG)) {
                  mudlog << "processing input for: " << *(pc_ptr->getName())
                     << ",  cmd -:" << *(pc_ptr->getInput()) << ":- " << endl;
               }

               critter* snooper;
               if ((snooper = pc_ptr->SNOOPED_BY)) {
                  mudlog.log(TRC, "Within snoop if\n");
                  String buf2(100);
                  Sprintf(buf2, "SNOOP_IN: %S:  %S\n",
                        pc_ptr->getName(snooper->SEE_BIT), pc_ptr->getInput());
                  snooper->show(buf2);
               }//if snoop

               if (pc_ptr->possessing) {
                  String cmd1 = pc_ptr->getInput()->Look_Command();
                  cmd1.Strip();
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Is possessing, cmd1: " << cmd1 << endl;
                  }
                  if (strncasecmp(cmd1, "unpossess", strlen("unpossess")) == 0) {
                     if (mudlog.ofLevel(DBG)) {
                        mudlog << "Unpossessing..." << endl;
                     }
                     pc_ptr->unPossess();
                     pc_ptr->show("You have stopped possessing that poor soul.\n");
                     short eos, term_by_period;
                     pc_ptr->getInput()->Get_Command(eos, term_by_period);
                  }//if
                  else {
                     pc_ptr->possessing->processInput(*(pc_ptr->getInput()),
                           FALSE, TRUE);
                     if (mudlog.ofLevel(DBG)) {
                        mudlog << "input after processInput: " << *(pc_ptr->getInput())
                           << endl;
                     }//if
                     //pc_ptr->setDoPrompt(TRUE);
                  }
               }//if possessing
               else {
                  pc_ptr->processInput(*(pc_ptr->getInput()), TRUE, FALSE);
               }
            }//else
         }//if
      }//while

       //log("Doing Prompts.\n");
                                     /* do prompts */
      pc_list.head(pc_cell);
      while ((pc_ptr = pc_cell.next())) {
         if (pc_ptr->getMode() == MODE_NORMAL) {
            if (pc_ptr->shouldDoPrompt()) {
               pc_ptr->doPrompt();

                  if (pc_ptr->shouldDoTank()) { //tank graph
                  critter* tank, *victim;
                  if ((victim = pc_ptr->getFirstFighting())) {
                     tank = victim->getFirstFighting();
                     if (tank) {
                        prompt = "\nTank^0: ";

                        // Colorized tank graph
                        if (tank->getHP() >
                              (int)((float)(tank->getHP_MAX())*0.70) ) {
                           prompt += "^g";
                        } else if (tank->getHP() >
                              (int)((float)(tank->getHP_MAX())*0.30) ) {
                           prompt += "^Y";
                        } else {
                           prompt += "^R";
                        }

                        int num_of_stars = (int)(((float)
                                                  (tank->getHP())/
                                                  (float)(tank->getHP_MAX())) *
                                                 40.0);
                        for (i = 0; i < num_of_stars; i++) {
                           prompt += "*";
                        }//for
                        prompt += "\nVict^0: ";
                        
                        // Colorized victim graph
                        // we have a random 30% skew when you yourself are <
                        // 50% health.
                        int victim_hp = victim->getHP();
                        if ( pc_ptr->getHP() <
                                (int)( (float)pc_ptr->getHP_MAX() / 2.0) ) {

                           victim_hp = (int)( (float)victim_hp *
                                 (( 85.0 + (float)d(1,40) ) / 100.0 ) );
                        }

                        if (victim_hp >
                              (int)((float)(victim->getHP_MAX())*0.70) ) {
                           prompt += "^g";
                        } else if (victim_hp >
                              (int)((float)(victim->getHP_MAX())*0.30) ) {
                           prompt += "^Y";
                        } else {
                           prompt += "^R";
                        }

                        num_of_stars = (int)(((float)
                                                  (victim_hp)/
                                                  (float)(victim->getHP_MAX())) *
                                                 40.0);
                        for (i = 0; i < num_of_stars; i++) {
                           prompt += "*";
                        }//for
                        prompt += "\n";

                        pc_ptr->show(prompt);
                     }// if tank
                  }//if victim
               }//if tank graph
               pc_ptr->setDoPrompt(FALSE);
            }//if should do prompt
         }//if in MODE_NORMAL
      }//while

      //log("Doing output.\n");
                                    /* output */
      pc_list.head(pc_cell);
      while ((pc_ptr = pc_cell.next())) {
         if ((pc_ptr->MODE != MODE_GO_LINKDEAD_PLEASE) &&
             (pc_ptr->MODE != MODE_LOGOFF_NEWBIE_PLEASE) &&
             (pc_ptr->MODE != MODE_QUIT_ME_PLEASE)) {
            if (pc_ptr->writeOutput() < 0) { 
               pc_ptr->pc->SocketProblem();
            }//if
         }//if
      }//while

      /*  Mob action procs.  */
      int obj_was_deleted = FALSE;
      ScriptCmd* cmd_ptr;
      for (int cnt = 0; cnt < proc_action_mobs.getCurLen(); cnt++) {
         if (!(pc_ptr = proc_action_mobs.elementAt(cnt))) {
            continue;
         }//if
         else {
            if (pc_ptr->getPause() > 0) {
               continue;
            }

            ScriptCmd* tmp_cmd_ptr;
            // Cast away const'ness, but still use it as if it's const.
            if ((tmp_cmd_ptr = (ScriptCmd*)(pc_ptr->getNextScriptCmd()))) {
               cmd_ptr = new ScriptCmd(*tmp_cmd_ptr);

               // pc_ptr is the script owner.
               MobScript::parseScriptCommand(*cmd_ptr, *pc_ptr, obj_was_deleted);
               delete cmd_ptr;
            }
            else {
               pc_ptr->finishedMobProc();

               if (!pc_ptr->isInProcNow()) {
                  Sprintf(buf, "Done with script..taking off proc_action_mobs, idx: %i\n",
                          cnt);
                  pc_ptr->show(buf);
                  proc_action_mobs.set((critter*)NULL, cnt);
               }
            }
         }//else
      }//for

      /* Room action procs */
      room* rm_ptr;
      for (int cnt = 0; cnt < proc_action_rooms.getCurLen(); cnt++) {
         if (!(rm_ptr = proc_action_rooms.elementAt(cnt))) {
            continue;
         }//if
         else {
            if (rm_ptr->getPause() > 0) {
               if (mudlog.ofLevel(SCRIPT)) {
                  mudlog << "room_script execute: pause is > 0:  " << rm_ptr->getPause()
                         << "  for room: " << rm_ptr->getIdNum() << endl;
               }
               continue;
            }
            else {
               if (mudlog.ofLevel(SCRIPT)) {
                  mudlog << "room_script execute: attempting to get script cmd for room: "
                         << rm_ptr->getIdNum() << endl;
               }
            }//else

            ScriptCmd* tmp_cmd_ptr;
            // Cast away const'ness, but still use it as if it's const.
            if ((tmp_cmd_ptr = (ScriptCmd*)(rm_ptr->getNextScriptCmd()))) {

               if (mudlog.ofLevel(SCRIPT)) {
                  mudlog << "grrmud.cc:  got a room script command -:"
                         << tmp_cmd_ptr->getCommand() << ":-" << endl;
               }
               cmd_ptr = new ScriptCmd(*tmp_cmd_ptr);

               // rm_ptr is the script owner.
               int script_ret_val;
               if ((script_ret_val = RoomScript::parseScriptCommand(*cmd_ptr, *rm_ptr, obj_was_deleted)) < 0) {
                  if (mudlog.ofLevel(WRN)) {
                     mudlog << "RoomScript command: target -:" 
                            << cmd_ptr->getTarget() << ":-  cmd -:"
                            << cmd_ptr->getCommand() << " for room# "
                            << rm_ptr->getIdNum() << " returned negative value: "
                            << script_ret_val << endl;
                  }
               }
               delete cmd_ptr;
            }
            else {
               rm_ptr->finishedRoomProc();

               // it may have started another one
               if (!rm_ptr->isInProcNow()) {
                  proc_action_rooms.set((room*)NULL, cnt);
               }
            }
         }//else
      }//for


      /* Object action procs */
      object* o_ptr;
      for (int cnt = 0; cnt < proc_action_objs.getCurLen(); cnt++) {
         //mudlog << "Searching for proc objs, cnt: " << cnt << endl;
         if (!(o_ptr = proc_action_objs.elementAt(cnt))) {
            //mudlog << "Was null.\n";
            continue;
         }//if
         else {
            //mudlog << "Found one: " << o_ptr->getIdNum() << endl;
            if (o_ptr->getPause() > 0) {
               continue;
            }

            ScriptCmd* tmp_cmd_ptr;
            // Cast away const'ness, but still use it as if it's const.
            if ((tmp_cmd_ptr = (ScriptCmd*)(o_ptr->getNextScriptCmd()))) {

               if (mudlog.ofLevel(DBG)) {
                  mudlog << "grrmud.cc:  got an object script command -:"
                         << tmp_cmd_ptr->getCommand() << ":-" << endl;
               }
               cmd_ptr = new ScriptCmd(*tmp_cmd_ptr);

               // rm_ptr is the script owner.
               int script_ret_val;
               if ((script_ret_val = ObjectScript::parseScriptCommand(*cmd_ptr, *o_ptr,
                                                                      obj_was_deleted)) < 0) {
                  if (!obj_was_deleted) {
                     if (mudlog.ofLevel(WRN)) {
                        mudlog << "WARNING:  ObjectScript command: target -:" 
                               << cmd_ptr->getTarget() << ":-  cmd -:"
                               << cmd_ptr->getCommand() << " for object# "
                               << o_ptr->getIdNum() << " returned negative value: "
                               << script_ret_val << endl;
                     }
                  }
               }
               delete cmd_ptr;
            }
            else {
               mudlog << "Could not get another command, must be done."
                      << endl;
               o_ptr->finishedObjProc();

               // it may have started another one
               if (!o_ptr->isInProcNow()) {
                  proc_action_objs.set((object*)NULL, cnt);
               }
            }
         }//else
      }//for

      //log("At end of while loop.\n");
   }//while
}// game_loop


struct timeval timediff(struct timeval *a, struct timeval *b)  {
   struct timeval rslt, tmp;

   //log("In timediff.\n");

   tmp = *a;

   if ((rslt.tv_usec = tmp.tv_usec - b->tv_usec) < 0) {
      rslt.tv_usec += 1000000;
      --(tmp.tv_sec);
   }//if
   if ((rslt.tv_sec = tmp.tv_sec - b->tv_sec) < 0) {
      rslt.tv_usec = 0;
      rslt.tv_sec = 0;
   }//if
   return(rslt);
}//timediff()



/* ******************************************************************
*  socket handling                                                  *
****************************************************************** */

int     init_socket(int port)   {
   int  s;
   int so_reuseaddr_opt;
   struct sockaddr_in sa;
   struct hostent *hp;

   mudlog.log(TRC, "in init_socket\n");
   memset((char*)(&sa), 0, sizeof(struct sockaddr_in ));

   if ( config.hostname == "all" ) {
      sa.sin_addr.s_addr = htonl(INADDR_ANY);
      sa.sin_family = AF_INET;
   } else {
      hp = gethostbyname(config.hostname);
      if (hp == NULL) {
         mudlog << "ERROR:  gethostbyname:  " << strerror(errno) << endl;
         do_shutdown = TRUE;
         exit(100);
      } else {
         memcpy((void *)&sa.sin_addr, (void *)hp->h_addr_list[0],
               sizeof(sa.sin_addr));
         sa.sin_family = hp->h_addrtype;
      }
   }

   sa.sin_port   = htons(port);

   if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      mudlog << "ERROR:  socket:  " << strerror(errno) << endl;
      do_shutdown = TRUE;
      exit(100);
   }//if

   so_reuseaddr_opt = 1;
   if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (void *)&so_reuseaddr_opt,
            sizeof(so_reuseaddr_opt))
          < 0) {
      mudlog << "ERROR:  setsockopt:  " << strerror(errno) << endl;
      do_shutdown = TRUE;
      exit(100);
   }//if

   int sanity = 0;
   while (TRUE) {
      if (sanity++ > 25) {
         mudlog << "ERROR:  bind sanity check kicked in.." << endl;
         close(s);
         do_shutdown = TRUE;
         exit(0); //Give up..something is wrong
      }//if
         
      if (bind(s, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
         mudlog << "ERROR:  trying to bind:  " << strerror(errno) << endl;
         sleep(10);
      }//if
      else {
         break;
      }
   }//while

   listen(s, 5);
   return(s);
}//init_socket, for the mother descriptor



int new_connection(int s)  {
   struct sockaddr_in isa;
   unsigned int  i;  //GLIBC
   // int i;
   int t;
 
   mudlog.log(TRC, "in new_connection\n");

   i = sizeof(isa);
   if (getsockname(s, (struct sockaddr *) & isa, &i) < 0) {
      mudlog << "ERROR:  new_connection, getsockname:  " 
             << strerror(errno) << endl;
      return (-1);
   }//if

   if ((t = accept(s, (struct sockaddr *)(&isa), &i)) < 0) {
      mudlog << "ERROR:  accept:  " << strerror(errno) << endl;
      return(-1);
   }//if
   nonblock(t);
   return(t);
}//new_connection()


// Static method, create a critter using the initialized socket s
int critter::createWithDescriptor(int s) {
   int  desc;
   critter *newd = new critter;
   unsigned int size;  //EGCS
   //int size;
   int i;
   struct sockaddr_in sock;
   struct hostent *from;
   String buf2(100);

   newd->pc = new pc_data;
   newd->CRITTER_TYPE = 0; //change default of MOB to PC

   newd->pc->p_handler = new TelnetHandler(newd);

   mudlog.log(TRC, "In createWithDescriptor\n");

   if ((desc = new_connection(s)) < 0) {
      delete newd->pc->p_handler;
      delete newd;
      return -1;
   }//if

   newd->setDescriptor(desc);
   
   if (sockets_connected > avail_descs) {
      char* str = "Sorry, ScryMUD is full, try again later!\n\r";
      write(desc, str, strlen(str)); 
      mudlog.log(WRN, "WARNING:  the mud was full.\n");
      close(desc);
      delete newd->pc->p_handler;
      delete newd;
      return -1;
   }//if

   mudlog.log(TRC, "Finding info.\n");

   /* find info */
   size = sizeof(sock);
   if (getpeername(desc, (struct sockaddr *) & sock, &size) < 0) {
      mudlog << "ERROR:  getpeername:  " << strerror(errno) << endl;
   }// if 
   else {
      // BEN:  Turning this off, might be crashing us...can just look up IP's
      // with nslookup
      if (TRUE || !(from = gethostbyaddr((char *)&sock.sin_addr,
               sizeof(sock.sin_addr), AF_INET))) { //begin of if
         //mudlog << "WARNING:  gethostbyaddr:  " << strerror(errno) << endl;
         i = sock.sin_addr.s_addr;
         Sprintf(newd->pc->host, "%i.%i.%i.%i", (i & 0x000000FF),
              ((i & 0x0000FF00) >> 8), ((i & 0x00FF0000) >> 16), 
              ((i & 0xFF000000) >> 24));
      } //if
      else {
         newd->pc->host = from->h_name;
      }//else
   }//else
   
   if (mudlog.ofLevel(DBG))
      mudlog << "Got host:  " << newd->pc->host << endl;

   if (is_banned(newd->pc->host)) {
      char* str =
"Your site has been banned.  There are three possible causes for this:\n\n\
You did something that is very strictly against the rules.\n\n\
Someone from your site did something that is very strictly against the rules,\n\
and you are now connecting from the same machine or IP address as they.\n\n\
Someone from your site did something that is very strictly against the rules,\n\
and tried to persist in that action.  In this case, the whole site has been\n\
banned instead of just one user.\n\n\
If you feel that you or your site has been wrongly sitebanned and would like\n\
the ban removed, please contact greear@cyberhighway.net to discuss the matter.";

      write(desc, str, strlen(str)); 
      close(desc);
      Sprintf(buf2, "WARNING:  Connection attempt denied from [%S]", 
              &(newd->pc->host));
      mudlog.log(WRN, buf2);
      delete newd->pc->p_handler;
      delete newd;
      return -1;
   }//if

   newd->PC_FLAGS.turn_on(16); //insert the \r that DOS loves so much *puke*
   pc_list.prepend(newd);
   sockets_connected++;

         //will read it in, or create it from input
   newd->startLogin(); 
   
   mudlog.log(TRC, "At end of new_descriptor.\n");         
   return TRUE;
}//createWithSocket



int critter::writeOutput() {
   int ret = 0;

   if (isInPageBreak()) { //in pause mode (ie page_break)
      return 0;
   }

   int len = pc->output.Strlen();
   // log("in write_output\n");
   if (len) { //no need to write zip
      if (PC_FLAGS.get(16)) { //if do carriage return
         pc->output.parse_show(PC_FLAGS.get(15)); //CR behind or not
         len = pc->output.Strlen();
      }//if
      if (PC_FLAGS.get(22)) { //if do page breaks
         int len = pc->output.Strlen();
         int nl_cnt = 0;
         int i = 0;
         const char* buf = (const char*)(pc->output);
         for (i = 0; i<len; i++) {
            if (buf[i] == '\n') {
               nl_cnt++;
               if (nl_cnt == max(getLinesOnPage(), 8)) {
                  break;
               }
            }//if
         }//for

         const char* pr = "******[ HIT RETURN TO CONTINUE ] ******";
         if (nl_cnt == max(getLinesOnPage(), 8)) {
            PC_FLAGS.turn_on(23);
            pc->output.insertAfter(i, pr);
            if (mudlog.ofLevel(XMT)) {
               mudlog << "About to write:  -:" << pc->output << ":-" << endl;
            }
            ret = pc->output.Write(pc->descriptor, i + strlen(pr));
            if (mudlog.ofLevel(XMT)) {
               mudlog << "After write:  -:" << pc->output << ":-" << endl;
            }
         }//if
         else {
            ret = pc->output.Write(pc->descriptor, len);
         }
      }//if
      else {
         ret = pc->output.Write(pc->descriptor, len);
      }
   }//if
   return ret;
}//write_output



#if defined(SVR4) || defined(LINUX)

void nonblock(int s) {
   int  flags;

   mudlog.log(TRC, "in nonblock\n");

   flags = fcntl(s, F_GETFL);
   flags |= O_NONBLOCK;
   if (fcntl(s, F_SETFL, flags) < 0) {
      mudlog << "ERROR:  fcntl, executing nonblock:  " << strerror(errno) 
             << endl;
      do_shutdown = TRUE;
      exit(100);
   }
}//nonblock


#else

void nonblock(int s) {
   if (fcntl(s, F_SETFL, O_NDELAY) == -1) {
      mudlog << "ERROR:  fcntl (!LINUX), executing nonblock:  " 
             << strerror(errno) << endl;
      do_shutdown = TRUE;
      exit(100);
   }//if
}//nonblock

#endif

// This is temporarily here for now as we need to be able to handle binary
// data for telnet option negotiations. String::Read() assumes that we only
// ever deal in \n terminated printable strings.
ssize_t netread(int fd, char* input_buf, size_t max_read) {
   ssize_t read_bytes;
   read_bytes = recv(fd, input_buf, max_read, 0);
   if ( read_bytes < 0 ) {
      if ( errno != EAGAIN ) {
         return(-1);
      } else {
         return(0);
      }
   } else if ( read_bytes == 0 ) {// the other end performed an orderly shutdown
      return(-2);
   }
      
   return(read_bytes);
}

int critter::readInput()  {

   if (!pc) {
      mudlog << "(in critter::readInput) Critter: " << getName()
         << " does not have pc_data." << endl;
      return -1;
   }

   errno = 0;

   char input_buf[4096];
   ssize_t read_bytes = netread(pc->descriptor, input_buf, 4096);

   if ( read_bytes < 0 ) {
      mudlog << "Critter: " << getName() << "  Read Error: " << strerror(errno) << endl;
      pc->SocketProblem();
      return(0);
   }

   if (mudlog.ofLevel(TRC)) {
      mudlog << "End of get_input, here it is:\n" << pc->input << ":-  ret: " << read_bytes << endl;
   }

   pc->p_handler->parse(input_buf, read_bytes);

   return read_bytes;  //valid input added to input buffer, can go parse it now.
}//get_input


void close_sockets(int mother) {
   Cell<critter*> cell;
   pc_list.head(cell);
   critter* crit_ptr;

   mudlog.log(DIS, "Closing all sockets now.\n");

   while ((crit_ptr = cell.next())) {
      if (crit_ptr->doLogOffActive() < 0)
         mudlog << "ERROR, close socket failed.\n";
   }

   close(mother);
}//close_sockets


// delete self after this!
int critter::doLogOffActive() {
   String buf(100);

   if (!isPc()) {
      mudlog.log(ERROR, "ERROR, doLoseLink on npc.\n");
      return -1;
   }

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In doLoseLink, critter name:  " <<  *(getName())
         << "  room:  " << getCurRoomNum() << "  addr:  " << this
         << endl << flush;
      mudlog << " link condition:  " << pc->link_condition
         << endl;
   }

   if (pc->link_condition != CON_PLAYING) {
      mudlog.log(ERROR, "ERROR, doLoseLink, link condition !CON_PLAYING\n");
      return -1;
   }

   doGoLinkdead(); //first step

   doLogOffInactive();

   linkdead_list.loseData(this); //take em off the link dead list.

   return 1;
}//doLogOffActive



int critter::doLogOffInactive() {
   String buf(100);
   //remove from linkdead list in calling code!

   if (mudlog.ofLevel(DBG)) {
      mudlog << "doLogOffInactive, critter:  " << *(getName())
         << "  size of PETS:  " << PETS.size() << endl;
   }

   doUngroup(1, &NULL_STRING); //take em out of their group, if in it.

   Cell<critter*> pet_cll(PETS);
   critter* pet_ptr;
   while ((pet_ptr = pet_cll.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "PETS:  Has a pet:  " <<  *(pet_ptr->getName())
            << "  ptr_address: " << pet_ptr << endl;
      }
   }//while

   List<critter*> tmp_list(NULL);
   tmp_list = PETS; //shallow copy

   tmp_list.head(pet_cll);
   while ((pet_ptr = pet_cll.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "Tmp_List:  Has a pet:  " <<  *(pet_ptr->getName())
            << "  ptr_address: " << pet_ptr << endl;
      }
   }//while

   if (mudlog.ofLevel(DBG)) {
      mudlog << "  size of tmp_list:  " << tmp_list.size() << endl;
   }

   tmp_list.head(pet_cll);
   while ((pet_ptr = pet_cll.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "Found a pet:  " <<  *(pet_ptr->getName())
            << endl << flush;
      }
      pet_ptr->show("Your master has left the game.\n");
      pet_ptr->doBecomeNonPet();
   }//while
   tmp_list.clear();

   if (PETS.size() > 0) {
      mudlog << "ERROR:  PETS.size() > 0 in doLogOffInactive:  "
         << PETS.size() << endl << flush;
      PETS.clear();
   }

   // Take care of their followers, if they still have any      
   tmp_list = FOLLOWERS;
   tmp_list.head(pet_cll);
   while ((pet_ptr = pet_cll.next())) {
      pet_ptr->doFollow(*pet_ptr, TRUE);
   }
   FOLLOWERS.clear();
   tmp_list.clear();

   if (pc->w_eye_obj) {
      pc->w_eye_obj->obj_proc->w_eye_owner = NULL;
      pc->w_eye_obj = NULL;
   }//if

   if (possessed_by) {
      possessed_by->unPossess();
      possessed_by = NULL;
   }

   if (SNOOPED_BY) {
      SNOOPED_BY->show("(SNOOPEE) has left the game.\n");
      SNOOPED_BY->SNOOPING = NULL;
   }

   doShowList(this, Selectors::instance().CC_gets_info_allow,
         Selectors::instance().CC_none, pc_list,
         CS_PLAYER_OFF_LD_LIST_INFO,
         getName());

   if (MASTER) {
      doBecomeNonPet();
   }

   emote("has left the game.");

   leave_room_effects(*(getCurRoom()), *this, FALSE);
   breakEarthMeld();

   doLeaveRoom();

   recursive_init_unload(*this); //take eq out of circulation

   return 1;
}//doLogOffInActive


//very little changes, but lots of messages are sent.
// just close the descriptor if it's open, and set the vis
// bit accordingly.  Add to link_dead list.
int critter::doGoLinkdead() {
   String buf(100);

   if (!isPc()) {
      mudlog.log(ERROR, "ERROR, doGoLinkDead on npc.\n");
      return -1;
   }

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In doGoLinkDead, critter name:  " <<  *(getName())
         << "  room:  " << getCurRoomNum() << "  addr:  " << this
         << endl << flush;
      mudlog << " link condition:  " << pc->link_condition
         << endl;
   }

   if (SNOOPED_BY) {
      Sprintf(buf, "%S (SNOOPEE) has lost link.\n", getName());
      buf.Cap();
      SNOOPED_BY->show(buf);
   }//if

   if (possessed_by) {
      possessed_by->unPossess();
      possessed_by = NULL;
   }

   doShowList(this, Selectors::instance().CC_gets_info_allow,
         Selectors::instance().CC_none, pc_list,
         CS_PLAYER_LOST_CON_INFO,
         getName());

   if (MASTER) {
      Sprintf(buf, "%S has lost link.\n", getName());
      buf.Cap();
      MASTER->show(buf);
   }//if

   emote("has lost link.");

   if (pc->descriptor != -1) {
      writeOutput();
      if (close(pc->descriptor) < 0) {
         mudlog << "ERROR: close_socket, close:  " << strerror(errno) 
            << endl;
      }//if
      else {
         sockets_connected--;
      }

      pc->descriptor = -1;
   }//if

   VIS_BIT |= 32;     
   linkdead_list.gainData(this);  //so one might re-login easily

   return 1;
}//doGoLinkDead


int critter::doLogOffNewLogin() {
   mudlog.log(TRC, "In CON_LOGGING_In\n");
   String buf(100);

   if (!isPc()) {
      mudlog.log(ERROR, "ERROR, doLogOffNewLogin on npc.\n");
      return -1;
   }

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In doLogOffNewLogin, critter name:  " <<  *(getName())
         << "  room:  " << getCurRoomNum() << "  addr:  " << this
         << endl << flush;
      mudlog << " link condition:  " << pc->link_condition
         << endl;
   }

   if (pc->descriptor != -1) {
      writeOutput(); //try to flush the buffer if possible.
      if (close(pc->descriptor) < 0) {
         mudlog << "ERROR: log_off: close() " << strerror(errno) << endl;
      }//if
      else
         sockets_connected--;
   }//if
   return 1;
}//doLogOffNewLogin
