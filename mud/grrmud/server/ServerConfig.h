#ifndef __ServerConfig_h__
#define __ServerConfig_h__
#include <fstream.h>
#include <string.h>
#include <string2.h>
#include <stdlib.h>


class ServerConfig {
   public:
      ServerConfig();
      void read(char* filename);
      void readDynamic(char* filename);

      void write(char* filename);
      void writeDynamic(char* filename);

      // Network
      int port;

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
};
#endif
