//
//ScryMUD Server Code
//Copyright (C) 2006,2007 Andrew Gibson
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
// To contact the Author, Andrew Gibson: gingon@wanfear.com 
//
///********************  weaher.h  *****************************///

#ifndef __WEATHER_INCLUDE
#define __WEATHER_INCLUDE
//we don't actually use most of these defines
//climate types
#define CTEMPERATE      0
#define CSAVANAH        1
#define CMOUNTAIN       2
#define CSNOWYMOUNTAIN  3
#define CSANDYDESERT    4
#define CDIRTDESERT     5
#define CSWAMP          6
#define CTROPCIAL       7
#define CARCTIC         8
//weather types
#define WCLEAR          0
#define WCLOUDY         1
#define WOVERCAST       2
#define WDIRZZLE        3
#define WRAIN           4
#define WHEAVYRAIN      5
#define WTHUNDERSTORM   6
#define WHAIL           7
#define WHAILSTORM      8
#define WLIGHTSOW       9
#define WHEAVYSNOW      10
#define WBLIZZARD       11
#define WSANDSTORM      12
//temperature types
#define TFREEZING       0
#define TVERYCOLD       1
#define TCOLD           2
#define TCHILLY         3
#define TPLEASENT       4
#define TWARM           5
#define THOT            6
#define TVERYHOT        7
#define TBURNINGHOT     8
//fog types
#define FNONE     0
#define FLIGHT    1
#define FMEDIUM   2
#define FHEAVY    3
//wind types
#define WDNONE       0
#define WDGENTLE     1
#define WDLIGHT      2
#define WDMODERATE   3
#define WDHEAVY      4
#define WDGALE       5

//we do use these ones
#define MAX_CLIMATES 9
#define MAX_WEATHER 13
#define MAX_FOG      4
#define MAX_WIND     6
#define MAX_TEMP     9


extern const int temperate_weights[];
extern const int savanah_weights[];
extern const int mountain_weights[];
extern const int snowymountain_weights[];
extern const int sandydesert_weights[];
extern const int dirtdesert_weights[];
extern const int swamp_weights[];
extern const int tropical_weights[];
extern const int arctic_weights[];

extern const float clear_weights[];
extern const float cloudy_weights[];
extern const float overcast_weights[];
extern const float drizzle_weights[];
extern const float rain_weights[];
extern const float heavyrain_weights[];
extern const float thunderstorm_weights[];
extern const float hail_weights[];
extern const float hailstorm_weights[];
extern const float lightsnow_weights[];
extern const float heavysnow_weights[];
extern const float blizzard_weights[];
extern const float sandstorm_weights[];
extern const float* weather_weights[];

extern const float spring_weights[];
extern const float summer_weights[];
extern const float fall_weights[];
extern const float winter_weights[];
extern const float* season_weights[];
//temperature
extern const int temperate_temp_weights[];
extern const int savanah_temp_weights[];
extern const int mountain_temp_weights[];
extern const int snowymountain_temp_weights[];
extern const int sandydesert_temp_weights[];
extern const int dirtdesert_temp_weights[];
extern const int swamp_temp_weights[];
extern const int tropical_temp_weights[];
extern const int arctic_temp_weights[];

extern const float clear_temp_weights[];
extern const float cloudy_temp_weights[];
extern const float overcast_temp_weights[];
extern const float drizzle_temp_weights[];
extern const float rain_temp_weights[];
extern const float heavyrain_temp_weights[];
extern const float thunderstorm_temp_weights[];
extern const float hail_temp_weights[];
extern const float hailstorm_temp_weights[];
extern const float lightsnow_temp_weights[];
extern const float heavysnow_temp_weights[];
extern const float blizzard_temp_weights[];
extern const float sandstorm_temp_weights[];
extern const float* weather_temp_weights[];

extern const float spring_temp_weights[];
extern const float summer_temp_weights[];
extern const float fall_temp_weights[];
extern const float winter_temp_weights[];
extern const float* season_temp_weights[];

//wind
extern const int wind_min[];
extern const int wind_max[];
//strings
extern const char* climate_strings[];
extern const char* weather_strings[];
extern const char* wind_strings[];
extern const char* temperature_strings[];
//mods
extern const float weather_regen_mods[];
extern const float temperature_regen_mods[];
extern const float weather_mv_mods[];
extern const float temperature_mv_mods[];

//enum FogType {none = 0, lightfog, mediumfog, heavyfog};

enum WeatherType {clear =0, cloudy, overcast, drizzle, 
   rain, heavyrain, thunderstorm, hail, hailstorm, lightsnow, heavysnow,
   blizzard, sandstorm, wNONE}; 

enum WindType {nowind = 0, gentlebreeze, lightwind, windy, verywindy, gale,
   wndNONE};

enum ClimateType {temperate = 0, savanah, mountain, snowymountain, sandydesert, 
   dirtdesert, swamp, tropical, arctic, cNONE};

enum TemperatureType {freezing=0, verycold, cold, chilly, pleasant, 
   warm, hot, veryhot, burninghot, tNONE};

struct Climate{
   ClimateType climate;
   unsigned int windamount;
   WeatherType weather;
   WindType wind;
   TemperatureType temperature;
   //store these here so we don't need a big switch in changeWeather
   const int* weather_base;
   const int* temperature_base;
};

class Weather{
public:
   Climate climates[MAX_CLIMATES];
   void update();
   void changeWeather(ClimateType i,WeatherType tw = wNONE);
   bool isRaining(ClimateType c) const;
   bool isSnowing(ClimateType c) const;
//   void changeWind(int i);
//   int weather_base[MAX_CLIMATES][MAX_WEATHER];
   Weather();
};

TemperatureType int_to_temperature_enum(unsigned int i);
WindType int_to_wind_enum(unsigned int i);
ClimateType int_to_climate_enum(unsigned int i);
WeatherType int_to_weather_enum(int i);
#endif
