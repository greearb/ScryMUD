#ifndef __ServerConfig_h__
#define __ServerConfig_h__
#include <fstream>
#include <string.h>
#include <string2.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

class ServerConfig {
   public:
      ServerConfig();
      void read(const char* filename);
      void readDynamic(const char* filename);

      void write(const char* filename);
      void writeDynamic(const char* filename);

      // chroot+daemonize
      int daemonize;
      int suid;
      uid_t suid_user;
      gid_t suid_group;

      // Network
      int port;
      String hostname;

      // Percent Load Modifiers
      int bootLoadModifier;
      int regularLoadModifier;
      int currentLoadModifier;

      // Player-related
      int experienceDivisor;
      int maxPlayers;
      int hungerMax;
      int thirstMax;
      int druggedMax;
      int sacPointsNeeded;

      // Price Modifiers
      double outOfTownModifier;
      double commerceSkillBuyEffect;
      double commerceSkillSellEffect;

      // Important room numbers
      int recallRoom;
      int tammuzRoom;
      int newbieRoom;
      int loginRoom;
      int donateRoom;
      int necrophiliaRoom;

      // Important object numbers
      int newbieBagObject;
      int corpseObject;
      int goldCoinsObject;
      int pcSkinObject;
      int HeadObject;
      int meatObject;
      int createFoodObject1;
      int createFoodObject2;
      int createFoodObject3;
      int createFoodObject4;
      int createLightObject;

      // Important mob numbers
      int earthGolemMob;
      int waterGolemMob;
      int airGolemMob;
      int fireGolemMob;
     int graniteGolemMob;
      int iceGolemMob;
      int lightningGolemMob;
      int hellfireGolemMob;
      int minionMob1;
      int minionMob2;
      int minionMob3;
      int minionMob4;
      int walkingCorpseMob;
      int illusionMob;

      // Important doors
      int gateDoor;

      // Time
      int year;
      int day;
      int hour;
      long optUsec;

      // Weight
      int goldPerLb;

      // General gameplay modifiers
      int newTeleport;
      int newSummon;
      int maxRedundantDonates;
      int convertWorldFromDev;
      int useViolenceTimer;

      // Database
      int useMySQL;
      String mySQLuser;
      String mySQLpassword;
      String mySQLdatabase;
      String mySQLhost;
      int mySQLport;

      // Misc
      bool resave_pfiles;
};
#endif
