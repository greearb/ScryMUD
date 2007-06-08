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

///********************  weather.cc  *****************************///

#include "classes.h" 
#include "misc.h"
#include "misc2.h"
#include "const.h"
#include <PtrArray.h>
#include "BuildInfo.h"
#include "weather.h"
#include <string2.h>
#include <assert.h>
#include <stdio.h>
#include <vector>
//TODO fix critter.isSleeping()
//TODO make wNONE, cNONE, tNONE valid array indexes
//TODO make int_to_enum inline funciton

//climate weather base
const int temperate_weights[] = { 520,130,130,65,55,48,32,20,10,65,48,32,0};
const int savanah_weights[] = { 520,100,90,32,28,24,16,10,5,5,0,0,0};
const int mountain_weights[] =  { 520,130,130,65,51,44,40,20,10,80,60,40,0};
const int snowymountain_weights[] = { 520,130,130,32,25,22,20,20,10,130,96,64,0};
const int sandydesert_weights[] = { 540,40,30,3,2,1,1,0,0,0,0,0,80};
const int dirtdesert_weights[] = { 600,40,30,3,2,1,1,0,0,0,0,0,0};
const int swamp_weights[] = { 400,150,150,85,75,68,32,20,10,16,8,4,0};
const int tropical_weights[] = { 520,130,130,65,55,48,32,10,5,0,0,0,0};
const int arctic_weights[] = { 540,130,110,3,2,1,1,0,0,80,60,40,0};

//current weather weights
const float clear_weights[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1};
const float cloudy_weights[] = { 1,3,2,1.5,1.25,1.125,1,1,1,1.5,1.25,1.125,1};
const float overcast_weights[] = { 1,2,3,2,1.5,1.25,1.125,1,1,2,1.5,1.25,1};
const float drizzle_weights[] = { 1,1.5,2,3,1.5,1.25,1.125,1.1,1.1,1,1,1,0};
const float rain_weights[] = { 0.8,1.25,1.5,2,3,2,1.5,1.25,1.1,0.8,0.8,0.7,0};
const float heavyrain_weights[] = { 0.6,1,1.25,1.5,2,3,1.6,1,1,0.6,0.6,0.5,0};
const float thunderstorm_weights[] = { 0.4,0.8,1,1.25,1.5,2,3,1.2,1.2,0.5,0.5,0.5,0};
const float hail_weights[] = { 0.6,1,1.25,1.5,2,3,2,1.5,1.5,0.5,0.5,0.5,0};
const float hailstorm_weights[] = { 0.4,0.8,1.5,2,2.5,2.5,1.5,1.5,0.35,0.35,0.35,0.35,0};
const float lightsnow_weights[] = { 1,1.6,2,1.2,1.1,1,0.8,0,0,3,2,1.25,0};
const float heavysnow_weights[] = { 0.6,1.4,1.6,1,0.7,0.5,0.4,0,0,2,3,1.5,0};
const float blizzard_weights[] = { 0.6,1.4,1.6,0.6,0.5,0.3,0.2,0,0,1.5,2,2.5,0};
const float sandstorm_weights[] = { 0.5,1,1,1,1,1,1,0,0,0,0,0,2};
const float* weather_weights[]= { clear_weights, cloudy_weights,
   overcast_weights,drizzle_weights,rain_weights,heavyrain_weights,
   thunderstorm_weights,hail_weights,hailstorm_weights,lightsnow_weights,
   heavysnow_weights,blizzard_weights,sandstorm_weights};

//weather season weights
const float spring_weights[] = { 1,2,2,2,2,2,2,1.5,1.5,0.25,0.1,0,1};
const float summer_weights[] = { 1,1,1,1,1.2,1.25,2,1,1,0,0,0,1};
const float fall_weights[] = { 1,1,1,1,1,1,1,0.5,0.5,1,1,1,1};
const float winter_weights[] = { 1.3,1,1,0.75,0.75,0.75,.75,1,1,2,2,2,1};
const float* season_weights[] = { spring_weights,summer_weights, fall_weights,
      winter_weights};

//climate temperature weights
const int temperate_temp_weights[] = { 1,30,40,100,100,100,40,30,0};
const int savanah_temp_weights[] = { 0,10,20,40,80,100,80,40,0};
const int mountain_temp_weights[] =  {10,40,80,100,60,20,0,0,0};
const int snowymountain_temp_weights[] = { 30,60,100,60,20,10,0,0,0};
const int sandydesert_temp_weights[] = { 0,0,0,0,20,40,100,100,20};
const int dirtdesert_temp_weights[] = { 0,0,0,0,20,40,100,100,20};
const int swamp_temp_weights[] = { 0,20,30,100,100,100,40,30,0};
const int tropical_temp_weights[] = { 0,10,20,60,100,100,20,0,0};
const int arctic_temp_weights[] = { 65,85,100,40,10,0,0,0,0};

//weather temperature weights
const float clear_temp_weights[] = { 1,1,1,1,1,1,1,1,1};
const float cloudy_temp_weights[] = { 1.05,1.05,1.05,1,0.95,0.95,0.95,0.90,0.90};
const float overcast_temp_weights[] = { 1.1,1.1,1.1,1.05,1,0.95,0.9,0.8,0.7};
const float drizzle_temp_weights[] = { 1.2,1.2,1.15,1.1,1,0.95,0.85,0.75,0.7};
const float rain_temp_weights[] = { 1.25,1.25,1.2,1.15,.95,0.85,0.75,0.7,0.65};
const float heavyrain_temp_weights[] = { 1.25,1.25,1.25,1.25,1,0.85,0.75,0.7,0.65};
const float thunderstorm_temp_weights[] = { 1.3,1.3,1.2,1.1,.95,0.80,0.70,0.65,0.6};
const float hail_temp_weights[] = { 1.25,1.25,1.2,1.15,.95,0.85,0.75,0.7,0.65};
const float hailstorm_temp_weights[] = { 1.25,1.25,1.25,1.25,1,0.85,0.75,0.7,0.65};
const float lightsnow_temp_weights[] = { 1,1,1,1,0,0,0,0,0};
const float heavysnow_temp_weights[] = { 1.15,1.1,1.05,1,0,0,0,0,0};
const float blizzard_temp_weights[] = { 1.25,1.15,1,1,0,0,0,0,0};
const float sandstorm_temp_weights[] = { 1,1,1,1,1,1,1,1,1};
const float* weather_temp_weights[] = { clear_temp_weights, cloudy_temp_weights,
   overcast_temp_weights, drizzle_temp_weights, rain_temp_weights,
   heavyrain_temp_weights, thunderstorm_temp_weights, hail_temp_weights,
   hailstorm_temp_weights, lightsnow_temp_weights, heavysnow_temp_weights,
   blizzard_temp_weights, sandstorm_temp_weights };

//season temperature weights
const float spring_temp_weights[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1};
const float summer_temp_weights[] = { 1,1,1,1,1.2,1.25,2,1,1,0,0,0,1};
const float fall_temp_weights[] = { 1,1,1,1,1,1,1,0.5,0.5,1,1,1,1};
const float winter_temp_weights[] = { 1.3,1,1,0.75,0.75,0.75,.75,1,1,2,2,2,1};
const float* season_temp_weights[] = {spring_temp_weights, summer_temp_weights,
   fall_temp_weights,winter_temp_weights};

//wind range
const int wind_min[] = {0,0,0,0,5,16,0,16,0,5,12,16,30};
const int wind_max[] = {30,30,30,30,30,35,65,30,65,30,35,65,70};

//name strings
const char* climate_strings[] = {"temperate","savanah","mountain",
   "snowy mountain","sandy desert","dirt desert","swamp","tropical","arctic"};
const char* weather_strings[] = {"clear","cloudy","overcast","drizzling",
   "raining", "raining heavily","thunder storm","hailing","hail storm",
   "snowing gently", "snowing heavily","blizzard","sandstorm"};
const char* wind_strings[] = {"no wind", "a gentle breeze", "light winds",
   "moderate winds", "strong winds", "gale-force winds"};
const char* temperature_strings[] = {"freezing","very cold","cold","chilly",
   "pleasent","warm","hot","very hot","burning hot"};

//regen and mv usage mods
const float weather_regen_mods[] = {1,1,1,1,0.95,0.9,0.8,0.9,0.8,0.95,0.9,0.8};
const float temperature_regen_mods[] = {0.6,0.75,0.9,0.95,1,0.95,0.9,0.7,0.6};
const float weather_mv_mods[] = {1,1,1,1,1.1,1.25,1.5,1.25,1.5,1.1,1.25,1.25};
const float temperature_mv_mods[] = {1.4,1.25,1.25,1.1,1.05,1,1.05,1.1,1.3,1.4};

TemperatureType int_to_temperature_enum(unsigned int i){
	switch(i){
		case 0:
			return freezing;
		case 1:
			return verycold;
		case 2:
			return cold;
		case 3:
			return chilly;
		case 4:
			return pleasant;
		case 5:
			return warm;
		case 6:
			return hot;
		case 7:
			return veryhot;
		case 8:
			return burninghot;
		default:
			return tNONE;
	}
}

WindType int_to_wind_enum(unsigned int i){
	// note that this works differantly from all the rest
   if(i <= 5){
      return nowind;
   } else if (i <=15){
      return gentlebreeze;
   } else if (i <=25){
      return lightwind;
   } else if (i <= 35){
      return windy;
   } else if (i <= 60){
      return verywindy;
   } else {return gale; }
}

ClimateType int_to_climate_enum(unsigned int i){
   ClimateType c;
   
   switch(i){
      case 0:
         c = temperate;
         break;
      case 1:
         c = savanah;
         break;
      case 2:
         c =  mountain;
         break;
      case 3:
         c =  snowymountain;
         break;
      case 4:
         c =  sandydesert;
         break;
      case 5:
         c =  dirtdesert;
         break;
      case 6:
         c =  swamp;
         break;
      case 7: 
         c =  tropical;
         break;
      case 8:
         c =  arctic;
         break;
	  default:
		  c = cNONE;
   }
   return c;
}

WeatherType int_to_weather_enum(int i){

   WeatherType w;
   
   switch(i){
      case 0:
         w = clear;
         break;
      case 1:
         w = cloudy;
         break;
      case 2:
         w = overcast;
         break;
      case 3:
         w = drizzle;
         break;
      case 4:
         w = rain;
         break;
      case 5:
         w = heavyrain;
         break;
      case 6:
         w = thunderstorm;
         break;
      case 7:
         w = hail;
         break;
      case 8:
         w = hailstorm;
         break;
      case 9:
         w = lightsnow;
         break;
      case 10:
         w = heavysnow;
         break;
      case 11:
         w = blizzard;
         break;
      case 12:
         w = sandstorm;
         break;
	  default:
		  w = wNONE;
   }
   return w;
}

Weather::Weather(){
   for(int i=0;i<MAX_CLIMATES;i++) {
      climates[i].weather = clear;
      climates[i].windamount = 0;
      climates[i].wind = nowind;
      climates[i].climate = static_cast<ClimateType>(i);
      
      switch(int_to_climate_enum(i)){ //so we can use the enum names
         case temperate:
            climates[i].weather_base = temperate_weights;
            climates[i].temperature_base = temperate_temp_weights;
         break;
         case savanah:
            climates[i].weather_base = savanah_weights;
            climates[i].temperature_base = savanah_temp_weights;
         break;
         case mountain:
            climates[i].weather_base = mountain_weights;
            climates[i].temperature_base = mountain_temp_weights;
         break;
         case snowymountain:
            climates[i].weather_base = mountain_weights;
            climates[i].temperature_base = snowymountain_temp_weights;
         break;
         case sandydesert:
            climates[i].weather_base = sandydesert_weights;
            climates[i].temperature_base = sandydesert_temp_weights;
         break;
         case dirtdesert:
            climates[i].weather_base = dirtdesert_weights;
            climates[i].temperature_base = dirtdesert_temp_weights;
         break;
         case swamp:
            climates[i].weather_base = swamp_weights;
            climates[i].temperature_base = swamp_temp_weights;
         break;
         case tropical:
            climates[i].weather_base = tropical_weights;
            climates[i].temperature_base = tropical_temp_weights;
         break;
         case arctic:
            climates[i].weather_base = arctic_weights;
            climates[i].temperature_base = arctic_temp_weights;
         break;
         default:
           abort();
         break;
      };
   }//for
}

void Weather::update(){

   int i = 0; 
   while(i < MAX_CLIMATES){
      //increasing chance of weather change over time,
      // max of 20 in game hours of a particular weather
      //if(climates[i].weather_time * 5 > rand()%100){
     // memcpy(&c,&i,sizeof(ClimateType));
      changeWeather(int_to_climate_enum(i));
      //}
      
      //wind changes faster than weather does, max of 4 in game hours
      //if(20 + climates[i].wind_time * 20 > rand()%100){
  //       changeWind(i);
      //}
      ++i;
   }
}

void Weather::changeWeather(ClimateType i, WeatherType tw){
// this will randomly change weather or set it to the specified type
   unsigned int idx = 0;
   int weights[MAX_WEATHER];
   DistProbMatrix<WeatherType> weather_prob;
   DistProbMatrix<TemperatureType> temp_prob;
   int tweights[MAX_TEMP];

   //grab base weights for weather and temp
   memcpy(weights,climates[i].weather_base,MAX_WEATHER*sizeof(int)); 
   memcpy(tweights,climates[i].temperature_base,MAX_TEMP*sizeof(int)); 
   //combine current weather weights with base weather
   combine_weights(weights,weather_weights[climates[i].weather],MAX_WEATHER); 
   // combine season weights for weather and temperature
   combine_weights(weights,season_weights[get_season(config.day)],
            MAX_WEATHER);
   combine_weights(tweights,season_temp_weights[get_season(config.day)],
            MAX_TEMP);

   if(tw == wNONE){ 
    //build the weather probability matrix
      while(idx < MAX_WEATHER){
		  if(weights[idx]!=0) weather_prob.add(int_to_weather_enum(idx),weights[idx]);
            ++idx;
      }
      //get our weather
	  climates[i].weather = weather_prob.get();
   }else climates[i].weather = tw;
   //we have our new weather, so we can combine weather and temp weights
   combine_weights(tweights,weather_temp_weights[i],MAX_TEMP);
  
   //build temp prob matrix
   idx =0;
   while(idx < MAX_TEMP){
	   if(weights[idx] != 0) temp_prob.add(int_to_temperature_enum(idx),tweights[idx]);
     ++idx;
   }
   climates[i].temperature = temp_prob.get();

    //get our wind values
   climates[i].windamount = 1+wind_min[i] + 
      (rand()%(wind_max[i]-wind_min[i]));

   //set our wind enum
   climates[i].wind = int_to_wind_enum(climates[i].windamount);


	
   //if it's hailing, let's smack some people
   if(climates[i].weather == hail || climates[i].weather == hailstorm){
      Cell<critter*> cll(pc_list);
      critter* pc;
      critter* crit_ptr;
      room* rm;
      String buf(100);
      unsigned int dam;
      int prob=8; // set the prob unrealisticaly low so as to not annoy players

      if(climates[i].weather == hail){
         prob = 5;
      }

      while((pc = cll.next())){
         if(!pc->isImmort() || !pc->isNoHassle()){
            rm = pc->getCurRoom();
            if(rm->getClimate() == i){
               if((rand()%20 + 1)>prob){
                  if(d(1,pc->DEX) > d(1,20)){
                     Cell<critter*> rcll(rm->getCrits()); //holds mob        s in room
                     dam = d(1,5);
                     pc->takeDamage(dam,NORMAL);
                     switch(dam){
                        case 1:
                        case 2:
                        case 3:
                           if(pc->isSleeping()){
                              pc->setPosn(POS_REST);
                              pc->show("You are awakened by a piece of hail hitting you!\n");                          
                              while((crit_ptr = rcll.next())){
                                 if(crit_ptr != pc){
                                     Sprintf(buf, "%S is woken up by being hit with a piece of hail!\n",
                                     pc->getName(crit_ptr->SEE_BIT));
                                     crit_ptr->show(buf);
                                  }
                              }
                           }else if(pc->isMeditating()){
                              pc->setPosn(POS_REST);
                              pc->show("Your meditation is broken by a stinging sensation!\n"); 
                              while((crit_ptr = rcll.next())){
                                 if(crit_ptr != pc){
                                    Sprintf(buf, "%S's meditation is broken by a small piece of hail!!\n",
                                    pc->getName(crit_ptr->SEE_BIT));
                                    crit_ptr->show(buf);
                                 }
                              }
                           }else{
                              pc->show("A small piece of hail hits you!\n");
                              while((crit_ptr = rcll.next())){
                                 if(crit_ptr != pc){
                                    Sprintf(buf, "A small piece of hails hits %S!\n",
                                       pc->getName(crit_ptr->SEE_BIT));
                                       crit_ptr->show(buf);
                                    }
                                 }
                              }
                           break;
                        case 4:
                        case 5:
                           if(pc->isSleeping()){
                              pc->setPosn(POS_REST);
                              pc->show("You are awakened by a stringing sensation!\n");
                              while((crit_ptr = rcll.next())){
                              if(crit_ptr != pc){
                                    Sprintf(buf, "%S is woken up by being hit with a large piece of hail!\n",
                                    pc->getName(crit_ptr->SEE_BIT));
                                    crit_ptr->show(buf);
                                 }
                              }
                           }else if(pc->isMeditating()){
                              pc->setPosn(POS_REST);
                              pc->show("A stinging sensation breaks your meditation\n");
                              while((crit_ptr = rcll.next())){
                                 if(crit_ptr != pc){
                                       Sprintf(buf, "%S's meditation is disrupted by a large peice of hail !\n",
                                       pc->getName(crit_ptr->SEE_BIT));
                                     crit_ptr->show(buf);
                                 }
                              }
                           }else{
                              pc->show("A large piece of hail hits you!\n");
                              while((crit_ptr = rcll.next())){
                                 if(crit_ptr != pc){
                                    Sprintf(buf, "A large piece of hails hits %S!\n",
                                          pc->getName(crit_ptr->SEE_BIT));
                                    crit_ptr->show(buf);
                                 }
                              }
                           }
                          break;
                     }
                  } else {
                     Cell<critter*> rcll(rm->getCrits()); //holds mobs in room
                     if(pc->isSleeping() || pc->isMeditating()){
                       while((crit_ptr = rcll.next())){
                          if(crit_ptr != pc){
                             if(crit_ptr->canSee(*pc)){
                                Sprintf(buf,
                                    "%S is nearly hit hit by a piece of hail!\n",
                                    pc->getName());
                                crit_ptr->show(buf);
                             }
                          }
                       }
                     }else{
                        pc->show("You barely avoid being hit by a piece of hail!\n");
                        while((crit_ptr = rcll.next())){
                           if(crit_ptr != pc){
                              if(crit_ptr->canSee(*pc)){
                                 Sprintf(buf,
                                    "%S barely avoids being hit by a piece of hail!\n",
                                    pc->getName());
                                 crit_ptr->show(buf);
                              }
                           }  
                        }
                     }
                  }
               }
            }
         }
      }   
   }

   if(climates[i].weather == thunderstorm){

	   Cell<critter*> cll(pc_list);
	   critter* pc;
	   critter* crit_ptr;
	   room* rm;
	   String buf(100);         
	   int dam;

	   if(d(1,100) <=50 ){//50% chance of thunder
		   while((pc = cll.next())){
			   rm = pc->getCurRoom();
			   if(rm->getClimate() == i){
				   if(d(1,1000) == 1){
					   Cell<critter*> rcll(rm->getCrits());
					   dam = d(3,30);
					   //struck by lightning
					   pc->takeDamage(dam, ELECTRICITY);
					   if(pc->isSleeping()){
						   pc->setPosn(POS_REST);
						   pc->show("You awaken to the intense pain of being struck by lightning!\n");

						   while((crit_ptr = rcll.next())){
							   if(crit_ptr != pc){
								   if(crit_ptr->canSee(*pc)){
									   Sprintf(buf,
										   "%S is struck by lightning!!\n",
										   pc->getName());
									   crit_ptr->show(buf);
								   }
							   }
						   }     
					   }else if(pc->isMeditating()){
						   pc->setPosn(POS_REST);
						   pc->show("You meditation is broken by a bolt of lightning striking you!\n");
						   while((crit_ptr = rcll.next())){
							   if(crit_ptr != pc){
								   if(crit_ptr->canSee(*pc)){
									   Sprintf(buf,
										   "%S is struck by lightning!!\n",
										   pc->getName());
									   crit_ptr->show(buf);
								   }
							   }
						   }                                 
					   }else {
						   pc->show("You've been struck by lightning!\n");
						   while((crit_ptr = rcll.next())){
							   if(crit_ptr != pc){
								   if(crit_ptr->canSee(*pc)){
									   Sprintf(buf,
										   "%S is struck by lightning!!\n",
										   pc->getName());
									   crit_ptr->show(buf);
								   }
							   }
						   }                  
					   }  
				   }
				   else{
					   if(pc->isSleeping()){
						   pc->setPosn(POS_REST);
						   pc->show("The crash of nearby thunder awakens you!\n");
					   } else if(pc->isMeditating()){
						   if(d(1,100) < get_percent_lrnd(MEDITATION_SKILL_NUM, *pc)){
							   //do nothing right now
						   }else{
							   pc->setPosn(POS_REST);
							   pc->show("A loud crash of thunder breaks your meditation!\n");
						   }
					   } else{
						   pc->show("A brief flash of lightning is followed a crash of thunder!\n");
					   }
				   }
			   }
		   }
	   }
   }   
}

bool Weather::isRaining(ClimateType c) const {

   switch(climates[c].weather){
      case drizzle:
      case rain:
      case heavyrain:
      case thunderstorm:
      case hail:
      case hailstorm:
         return true;
      default:
         return false;
   }
}
 
bool Weather::isSnowing(ClimateType c) const {

   switch(climates[c].weather){
      case lightsnow:
      case heavysnow:
      case blizzard:
         return true;
      default:
         return false;
   }
}
