# $Id: translation_misc.spec,v 1.3 1999/08/29 01:17:17 greear Exp $
# $Revision: 1.3 $  $Author: greear $ $Date: 1999/08/29 01:17:17 $

# The syntax is as follows:
# ENUM_NAME LANGUAGE "TEXT" LANGUAGE "TEXT" ... ~
#
# The parser is free form in that white-space does not matter.
# All strings should be in double quotes, and you must terminate
# with a ~ (tilde) as a 'language'.
#
# NOTE:  Order is not important, and you may abbreviate the language,
#        so long as you have enough significant characters to
#        distinguish amongst them.
#
# Comments must not be in the middle of a definition.  In other words,
# only right before the ENUM name.  Comments must not be longer than
# 200 characters per line!!
#
# Languages currently supported by the code_generation process are (at least):
# English, Spanish, Portugues, Italian

# This is used as a kind of NULL enum.  It should never be used
# where a real CSentry entity is desired.

# See translations.spec for more details...


CS_GLOWS_BRIGHTLY
	eng "\t\t%s glows brightly.\n"
	~

CS_CLOSES_N_VANISHES
	eng "%S closes up and vanishes.\n"
	~

CS_THE_NEWBIE
	eng " the newbie"
	~

CS_NEWBIE_LONG_DESC
	eng "You see someone who is quite normal."
	~

CS_CONSUME_TELEPORT_F
	eng "consume_teleport"
	~

CS_CONSTR_DATA_F
	eng "has_construct_data"
	~

CS_HAS_SKIN_F
	eng "has_skin"
	~

CS_CONSUME_POISON_F
	eng "consume_poison"
	~

CS_NULL
	eng "NULL"
	~

CS_CASTS_SPELLS_F
	eng "casts_spells"
	~

CS_OBJ_SPEC_DATA_FLAGS
	eng "Obj Spec Data Flags:"
	~

CS_OPEN_EXIT_F
	eng "Open_Exit"
	~

CS_MAG_LOCKABLE_F
	eng "Mag_Lockable"
	~

CS_IS_CLOSED_F
	eng "is_closed"
	~

CS_IS_PICKABLE_F
	eng "is_pickable"
	~

CS_IS_LOCKED_F
	eng "Is_Locked"
	~

CS_IS_MAG_LOCKED_F
	eng "is_mag_locked"
	~

CS_IS_DESTRUCTABLE_F
	eng "is_destructable {fireball may open it till reset}"
	~

CS_IS_CLOSABLE_F
	eng "Is_Closable"
	~

CS_IS_FLIPPABLE_F
	eng "Is_Flippable"
	~

CS_IS_IN_USE_F
	eng "Is_In_Use"
	~

CS_IS_NO_OPEN_F
	eng "Is_Unopenable"
	~

CS_IS_VEHICLE_EXIT_F
	eng "Is_Vehicle_Exit"
	~

CS_IS_SECRET_F
	eng "Is_Secret"
	~

CS_IS_BLOCKED_F
	eng "Is_Blocked"
	~

CS_IS_NOT_COMPLETE_F
	eng "Is_Not_Complete"
	~

CS_SECRET_WHEN_OPEN_F
	eng "Secret_When_Open"
	~

CS_CONSUME_KEY_F
	eng "consume_key"
	~

CS_NO_PASSDOOR_F
	eng "No_Passdoor"
	~

CS_IS_CORPSE_F
	eng "Is_Corpse"
	~

CS_CONTAINER_FLAGS
	eng "Container Flags:"
	~

CS_BUY_ZERO_F
	eng "Buy_0"
	~

CS_SELL_ZERO_F
	eng "Sell_0"
	~

CS_OFFER_ZERO_F
	eng "Offer_0"
	~

CS_PLAYER_RUN_SHOPKEEPER_F
	eng "Player-run Shopkeeper"
	~

CS_NON_WEAPON_F
	eng "NON weapon"
	~

CS_SLASH_F
	eng "slash"
	~

CS_SMASH_F
	eng "smash"
	~

CS_PIERCE_F
	eng "pierce"
	~

CS_WHIP_F
	eng "whip"
	~

CS_NEEDS_AMMO_F
	eng "needs_ammo"
	~

CS_DART_THROWER_F
	eng "dart_thrower"
	~

CS_BOW_F
	eng "bow"
	~

CS_DART_F
	eng "dart"
	~

CS_ARROW_F
	eng "arrow"
	~

CS_JUNK_F
	eng "arrow"
	~

CS_WAND_F
	eng "wand"
	~

CS_POTION_F
	eng "potion"
	~

CS_SCROLL_F
	eng "scroll"
	~

CS_CONTAINER_F
	eng "container"
	~

CS_COINS_F
	eng "coins"
	~

CS_ARMOR_F
	eng "armor"
	~

CS_WEAPON_F
	eng "weapon"
	~

CS_LIGHT_SOURCE_F
	eng "light_source"
	~

CS_CANTEEN_F
	eng "canteen"
	~

CS_LIQUID_F
	eng "liquid"
	~

CS_FOOD_F
	eng "food"
	~

CS_BOAT_F
	eng "boat"
	~

CS_HAS_PROC_DATA_F
	eng "has_spec_proc_data"
	~

CS_TOOLBOX_F
	eng "toolbox"
	~

CS_CAULDRON_F
	eng "cauldron"
	~

CS_PEN_F
	eng "pen"
	~

CS_CONSTR_COMP_F
	eng "construct_component"
	~

CS_CONCOCT_COMP_F
	eng "concoct_component"
	~

CS_PARCHMENT_F
	eng "parchment"
	~

CS_HERB_F
	eng "herb"
	~

CS_VEND_MACHINE_F
	eng "Vending_Machine"
	~

CS_SHOP_DATA_FLAGS
	eng "Shop Data Flags (Trades these):"
	~

CS_WARRIOR
	eng "Warrior"
	~

CS_SAGE
	eng "Sage"
	~

CS_WIZARD
	eng "Wizard"
	~

CS_RANGER
	eng "Ranger"
	~

CS_THIEF
	eng "Thief"
	~

CS_ALCHEMIST
	eng "Alchemist"
	~

CS_CLERIC
	eng "Cleric"
	~

CS_BARD
	eng "Bard"
	~

CS_TEACH_DATA_FLAGS
	eng "Teach Data Flags:"
	~

CS_SHOPKEEPER_F
	eng "shopkeeper"
	~

CS_TEACHER_F
	eng "Teacher"
	~

CS_SAME_CLASS_PASS_F
	eng "let_same_class_pass"
	~

CS_LET_SAME_RACE_PASS_F
	eng "let_same_race_pass"
	~

CS_HAS_AI_F
	eng "has_AI"
	~

CS_MOB_PROC_FLAGS
	eng "Mob Proc Flags:"
	~

CS_CAN_SEE_INV_F
	eng "can_see_inv"
	~

CS_USING_LIGHT_SRC_F
	eng "using_light_src"
	~

CS_IS_FLYING_F
	eng "is_flying"
	~

CS_HAVE_BOAT_F
	eng "have_boat"
	~

CS_CAN_CLIMB_F
	eng "can_climb"
	~

CS_GOS_F
	eng "gossip_channel"
	~

CS_YELL_F
	eng "yell_channel"
	~

CS_GRATZ_F
	eng "gratz_channel"
	~

CS_AUC_F
	eng "auction_channel"
	~

CS_SHOUT_F
	eng "shout_channel"
	~

CS_SAY_F
	eng "say_channel"
	~

CS_TELL_F
	eng "tell_channel"
	~

CS_WIZNET_F
	eng "wiznet_channel"
	~

CS_PARALYZED_F
	eng "is_paralyzed"
	~

CS_PERM_SLEEPED_F
	eng "is_perm_sleeped"
	~

CS_DUAL_WIELDING_F
	eng "is_dual_wielding"
	~

CS_IS_SNEAK_F
	eng "is_sneak"
	~

CS_CAN_DIVE_F
	eng "can_dive"
	~

CS_SPELL_TESTED_YET_F
	eng "spell_tested_yet"
	~

CS_IS_HIDING_F
	eng "is_hiding"
	~

CS_IS_TAILING_F
	eng "is_tailing"
	~

CS_ALREADY_HURLED_F
	eng "already_hurled"
	~

CS_CRIT_FLAGS
	eng "Critter Flags:"
	~

CS_SCAVENGE_F
	eng "scavenge"
	~

CS_WANDER_F
	eng "wander"
	~

CS_SHOULD_DO_PROCS_F
	eng "should_do_procs"
	~

CS_NEEDS_RESETTING_F
	eng "needs_resetting"
	~

CS_EDIBLE_CORPSE_F
	eng "edible_corpse"
	~

CS_IS_BANKER_F
	eng "is_banker"
	~

CS_IS_SESSILE_F
	eng "is_sessile"
	~

CS_IS_NO_HOMING_F
	eng "NO_homing"
	~

CS_HAS_SCRIPT_F
	eng "has_script"
	~

CS_MOB_DATA_FLAGS
	eng "Mob Data Flags:"
	~

CS_FROZEN_F
	eng "frozen"
	~

CS_GAGGED_F
	eng "gagged"
	~

CS_HAS_IMM_DATA_F
	eng "has_imm_data"
	~

CS_CLOAKED_F
	eng "is_cloaked"
	~

CS_TANK_GRAPH_F
	eng "do_tank_graph"
	~

CS_USING_CLIENT_F
	eng "using_client_tags"
	~

CS_AUTO_EXIT_F
	eng "auto_exit"
	~

CS_NO_HASSLE_F
	eng "NO_hassle"
	~

CS_BRIEF_F
	eng "brief"
	~

CS_AUTOSPLIT_F
	eng "autosplit"
	~

CS_DO_PROMPT_F
	eng "do_prompt"
	~

CS_IS_BUILDER_F
	eng "is_builder"
	~

CS_AUTOLOOT_F
	eng "auto_loot"
	~

CS_OLC_REDO_F
	eng "olc_redo"
	~

CS_EXTRA_INFO_F
	eng "extra_info"
	~

CS_CR_BEHIND_F
	eng "cr_behind_nl"
	~

CS_DO_CR_F
	eng "do_carriage_return"
	~

CS_IS_BLOCKING_DOOR_F
	eng "is_blocking_door"
	~

CS_CAN_DET_MAGIC_F
	eng "can_det_magic"
	~

CS_DET_INV_F
	eng "detect_inventory"
	~

CS_SHOW_VNUMS_F
	eng "show_vnums"
	~

CS_HAS_POOF_MSGS_F
	eng "has_poofin_poofout_msg"
	~

CS_PAGE_OUTPUT_F
	eng "page_output"
	~

CS_PB_MODE_F
	eng "in_page_break_mode"
	~

CS_NO_WIZCHAT_F
	eng "NO_wizchat"
	~

CS_HAS_COLORS_F
	eng "has_colors"
	~

CS_USE_COLOR_F
	eng "use_color"
	~

CS_HAS_LANG_CHOICE_F
	eng "has_language_choice"
	~

CS_NO_SHOW_NPC_ENTRY_F
	eng "NO_show_NPC_entry"
	~

CS_NO_BEEP_F
	eng "no_beep"
	~

CS_PC_DATA_FLAGS
	eng "PC Data Flags:"
	~

CS_IS_SELF_GUIDED_F
	eng "is_self_guided"
	~

CS_STEALTHY_F
	eng "is_stealthy"
	~

CS_UNLIMITED_FUEL_F
	eng "unlimited_fuel"
	~


CS_CAN_FLOAT_F
	eng "can_float"
	~

CS_CAN_SEE_OUT_F
	eng "can_see_out (show room descs to passengers)"
	~

CS_VEH_FLAGS
	eng "Vehicle Flags:"
	~

CS_IS_PERM_DARK_F
	eng "is_perm_dark"
	~

CS_HAS_WEATHER_F
	eng "has_weather"
	~

CS_NO_SHOUT_F
	eng "NO_shout"
	~

CS_NO_MAG_EXIT_F
	eng "NO_mag_exit"
	~

CS_HAVEN_F
	eng "is_haven"
	~

CS_NO_PK_F
	eng "Anti-PK"
	~

CS_NO_MAGIC_F
	eng "Anti-Magic"
	~

CS_ANTI_NPC_F
	eng "Anti-NPC"
	~

CS_ANTI_POTION_F
	eng "Anti-Potion"
	~
CS_ANTI_STAFF_F
	eng "Anti-Staffs"
	~

CS_ANTI_MORT_F
	eng "Anti_Mortal"
	~

CS_NORMAL_DARK_F
	eng "normally_dark"
	~

CS_RIVER_LAKE_F
	eng "river/small lake"
	~

CS_OCEAN_LAKE_F
	eng "ocean/big lake"
	~

CS_SWAMP_F
	eng "swamp"
	~

CS_NEED_FLY_F
	eng "need_fly"
	~

CS_NEED_BOAT_F
	eng "need_boat"
	~

CS_NEED_CLIMB_F
	eng "need_climb"
	~

CS_ZLOCKED_F
	eng "is_zlocked (for olc)"
	~

CS_TOT_LOADED_F
	eng "is_totally_loaded"
	~

CS_NO_MAG_ENTRY_F
	eng "Anti-Magic-Entry"
	~

CS_NO_VEH_F
	eng "!vehicle (vehicles can't drive here)"
	~

CS_CRAMPED_F
	eng "cramped (!huge)"
	~

CS_NO_RANGED_F
	eng "Anti-Ranged-Combat"
	~

CS_NEED_DIVE_F
	eng "need_dive_ability"
	~

CS_USED_IN_TRACK_F
	eng "Used-In-Track"
	~

CS_CAN_CAMP_F
	eng "can_camp"
	~

CS_HAS_KEYWORDS_F
	eng "has_keywords"
	~

CS_NO_MOB_WANDER_F
	eng "NO_mob_wander"
	~

CS_NO_FOREIGN_MOB_WANDER_F
	eng "NO_foreign_mob_wander"
	~

CS_ROOM_FLAGS
	eng "Room Flags:"
	~

CS_NO_EVIL_F
	eng "Anti-Evil"
	~

CS_NO_NEUT_F
	eng "Anti-Neutral"
	~

CS_NO_GOOD_F
	eng "Anti-Good"
	~

CS_NO_DONATE_F
	eng "Anti-Donate"
	~

CS_NO_DROP_F
	eng "Anti-Drop"
	~

CS_NO_REM_F
	eng "Anti-Remove"
	~

CS_NO_IMM_F
	eng "Anti-Immortal"
	~

CS_NO_GOD_F
	eng "Anti-God"
	~

CS_NO_DEMI_F
	eng "Anti-Demi"
	~

CS_NO_WARRIOR_F
	eng "Anti-Warrior"
	~

CS_NO_SAGE_F
	eng "Anti-Sage"
	~

CS_NO_WIZ_F
	eng "Anti-Wizard"
	~

CS_NO_RANGER_F
	eng "Anti-Ranger"
	~

CS_NO_THIEF_F
	eng "Anti-Thief"
	~

CS_NO_ALCH_F
	eng "Anti-Alchemist"
	~

CS_NO_CLERIC_F
	eng "Anti-Cleric"
	~

CS_NO_BARD_F
	eng "Anti-Bard"
	~

CS_NO_PC_F
	eng "Anti-PC"
	~

CS_NOT_WORN_F
	eng "Not-worn"
	~

CS_WORN_HEAD_F	
	eng "Worn-head"
	~

CS_WORN_NECK_F
	eng "Worn-Neck"
	~

CS_ARND_BODY_F
	eng "Worn-around-body"
	~

CS_WORN_ARMS_F
	eng "Worn-Arms"
	~

CS_WORN_W1_F
	eng "Worn-wrist1"
	~

CS_WORN_W2_F
	eng "Worn-wrist2"
	~

CS_WORN_HANDS_F
	eng "Worn-Hands"
	~

CS_WIELDED_F
	eng "Wielded"
	~

CS_HELD_F
	eng "Held"
	~

CS_WORN_LIGHT_F
	eng "Worn-Light"
	~

CS_WORN_BODY_F
	eng "Worn-body"
	~

CS_BELT_F
	eng "Belt"
	~

CS_WORN_LEGS_F
	eng "Worn-Legs"
	~

CS_WORN_FEET_F
	eng "Worn-Feet"
	~

CS_WORN_FINGER1_F
	eng "Worn-finger-1"
	~

CS_WORN_FINGER2_F
	eng "Worn-finger-2"
	~

CS_SHIELD_F
	eng "Shield"
	~

CS_BULLETIN_BOARD_F
	eng "Bulletin_board"
	~

CS_BUTCHERABLE_F
	eng "Is_butcherable"
	~


CS_OBJ_FLAGS
	eng "Object Flags:"
	~

CS_POSN_F
	eng "position"
	~

CS_STR_F
	eng "Strength"
	~

CS_INT_F
	eng "Inteligence"
	~

CS_CON_F
	eng "Constitution"
	~

CS_CHA_F
	eng "Charisma"
	~

CS_WIZ_F
	eng "Wisdom"
	~

CS_DEX_F
	eng "Dexterity"
	~

CS_HIT_F
	eng "HIT"
	~

CS_DAM_F
	eng "Damage"
	~

CS_AC_F
	eng "AC"
	~

CS_ATTACKS_F
	eng "Attacks"
	~

CS_PAUSE_F
	eng "PAUSE_CNT"
	~

CS_SEX_F
	eng "SEX"
	~

CS_CLASS_F
	eng "CLASS"
	~

CS_RACE_F
	eng "RACE"
	~

CS_CUR_HP_F
	eng "Current HP"
	~

CS_CUR_MA_F
	eng "Current MANA"
	~

CS_CUR_MOV_F
	eng "Current Movement"
	~

CS_ALIGN_F
	eng "Alignment"
	~

CS_LVL_F
	eng "Level"
	~

CS_HOMETOWN_F
	eng "Hometown"
	~

CS_WIMPY_F
	eng "Wimpy"
	~

CS_PRAC_F
	eng "practices"
	~

CS_MAX_HP_F
	eng "MAX HP"
	~

CS_MAX_MA_F
	eng "MAX Mana"
	~

CS_MAX_MOV_F
	eng "MAX Movement"
	~

CS_CTYPE_F
	eng "PC/SMOB/MOB"
	~

CS_DRM_F
	eng "DAM_RECV_Modifier"
	~

CS_DGM_F
	eng "DAM_Given_Modifier"
	~

CS_NO_REST_F
	eng "No Restrictions"
	~

CS_HT_RESIS_F
	eng "Heat Resistance"
	~

CS_CLD_RESIS_F
	eng "Cold Resistance"
	~

CS_ELEC_RESIS_F
	eng "Electrical Resistance"
	~

CS_SPELL_RESIS_F
	eng "Spell Resistance"
	~

CS_RELIGION_F
	eng "Religion"
	~

CS_HOME_ZONE_F
	eng "Belongs to Zone"
	~

CS_BHDC_F
	eng "Bare Hand Dice Count"
	~

CS_BHDS_F
	eng "Bare Hand Dice Sides"
	~

CS_HP_REGEN_F
	eng "HP Regeneration"
	~

CS_MA_REGEN_F
	eng "MANA Regeneration"
	~

CS_MV_REGEN_F
	eng "Movement Regeneration"
	~

CS_GUILD_F
	eng "Guild"
	~

CS_NAKED_WEIGHT_F
	eng "Naked Weight"
	~

CS_STAT_AFFECTS
	eng "Stat Affects:"
	~