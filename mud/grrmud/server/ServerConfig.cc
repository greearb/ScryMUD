#include "ServerConfig.h"

ServerConfig::ServerConfig() {
   port=4000;
   
   bootLoadModifier=300;
   regularLoadModifier=100;
   currentLoadModifier=100;
   
   experienceDivisor=1;
   maxPlayers=500;
   hungerMax=100;
   thirstMax=75;
   druggedMax=25;
   sacPointsNeeded=150;

   outOfTownModifier=1.25;
   commerceSkillBuyEffect=0.86;
   commerceSkillSellEffect=1.05;
   
   recallRoom=1;
   tammuzRoom=2923;
   newbieRoom=6;
   loginRoom=1;
   donateRoom=7;
   necrophiliaRoom=6888;

   newbieBagObject=358;
   corpseObject=3;
   goldCoinsObject=5;
   pcSkinObject=8;
   HeadObject=1030;
   meatObject=97;
   createFoodObject1=94;
   createFoodObject2=68;
   createFoodObject3=13;
   createFoodObject4=103;
   createLightObject=345;

   earthGolemMob=37;
   waterGolemMob=190;
   airGolemMob=191;
   fireGolemMob=193;
   graniteGolemMob=321;
   iceGolemMob=322;
   lightningGolemMob=323;
   hellfireGolemMob=324;
   minionMob1=38;
   minionMob2=39;
   minionMob3=41;
   minionMob4=42;
   walkingCorpseMob=57;
   illusionMob=55;

   gateDoor=44;

   year=1;
   day=1;
   hour=1;
   optUsec=200000;
   
   goldPerLb=1000;
   
   newTeleport=true;
   newSummon=true;
   maxRedundantDonates=5;
   convertWorldFromDev=false;
   useViolenceTimer=false;

   useMySQL=false;
   mySQLuser="gmud";
   mySQLpassword="password";
   mySQLdatabase="gmud";
   mySQLhost="localhost";
   mySQLport=0;
}//ServerConfig

void ServerConfig::read(char* filename) {
   String key(20);
   String val(10);
   ifstream file(filename);

   if (!file) {
      cerr << "Couldn't open " << filename << " for reading.\n";
      return;
   }

   while (!file.eof()) {
      file >> key;
      if (key[0] == '#') {
         while (file.get() != '\n' && !file.eof());
      }
      else if (strlen(key) != 0) {
         file >> val;
         if (strcasecmp(key, "port") == 0) {
            port = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "bootLoadModifier") == 0) {
            bootLoadModifier = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "regularLoadModifier") == 0) {
            regularLoadModifier = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "experienceDivisor") == 0) {
            experienceDivisor = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "maxPlayers") == 0) {
            maxPlayers = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "hungerMax") == 0) {
            hungerMax = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "thirstMax") == 0) {
            thirstMax = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "druggedMax") == 0) {
            druggedMax = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "sacPointsNeeded") == 0) {
            sacPointsNeeded = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "outOfTownModifier") == 0) {
            outOfTownModifier = (double)strtod(val, NULL);
         }
         else if (strcasecmp(key, "commerceSkillBuyEffect") == 0) {
            commerceSkillBuyEffect = (double)strtod(val, NULL);
         }
         else if (strcasecmp(key, "commerceSkillSellEffect") == 0) {
            commerceSkillSellEffect = (double)strtod(val, NULL);
         }
         else if (strcasecmp(key, "recallRoom") == 0) {
            recallRoom = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "tammuzRoom") == 0) {
            tammuzRoom = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "newbieRoom") == 0) {
            newbieRoom = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "loginRoom") == 0) {
            loginRoom = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "donateRoom") == 0) {
            donateRoom = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "necrophiliaRoom") == 0) {
            necrophiliaRoom = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "newbieBagObject") == 0) {
            newbieBagObject = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "corpseObject") == 0) {
            corpseObject = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "goldCoinsObject") == 0) {
            goldCoinsObject = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "pcSkinObject") == 0) {
            pcSkinObject = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "HeadObject") == 0) {
            HeadObject = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "meatObject") == 0) {
            meatObject = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "createFoodObject1") == 0) {
            createFoodObject1 = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "createFoodObject2") == 0) {
            createFoodObject2 = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "createFoodObject3") == 0) {
            createFoodObject3 = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "createFoodObject4") == 0) {
            createFoodObject4 = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "createLightObject") == 0) {
            createLightObject = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "earthGolemMob") == 0) {
            earthGolemMob = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "waterGolemMob") == 0) {
            waterGolemMob = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "airGolemMob") == 0) {
            airGolemMob= (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "fireGolemMob") == 0) {
            fireGolemMob = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "graniteGolemMob") == 0) {
            graniteGolemMob = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "iceGolemMob") == 0) {
            iceGolemMob = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "lightningGolemMob") == 0) {
            lightningGolemMob= (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "hellfireGolemMob") == 0) {
            hellfireGolemMob = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "minionMob1") == 0) {
           minionMob1  = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "minionMob2") == 0) {
            minionMob2 = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "minionMob3") == 0) {
            minionMob3 = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "minionMob4") == 0) {
            minionMob4 = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "walkingCorpseMob") == 0) {
            walkingCorpseMob = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "illusionMob") == 0) {
            illusionMob = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "gateDoor") == 0) {
            gateDoor = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "year") == 0) {
            year = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "day") == 0) {
            day = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "hour") == 0) {
            hour = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "optUsec") == 0) {
            optUsec = strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "goldPerLb") == 0) {
            goldPerLb = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "newTeleport") == 0) {
            if (strcasecmp(val, "true") == 0) newTeleport=true;
            else if (strcasecmp(val, "false") == 0) newTeleport=false;
         }
         else if (strcasecmp(key, "newSummon") == 0) {
            if (strcasecmp(val, "true") == 0) newSummon=true;
            else if (strcasecmp(val, "false") == 0) newSummon=false;
         }
         else if (strcasecmp(key, "maxRedundantDonates") == 0) {
            maxRedundantDonates = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "useMySQL") == 0) {
            if (strcasecmp(val, "true") == 0) useMySQL=true;
            else if (strcasecmp(val, "false") == 0) useMySQL=false;
         }
         else if (strcasecmp(key, "mySQLuser") == 0) {
            mySQLuser=val;
         }
         else if (strcasecmp(key, "mySQLpassword") == 0) {
            mySQLpassword=val;
         }
         else if (strcasecmp(key, "mySQLdatabase") == 0) {
            mySQLdatabase=val;
         }
         else if (strcasecmp(key, "mySQLhost") == 0) {
            mySQLhost=val;
         }
         else if (strcasecmp(key, "mySQLport") == 0) {
            mySQLport = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "useViolenceTimer") == 0) {
            cout << "Found useViolenceTimer.\n";
            if (strcasecmp(val, "true") == 0) {
               useViolenceTimer=true;
               cout << "Using useViolenceTimer.\n";
            } else if (strcasecmp(val, "false") == 0) {
               useViolenceTimer=false;
               cout << "NOT Using useViolenceTimer.\n";
            }
         }
      }
   }
}

void ServerConfig::readDynamic(char* filename) {
   String key(20);
   String val(10);
   ifstream file(filename);

   if (!file) {
      return;
   }

   while (!file.eof()) {
      file >> key;
      if (key[0] == '#') {
         while (file.get() != '\n' && !file.eof());
      }
      else if (strlen(key) != 0) {
         file >> val;
         if (strcasecmp(key, "year") == 0) {
            year = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "day") == 0) {
            day = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "hour") == 0) {
            hour = (int)strtol(val, NULL, 0);
         }
         else if (strcasecmp(key, "convertWorldFromDev") == 0) {
            if (strcasecmp(val, "true") == 0) convertWorldFromDev=true;
            else if (strcasecmp(val, "false") == 0) convertWorldFromDev=false;
         }
      }
   }
}

void ServerConfig::write(char* filename) {
   ofstream file(filename);

   if (!file) {
      cerr << "Couldn't open " << filename << " for writing.\n";
      return;
   }

   file << "port " << port << '\n';
   
   file << "bootLoadModifier " << bootLoadModifier << '\n';
   file << "regularLoadModifier " << regularLoadModifier << '\n';
   
   file << "experienceDivisor " << experienceDivisor << '\n';
   file << "maxPlayers " << maxPlayers << '\n';
   file << "hungerMax " << hungerMax << '\n';
   file << "thirstMax " << thirstMax << '\n';
   file << "druggedMax " << druggedMax << '\n';
   file << "sacPointsNeeded " << sacPointsNeeded << '\n';

   file << "outOfTownModifier " << outOfTownModifier << '\n';
   file << "commerceSkillBuyEffect " << commerceSkillBuyEffect << '\n';
   file << "commerceSkillSellEffect " << commerceSkillSellEffect << '\n';
   
   file << "recallRoom " << recallRoom << '\n';
   file << "tammuzRoom " << tammuzRoom << '\n';
   file << "newbieRoom " << newbieRoom << '\n';
   file << "loginRoom " << loginRoom << '\n';
   file << "donateRoom " << donateRoom << '\n';

   file << "newbieBagObject " << newbieBagObject << '\n';
   file << "corpseObject " << corpseObject << '\n';
   file << "goldCoinsObject " << goldCoinsObject << '\n';
   file << "pcSkinObject " << pcSkinObject << '\n';
   file << "HeadObject" << HeadObject << '\n';
   file << "meatObject " << meatObject << '\n';
   file << "createFoodObject1 " << createFoodObject1 << '\n';
   file << "createFoodObject2 " << createFoodObject2 << '\n';
   file << "createFoodObject3 " << createFoodObject3 << '\n';
   file << "createFoodObject4 " << createFoodObject4 << '\n';
   file << "createLightObject " << createLightObject << '\n';

   file << "earthGolemMob " << earthGolemMob << '\n';
   file << "waterGolemMob " << waterGolemMob << '\n';
   file << "airGolemMob " << airGolemMob << '\n';
   file << "fireGolemMob " << fireGolemMob << '\n';

   file << "graniteGolemMob " << graniteGolemMob << '\n';
   file << "iceGolemMob " << iceGolemMob << '\n';
   file << "lightningGolemMob " << lightningGolemMob << '\n';
   file << "hellfireGolemMob " << hellfireGolemMob << '\n';

   file << "minionMob1 " << minionMob1 << '\n';
   file << "minionMob2 " << minionMob2 << '\n';
   file << "minionMob3 " << minionMob3 << '\n';
   file << "minionMob4 " << minionMob4 << '\n';
   file << "walkingCorpseMob " << walkingCorpseMob << '\n';
   file << "illusionMob " << illusionMob << '\n';

   file << "gateDoor " << gateDoor << '\n';

   file << "Year " << year << '\n';
   file << "Day " << day << '\n';
   file << "Hour " << hour << '\n';
   file << "optUsec" << optUsec << '\n';
   
   file << "goldPerLb " << goldPerLb << '\n';
   
   if (newTeleport) {
      file << "newTeleport true\n";
   }
   else {
      file << "newSummon false\n";
   }
   if (newSummon) {
      file << "newSummon true\n";
   }
   else {
      file << "newSummon false\n";
   }
   if (useViolenceTimer) {
      file << "useViolenceTimer true\n";
   }
   else {
      file << "useViolenceTimer false\n";
   }

   file << "maxRedundantDonates " << maxRedundantDonates << '\n';

   if (useMySQL) {
      file << "useMySQL true\n";
      file << "mySQLuser " << mySQLuser << '\n';
      file << "mySQLpassword " << mySQLpassword << '\n';
      file << "mySQLdatabase " << mySQLdatabase << '\n';
      file << "mySQLhost " << mySQLhost << '\n';
      file << "mySQLport " << mySQLport << '\n';
   }
   else {
      file << "useMySQL false\n";
   }
}

void ServerConfig::writeDynamic(char* filename) {
   ofstream file(filename);

   if (!file) {
      cerr << "Couldn't open " << filename << " for writing.\n";
      return;
   }

   file << "# This file holds the fields the server changes by itself.\n";
   file << "# They will override anything in grrmud.cfg\n";
   file << "Year " << year << '\n';
   file << "Day " << day << '\n';
   file << "Hour " << hour << '\n';
   file << "convertWorldFromDev " << convertWorldFromDev << '\n';
}
