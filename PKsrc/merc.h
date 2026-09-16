/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#ifndef __MERC_H_
#define __MERC_H_

#include <time.h>
#include <stdio.h>
#include <unistd.h>

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_INTERACTIVE( fun )	bool fun( )
#define DECLARE_ROOM_SPECIAL( fun )	bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#define DECLARE_WEAR_FUN( fun )		void fun( )
#define DECLARE_OBJECT_FUN( fun )	bool fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_INTERACTIVE( fun )	INTERACTIVE fun
#define DECLARE_ROOM_SPECIAL( fun )	ROOM_SPECIAL fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#define DECLARE_WEAR_FUN( fun )		WEAR_FUN fun
#define DECLARE_OBJECT_FUN( fun )	OBJECT_FUN fun
#endif



/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int				sh_int;
typedef int				bool;
#define unix
#else
typedef short    int			sh_int;
#ifndef CPPBUILD
typedef int				bool;
#endif
#endif


/*
 * Structure types.
 */
typedef struct	affect_data		AFFECT_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;



/*
 * Function types.
 */
typedef	void DO_FUN	args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN	args( ( CHAR_DATA *ch ) );
typedef bool INTERACTIVE args( ( CHAR_DATA *mob, CHAR_DATA *ch,
	 char *command, char *argument ) );
typedef bool ROOM_SPECIAL args( ( CHAR_DATA *ch, char *command, char *argument ) );
typedef void SPELL_FUN	args( ( int sn, int level, CHAR_DATA *ch, void *vo ) );
typedef void WEAR_FUN	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool on_off ) );
typedef bool OBJECT_FUN args( ( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument ) );



/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 4096
#define MAX_INPUT_LENGTH	  160

/*
 *  Arbitrary max vnum.
 */
#define MAX_VNUM 32768


/*
 * boot reasons.
 */
#define BOOT_CRASH 0
#define BOOT_REBOOT 1


/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_SKILL		  200
#define MAX_CLASS		   27
#define MAX_LEVEL		   41
#define MAX_NAME_LENGTH            12
#define LEVEL_HERO		   (MAX_LEVEL - 5)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 4)


#define PULSE_PER_SECOND	    4
#define PULSE_VIOLENCE		  ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  ( 4 * PULSE_PER_SECOND)
/*
#define PULSE_TICK		  (30 * PULSE_PER_SECOND)
*/
#define PULSE_TICK                (75 * PULSE_PER_SECOND)
#define PULSE_AREA		  (60 * PULSE_PER_SECOND)



/*
 * war definitions
 */
#define	NO_WAR			0
#define PRE_WAR			1
#define WAR_ON			2
#define POST_WAR		3

/*
 *	war flags
 */

#define WAR_FLAG_BLOODS		1
#define WAR_FLAG_CONTINUOUS	2
#define WAR_FLAG_LOADALL	4
#define WAR_FLAG_UNUSED		8
#define WAR_FLAG_NOREGEN       16
#define WAR_FLAG_SUMMON	       32
#define WAR_FLAG_FOUNTAINS     64
#define WAR_FLAG_RANDOMCLASS  128
#define WAR_FLAG_TELEALL      256
#define WAR_FLAG_NOQUAFF      512
#define WAR_FLAG_HEALTHLAG   1024
#define WAR_FLAG_NOEXTEND    2048

/*
 * War numbers.
 */
#define WAR_NUM_DEAD            0
#define WAR_NUM_MAIN            1
#define WAR_NUM_FFA             2
#define WAR_NUM_PRIVATE         3

#define NUM_PHASES              4


/* wear proc defs */
#define PUT_ON			1
#define TAKE_OFF		0

/* class definitions */
#define CLASS_MAGE			0
#define CLASS_CLERIC			1
#define CLASS_THIEF			2
#define CLASS_WARRIOR			3
#define CLASS_RANGER   		4
#define CLASS_INVOKER			5
#define CLASS_ABJURER			6
#define CLASS_ALTERER			7
#define CLASS_SCOUT		8
#define CLASS_DIVINER			9
#define CLASS_ENCHANTER		10
#define CLASS_NECROMANCER		11
#define CLASS_ILLUSIONIST		12
#define CLASS_DEFILER			13
#define CLASS_PALADIN			14
#define CLASS_ANTI_PALADIN		15
#define CLASS_ARCHER		16
#define CLASS_BARBARIAN		17
#define CLASS_MONK			18
#define CLASS_ASSASSIN		19
#define CLASS_BARD			20
#define CLASS_DRAGON			21
#define CLASS_NINJA			22
#define CLASS_PSIONICIST		23
#define CLASS_SAMURAI		24
#define CLASS_DRUID		25
#define CLASS_SPELLFILCHER	26



/*
 * random tables
 */
#define TABLE_JEWELRY		1
#define TABLE_STAFF		2
#define TABLE_SCROLL		3
#define TABLE_QUIVER		4



struct high_score_element
{
  struct high_score_element *next;
  char name[MAX_NAME_LENGTH+1];
  int value;
  time_t time;
};


/*
 * Site ban structure.
 */
struct	ban_data
{
    BAN_DATA *	next;
    char *	name;
};



/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
};

struct	weather_data
{
    int		mmhg;
    int		change;
    int		sky;
    int		sunlight;
};



/*
 * Connected state for a channel.
 */
#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PASSWORD		 2
#define CON_GET_OLD_PASSWORD2		 3
#define CON_CONFIRM_NEW_NAME		 4
#define CON_GET_NEW_PASSWORD		 5
#define CON_CONFIRM_NEW_PASSWORD	 6
#define CON_GET_NEW_SEX			 7
#define CON_GET_NEW_CLASS		 8
#define CON_READ_MOTD			 9



struct snoop_data
{
    struct snoop_data *	next;
    DESCRIPTOR_DATA *	d;
};

/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    struct snoop_data *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
    char *		host;
    sh_int		descriptor;
    sh_int		connected;
    bool		fcommand;
    char		inbuf		[4 * MAX_INPUT_LENGTH];
    char		incomm		[MAX_INPUT_LENGTH];
    char		inlast		[MAX_INPUT_LENGTH];
    int			repeat;
    char *		outbuf;
    int			outsize;
    int			outtop;
/*
    bool		ansi_enable;
*/
};



/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    sh_int	tohit;
    sh_int	todam;
    sh_int	carry;
    sh_int	wield;
};

struct	int_app_type
{
    sh_int	learn;
};

struct	wis_app_type
{
    sh_int	practice;
};

struct	dex_app_type
{
    sh_int	defensive;
};

struct	con_app_type
{
    sh_int	hitp;
    sh_int	shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3



/*
 * Help table types.
 */
struct	help_data
{
    HELP_DATA *	next;
    sh_int	level;
    char *	keyword;
    char *	text;
};



/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
    sh_int	keeper;			/* Vnum of shop keeper mob	*/
    sh_int	buy_type [MAX_TRADE];	/* Item types shop will buy	*/
    sh_int	profit_buy;		/* Cost multiplier for buying	*/
    sh_int	profit_sell;		/* Cost multiplier for selling	*/
    sh_int	open_hour;		/* First opening hour		*/
    sh_int	close_hour;		/* First closing hour		*/
};



/*
 * Data structure for notes.
 */
struct	note_data
{
    NOTE_DATA *	next;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    char *	text;
};



/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    sh_int		type;
    sh_int		duration;
    sh_int		location;
    sh_int		modifier;
    int			bitvector;
};



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_RAMBO                     2
#define MOB_CRYSTAL_DRAGON            3
#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   3404

/* POKER THING */
#define POKER_OFF	0
#define POKER_WAITING	1
#define POKER_ANTE	2
#define POKER_BET	3
#define POKER_GIVEUP	4
#define POKER_END	5

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		      1		/* Auto set for mobs	*/
#define ACT_SENTINEL		      2		/* Stays in one room	*/
#define ACT_SCAVENGER		      4		/* Picks up objects	*/
#define ACT_AGGRESSIVE		     32		/* Attacks PC's		*/
#define ACT_STAY_AREA		     64		/* Won't leave area	*/
#define ACT_UNUSED		    128		/* Used to be wimpy	*/
#define ACT_TERMINATOR		    256		/* gets multiple updates */
#define ACT_TRAIN		    512		/* Can train PC's	*/
#define ACT_PRACTICE		   1024		/* Can practice PC's	*/
#define ACT_ILLUSION		   2048		/* a big fuzzy illusion	*/



/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		      1 /* 0x00000001 */
#define AFF_INVISIBLE		      2 /* 0x00000002 */
#define AFF_DETECT_EVIL		      4 /* 0x00000004 */
#define AFF_DETECT_INVIS	      8 /* 0x00000008 */
#define AFF_DETECT_MAGIC	     16 /* 0x00000010 */
#define AFF_DETECT_HIDDEN	     32 /* 0x00000020 */
/* removing the unused AFF_HOLD, replacing with AFF_NO_SUMMON */
/*
#define AFF_HOLD		     64
*/
#define AFF_NO_SUMMON                64 /* 0x00000040 */
#define AFF_SANCTUARY		    128 /* 0x00000080 */
#define AFF_FAERIE_FIRE		    256 /* 0x00000100 */
#define AFF_INFRARED		    512 /* 0x00000200 */
#define AFF_CURSE		   1024 /* 0x00000400 */
/* replacing AFF_FLAMING with AFF_FIRESHIELD */
#define AFF_FIRESHIELD		   2048 /* 0x00000800 */
#define AFF_POISON		   4096 /* 0x00001000 */
#define AFF_PROTECT		   8192 /* 0x00002000 */
#define AFF_PARALYSIS		  16384 /* 0x00004000 unused */
#define AFF_SNEAK		  32768 /* 0x00008000 */
#define AFF_HIDE		  65536 /* 0x00010000 */
#define AFF_SLEEP		 131072 /* 0x00020000 */
#define AFF_CHARM		 262144 /* 0x00040000 */
#define AFF_FLYING		 524288 /* 0x00080000 */
#define AFF_PASS_DOOR		1048576 /* 0x00100000 */
/* adding this sucker... */
#define AFF_IMPROVED_INVIS	2097152 /* 0x00200000 */
#define AFF_BLACKMANTLE         4194304 /* 0x00400000 */
#define AFF_FARSEE		8388608 /* 0x00800000 */
#define AFF_TOWERID	       16777216 /* 0x01000000 */
#define AFF_UNUSED	       33554432 /* 0x02000000 */
#define AFF_INVULNERABILITY    67108864 /* 0x04000000 */

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2


/* race definitions */
#define RACE_HUMAN		 0
#define RACE_ELF		 1
#define RACE_DWARF		 2
#define RACE_ORC		 3
#define RACE_HALF_ORC		 4
#define RACE_HALFLING		 5
#define RACE_HALF_ELF		 6
#define RACE_HUMANOID		 7
#define RACE_MINOTAUR		 8
#define RACE_ANIMAL		 9
#define RACE_UNDEAD		10
#define RACE_INSECT		11
#define RACE_BIRD		12
#define RACE_DRAGON		13
#define RACE_ELEMENTAL		14
#define RACE_DROW		15


/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_FINAL_TURD	     16

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22

#define OBJ_VNUM_WEAK_LANTERN        24

#define OBJ_RAMBO_GUTS               25
#define OBJ_DRAGON_TONGUE            26

#define OBJ_VNUM_GLINTING_RING     7190
#define OBJ_VNUM_PURPLE_CLOAK      7203
#define OBJ_VNUM_BRONZE_BREAST     3353
#define OBJ_VNUM_BRONZE_HELMET     3356
#define OBJ_VNUM_BRONZE_LEGGINGS   3357
#define OBJ_VNUM_BRONZE_BOOTS      3358
#define OBJ_VNUM_BRONZE_SLEEVES    3360
#define OBJ_VNUM_SWORDSMANS_GLOVES 4101
#define OBJ_VNUM_BLACK_DEMON_BLADE 4100

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_GRENADE                  6
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_UNKNOWN1		     11
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_JEWELRY                 30
#define ITEM_QUIVER                  31
#define ITEM_TRAP		     32

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		      1
#define ITEM_HUM		      2
#define ITEM_DARK		      4
#define ITEM_RENAME		      8
#define ITEM_EVIL		     16
#define ITEM_INVIS		     32
#define ITEM_MAGIC		     64
#define ITEM_NODROP		    128
#define ITEM_BLESS		    256
#define ITEM_ANTI_GOOD		    512
#define ITEM_ANTI_EVIL		   1024
#define ITEM_ANTI_NEUTRAL	   2048
#define ITEM_NOREMOVE		   4096
#define ITEM_INVENTORY		   8192
#define ITEM_REQUIRES_HAND        32768
#define ITEM_DISARM              524288
#define ITEM_RANGED_WEAPON      1048576
#define ITEM_RANGED_MAGIC	2097152

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		      1
#define ITEM_WEAR_FINGER	      2
#define ITEM_WEAR_NECK		      4
#define ITEM_WEAR_BODY		      8
#define ITEM_WEAR_HEAD		     16
#define ITEM_WEAR_LEGS		     32
#define ITEM_WEAR_FEET		     64
#define ITEM_WEAR_HANDS		    128 
#define ITEM_WEAR_ARMS		    256
#define ITEM_WEAR_SHIELD	    512
#define ITEM_WEAR_ABOUT		   1024 
#define ITEM_WEAR_WAIST		   2048
#define ITEM_WEAR_WRIST		   4096
#define ITEM_WIELD		   8192
#define ITEM_HOLD		  16384
#define ITEM_WEAR_BACK            32768   


/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_REGEN_HIT              25
#define APPLY_REGEN_MANA             26

#define APPLY_RESIST_FIRE            30
#define APPLY_RESIST_COLD            31
#define APPLY_RESIST_LIGHTNING       32
#define APPLY_RESIST_PHYSICAL        33
#define APPLY_RESIST_MAGIC           34
#define APPLY_RESIST_DRAIN           35

#define APPLY_SKILL_PICK             100
#define APPLY_SKILL_BALANCE          101
#define APPLY_SKILL_SIXTH_SENSE      102
#define APPLY_SKILL_BASH             103
#define APPLY_SKILL_BACKSTAB         104




/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8



/*
 * Area flags.
 */
#define AREA_CRITICAL                 1   /* mud MUST have this accessible */
#define AREA_NEVER                    2   /* never accessible to inwar morts */
#define AREA_DISABLED                 4   /* internal.  set by game. */
#define AREA_NO_START                 8   /* can't start in this zone */
#define AREA_NOSHOW                  16   /* doesnt appear on areas command */



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_MAP_ROOM         13000
#define ROOM_VNUM_TEMPLE	   13067
#define ROOM_VNUM_TEMPLE_OF_YS	   13011
#define ROOM_VNUM_ALTAR		   13013



/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		      1
#define ROOM_NO_MOB		      4
#define ROOM_INDOORS		      8
#define ROOM_UNUSED1		    512
#define ROOM_SAFE		   1024
#define ROOM_UNUSED2		   2048

#define ROOM_NO_RECALL		   8192
#define ROOM_NO_START		  16384


/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      1
#define EX_CLOSED		      2
#define EX_LOCKED		      4
#define EX_PICKPROOF		     32



/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNUSED		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11



/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define MAX_WEAR		     18



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define LIQUID_MANA                   0
#define LIQUID_HITS                   1



/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5

#define POS_FIGHTING		      7
#define POS_STANDING		      8



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		      1		/* Don't EVER set.	*/
#define PLR_RANDOM_CLASS	      2 /* 0x00000002 */
#define PLR_NOPLAY                    4
#define PLR_AUTOEXIT		      8 /* 0x00000004 */
#define PLR_AUTOLOOT		     16 /* 0x00000008 */
#define PLR_AUTOSAC                  32 /* 0x00000010 */
#define PLR_BLANK		     64 /* 0x00000020 */
#define PLR_BRIEF		    128 /* 0x00000040 */
#define PLR_CMD_SUCCESS		    256
#define PLR_COMBINE		    512 /* 0x00000080 */
#define PLR_PROMPT		   1024 /* 0x00000100 */
#define PLR_TELNET_GA		   2048 /* 0x00000200 */
#define PLR_HOLYLIGHT		   4096 /* 0x00000400 */
#define PLR_WIZINVIS		   8192 /* 0x00000800 */
#define PLR_RATING_OVERRIDE       16384
#define	PLR_SILENCE		  32768 /* 0x00001000 */
#define PLR_NO_EMOTE		  65536 /* 0x00002000 */
#define PLR_NO_TELL		 262144 /* 0x00004000 */
#define PLR_LOG			 524288 /* 0x00008000 */
#define PLR_DENY		1048576 /* 0x00010000 */
#define PLR_FREEZE		2097152 /* 0x00020000 */
#define PLR_CAN_TELEPORT        4194304 /* 0x00040000 */

#define PLR_COLOUR             33554432 /* 0x00200000 */
#define PLR_QUIETMODE          67108864 /* 0x00400000 */

#define PLR_NOTIFY	      268435456 /* 0x01000000 */
#define PLR_TRANS	      536870912 /* 0x02000000 */



/*
 * Channel bits.
 */
#define	CHANNEL_AUCTION		      1
#define	CHANNEL_CHAT		      2
#define	CHANNEL_SUBWAR		      4
#define	CHANNEL_IMMTALK		      8
#define	CHANNEL_MUSIC		     16
#define	CHANNEL_QUESTION	     32
#define	CHANNEL_SHOUT		     64
#define	CHANNEL_YELL		    128
#define CHANNEL_INFO                256
#define CHANNEL_DEATH		    512
#define CHANNEL_WARMESSAGE	   1024

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    SPEC_FUN *		spec_fun;
    INTERACTIVE *	interactive;
    SHOP_DATA *		pShop;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    sh_int		vnum;
    sh_int		count;
    sh_int		sex;
    sh_int		level;
    int			act;
    int			affected_by;
    sh_int		alignment;
    sh_int		hitroll;		/* Unused */
    sh_int		ac;			/* Unused */
    sh_int		hitnodice;		/* Unused */
    sh_int		hitsizedice;		/* Unused */
    sh_int		hitplus;		/* Unused */
    sh_int		damnodice;		/* Unused */
    sh_int		damsizedice;		/* Unused */
    sh_int		damplus;		/* Unused */
    int			gold;			/* Unused */
    int			mark;			/* for searching resets */
};



/*
 * One character (PC or NPC).
 */
struct	char_data
{
    CHAR_DATA *		next;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		master;
    CHAR_DATA *		leader;
    CHAR_DATA *		fighting;
    CHAR_DATA *		reply;
    CHAR_DATA *		hunting;
    CHAR_DATA *		private_war;
    SPEC_FUN *		spec_fun;
    INTERACTIVE *	interactive;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    PC_DATA *		pcdata;
    RESET_DATA         *pReset;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    unsigned int        phase;
    int                 sex;
    int                 char_class;
    int                 race;
    int                 level;
    int                 trust;
    int			played;
    time_t		logon;
    time_t		save_time;
    int                 timer;
    int                 wait;
    int                 hit;
    int                 max_hit;
    int                 hit_regen_bonus;
    int                 mana;
    int                 max_mana;
    int                 mana_regen_bonus;
    int			move_lag;
    int			gold;
    int			exp;
    int			act;
    int			affected_by;
    int                 position;
    int                 practice;
    int                 carry_weight;
    int                 carry_number;
    int                 saving_throw;
    int                 alignment;
    int                 hitroll;
    int                 damroll;
    int                 nojoin;
    int                 armor;
    int                 deaf;
    int			data[4];   /* used by spec procs */
    int			resist_fire;
    int			resist_cold;
    int			resist_lightning;
    int			resist_physical;
    int			resist_magic;
    int			resist_drain;
    int			eq_points;

    struct struct_social {

	char *name;
	char *roomnv;
	char *charnv;
	char *room;
	char *charc;
	char *vict;
    } social;

};



/*
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA *		next;
    char *		pwd;
    char *		bamfin;
    char *		bamfout;
    char *		title;
    char *		createsite;
    char *		site;
    char *		tmc_name;    /* mud connector account name */
    int			challenge_flags;
    sh_int		perm_str;
    sh_int		perm_int;
    sh_int		perm_wis;
    sh_int		perm_dex;
    sh_int		perm_con;
    sh_int		mod_str;
    sh_int		mod_int;
    sh_int		mod_wis;
    sh_int		mod_dex;
    sh_int		mod_con;
    int			kills;
    int			deaths;
    int			wins;
    int			total_victim_kds;
    int			doomsdays;
    int			ld_at_end;
    int			mob_deaths;
    sh_int		learned		[MAX_SKILL];
    sh_int		war_num;
    int			class_kills	[MAX_CLASS];
    int			class_deaths	[MAX_CLASS];
    int			last_war;
    int			snoop_war;
    int			damage_inflicted;
    int			damage_received;
    int			potential_dr;
    sh_int		wizinvis_level;
    sh_int		failed_combat;
    int			rating;
    int			ratings_victims;
    int			ratings_killers;
    int			rated_kills;
    int			rated_deaths;
    int			blicks;
    int			blicked;
    int			color_gossip;
    int			color_warmessage;
    int			color_prompt;
    int			color_exits;
    int			color_room_name;
    int			color_tell;
    int			color_say;
};


/*
 * Mob stats, by Rip
 */
/* mob lev max must be max level mob PLUS ONE */
#define MOB_LEV_MAX	103

struct mob_stats
{
	sh_int	hit_die;
	sh_int	hit_constant;
	sh_int	thac0;
	sh_int	armor;
/* changed to 1dlevel + level damage */
/*
	sh_int	dam_dice;
	sh_int	dam_size;
	sh_int	dam_bonus;
*/
};




/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_BLOOD       13
#define LIQ_KOOL_AID    24
#define LIQ_MAX		25

struct	liq_type
{
    char *	liq_name;
    char *	liq_color;
    sh_int	liq_affect[3];
};


struct affect_loc_info
{
  char *	loc_name;
  int		loc_bit;
};

struct item_type_info
{
  char *	name;
  int		ident;
};


struct wear_loc_names
{
  char		loc_name[11];
  int		loc_bit;
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	/* Next in list                     */
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    WEAR_FUN *		wear_fun;
    OBJECT_FUN *	special;
    char *		name;
    char *		short_descr;
    char *		description;
    sh_int		vnum;
    sh_int		item_type;
    int			extra_flags;
    sh_int		wear_flags;
    sh_int		count;
    sh_int		weight;
    int			cost;			/* Unused */
    int			load_percent;
    int			value	[4];
    int			mark;			/* for searching resets */
};



/*
 * One object.
 */
struct	obj_data
{
    OBJ_DATA *		next;
    OBJ_DATA *		next_content;
    OBJ_DATA *		contains;
    OBJ_DATA *		in_obj;
    CHAR_DATA *		carried_by;
    CHAR_DATA *		killer;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    WEAR_FUN *		wear_fun;
    OBJECT_FUN *	special;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    RESET_DATA         *pReset;
    char *		name;
    char *		short_descr;
    char *		description;
    int                 item_type;
    int			extra_flags;
    int                 wear_flags;
    int                 wear_loc;
    int                 weight;
    int			cost;
    int                 timer;
    int			value	[4];
    unsigned int        phase;
};



/*
 * Exit data.
 */
struct	exit_data
{
    ROOM_INDEX_DATA *	to_room;
    int                 vnum;
    int                 exit_info[NUM_PHASES];
    int                 key;
    char *		keyword;
    char *		description;
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct	reset_data
{
  RESET_DATA   *next;
  char          command;
  int           arg1;
  int           arg2;
  int           arg3;
  void         *instance[NUM_PHASES];
};



/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA *		next;
    RESET_DATA *	reset_first;
    RESET_DATA *	reset_last;
    char *		name;
    char *		author;
    int                 lifespan[NUM_PHASES];
    int                 nplayer;
    int			area_flags;
    int			access_percent;
    int			mark;
};



/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[6];
    ROOM_SPECIAL *	special;
    char *		name;
    char *		description;
    sh_int		vnum;
    int			room_flags;
    sh_int		light;
    int			smoke;
    sh_int		sector_type;
    sh_int		last_exit_dir;
    char *		last_char;
    int			mark;
    ROOM_INDEX_DATA    *bfs_next;
    int                 bfs_identifier;
    int                 bfs_direction;
    int                 bfs_depth;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000



/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4
#define TAR_CHAR_HINDRANCE          5



/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			/* Name of skill		*/
    SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
    sh_int	target;			/* Legal targets		*/
    sh_int	minimum_position;	/* Position for caster / user	*/
    sh_int *	pgsn;			/* Pointer to associated gsn	*/
    sh_int	slot;			/* Slot for #OBJECT loading	*/
    sh_int	min_mana;		/* Minimum mana used		*/
    sh_int	beats;			/* Waiting time after use	*/
    char *	noun_damage;		/* Damage message		*/
    char *	msg_off;		/* Wear off message		*/
    char *	spell_color;
    char *	arrow_color;
    char *	msg_death_char;
    char *	msg_death_targ;
    char *	msg_death_room;
    char *	msg_dam_char;
    char *	msg_dam_targ;
    char *	msg_dam_room;
    char *	msg_miss_char;
    char *	msg_miss_targ;
    char *	msg_miss_room;
};



/*
 * These are skill_lookup return values for common skills and spells.
 */
/* this shouldn't go here but i jsut searched for extern, shrug */

extern  sh_int  gsn_acid_breath;
extern  sh_int  gsn_archery;
extern  sh_int  gsn_armor;
extern  sh_int  gsn_backstab;
extern  sh_int  gsn_balance;
extern  sh_int  gsn_bash;
extern  sh_int  gsn_become_rambo;
extern  sh_int  gsn_blackmantle;
extern  sh_int  gsn_bless;
extern  sh_int  gsn_blindness;
extern  sh_int  gsn_brilliance;
extern  sh_int  gsn_burning_hands;
extern  sh_int  gsn_call_lightning;
extern  sh_int  gsn_caltrops;
extern  sh_int  gsn_cause_critical;
extern  sh_int  gsn_cause_light;
extern  sh_int  gsn_cause_serious;
extern  sh_int  gsn_change_sex;
extern  sh_int  gsn_charm_person;
extern  sh_int  gsn_chill_touch;
extern  sh_int  gsn_colour_spray;
extern  sh_int  gsn_confusion;
extern  sh_int  gsn_continual_light;
extern  sh_int  gsn_control_weather;
extern  sh_int  gsn_create_food;
extern  sh_int  gsn_create_spring;
extern  sh_int  gsn_create_water;
extern  sh_int  gsn_cure_blindness;
extern  sh_int  gsn_cure_critical;
extern  sh_int  gsn_cure_light;
extern  sh_int  gsn_cure_poison;
extern  sh_int  gsn_cure_serious;
extern  sh_int  gsn_curse;
extern  sh_int  gsn_dancing_sword;
extern  sh_int  gsn_death_spray;
extern  sh_int  gsn_detect_evil;
extern  sh_int  gsn_detect_hidden;
extern  sh_int  gsn_detect_invis;
extern  sh_int  gsn_detect_magic;
extern  sh_int  gsn_detect_poison;
extern  sh_int  gsn_disarm;
extern  sh_int  gsn_disarm_traps;
extern  sh_int  gsn_dispel_evil;
extern  sh_int  gsn_dispel_good;
extern  sh_int  gsn_dispel_magic;
extern  sh_int  gsn_dodge;
extern  sh_int  gsn_dragon_power;
extern  sh_int  gsn_earthquake;
extern  sh_int  gsn_enchant_weapon;
extern  sh_int  gsn_energy_drain;
extern  sh_int  gsn_enhanced_damage;
extern  sh_int  gsn_entangle;
extern  sh_int  gsn_explosion;
extern  sh_int  gsn_faerie_fire;
extern  sh_int  gsn_faerie_fog;
extern  sh_int  gsn_farsee;
extern  sh_int  gsn_find_traps;
extern  sh_int  gsn_fire_breath;
extern  sh_int  gsn_fireball;
extern  sh_int  gsn_fireshield;
extern  sh_int  gsn_flamestrike;
extern  sh_int  gsn_fly;
extern  sh_int  gsn_free_wheres;
extern  sh_int  gsn_frost_breath;
extern  sh_int  gsn_gate;
extern  sh_int  gsn_gas_breath;
extern  sh_int  gsn_harm;
extern  sh_int  gsn_heal;
extern  sh_int  gsn_follow;
extern  sh_int  gsn_hide;
extern  sh_int  gsn_ice_storm;
extern  sh_int  gsn_illusory_appearance;
extern  sh_int  gsn_illusory_auras;
extern  sh_int  gsn_improved_invis;
extern  sh_int  gsn_infravision;
extern  sh_int  gsn_invis;
extern  sh_int  gsn_invulnerability;
extern  sh_int  gsn_ki_release;
extern  sh_int  gsn_kick;
extern  sh_int  gsn_know_aligment;
extern  sh_int  gsn_lightning_bolt;
extern  sh_int  gsn_lightning_breath;
extern  sh_int  gsn_locate_object;
extern  sh_int  gsn_magic_missile;
extern  sh_int  gsn_mass_invis;
extern  sh_int  gsn_mesmerize;
extern  sh_int  gsn_miracle;
extern  sh_int  gsn_no_summon;
extern  sh_int  gsn_parry;
extern  sh_int  gsn_pass_door;
extern  sh_int  gsn_peek;
extern  sh_int  gsn_phantasmal_weaponry;
extern  sh_int  gsn_pick_lock;
extern  sh_int  gsn_poison;
extern  sh_int  gsn_protection;
extern  sh_int  gsn_psionic_blast;
extern  sh_int  gsn_ego_whip;
extern  sh_int  gsn_read_magic;
extern  sh_int  gsn_reflect;
extern  sh_int  gsn_remove_curse;
extern  sh_int  gsn_rescue;
extern  sh_int  gsn_sanct;
extern  sh_int  gsn_smoke_bomb;
extern  sh_int  gsn_second_attack;
extern  sh_int  gsn_set_traps;
extern  sh_int  gsn_shield;
extern  sh_int  gsn_shocking_grasp;
extern  sh_int  gsn_silence;
extern  sh_int  gsn_sixth_sense;
extern  sh_int  gsn_sleep;
extern  sh_int  gsn_slow;
extern  sh_int  gsn_snare;
extern  sh_int  gsn_sneak;
extern  sh_int  gsn_steal;
extern  sh_int  gsn_stone_skin;
extern  sh_int  gsn_strength;
extern  sh_int  gsn_summon;
extern  sh_int  gsn_swim;
extern  sh_int  gsn_teleport;
extern  sh_int  gsn_third_attack;
extern  sh_int  gsn_tower_id;
extern  sh_int  gsn_track;
extern  sh_int  gsn_use_staffs;
extern  sh_int  gsn_use_wands;
extern  sh_int  gsn_vampiric_touch;
extern  sh_int  gsn_ventriloquate;
extern  sh_int  gsn_void_aura;
extern  sh_int  gsn_weaken;
extern  sh_int  gsn_weapon_immunity;
extern  sh_int  gsn_word_of_recall;



/*
 * Utility macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))



/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define GET_NAME(ch)		(IS_NPC(ch) ? (ch)->short_descr : (ch)->pcdata->wizinvis_level == get_trust(ch) ? "someone" : (ch)->name )
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_GOD(ch)		(get_trust(ch) >= 39)
#define IS_IMP(ch)		(get_trust(ch) == MAX_LEVEL) 
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch)		((ch)->armor				    \
				    + ( IS_AWAKE(ch)			    \
				    ? dex_app[get_curr_dex(ch)].defensive   \
				    : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+str_app[get_curr_str(ch)].tohit)
#define GET_DAMROLL(ch)		((ch)->damroll+str_app[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (IS_IMP(ch) ? (0) : npulse)))


/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))



/*
 * Description macros.
 */
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (ch)->name ) : "someone" )



/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
    sh_int		position;
    sh_int		level;
    sh_int		log;
};


struct  social_olc {

	char	*name;
	char	*charc;
	char	*roomnv;
	char	*charnv;
	char	*room;
	char	*vict;
};

/*
 * Structure for a social in the socials table.
 */
struct	social_type
{
    char * const	name;
    char * const	char_no_arg;
    char * const	others_no_arg;
    char * const	char_found;
    char * const	others_found;
    char * const	vict_found;
    char * const	char_auto;
    char * const	others_auto;
};



/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[26];
extern	const	struct	int_app_type	int_app		[26];
extern	const	struct	wis_app_type	wis_app		[26];
extern	const	struct	dex_app_type	dex_app		[26];
extern	const	struct	con_app_type	con_app		[26];

/* extern	struct	class_info	class_table	[MAX_CLASS]; */
extern	const	struct	cmd_type	cmd_table	[];
extern	const	struct	liq_type	liq_table	[LIQ_MAX];
extern	const	struct	skill_type	skill_table	[MAX_SKILL];
extern	const	struct	social_type	social_table	[];
/* Mob stat constants for simple mobs */
extern	const	struct	mob_stats	stat_table	[MOB_LEV_MAX];
extern	struct	wear_loc_names	wear_loc_table	[];
extern	const struct	affect_loc_info	affect_loc_table	[];
extern	const struct	item_type_info	item_type_table	[];



/*
 * Global variables.
 */
extern		HELP_DATA	  *	help_first;
extern		SHOP_DATA	  *	shop_first;

extern		BAN_DATA	  *	ban_list;
extern		BAN_DATA	  *	no_note_list;
extern		BAN_DATA	  *	restrict_list;
extern		BAN_DATA	  *	no_rating_list;
extern		CHAR_DATA	  *	char_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		NOTE_DATA	  *	note_list;
extern		OBJ_DATA	  *	object_list;

extern		AFFECT_DATA	  *	affect_free;
extern		BAN_DATA	  *	ban_free;
extern		CHAR_DATA	  *	char_free;
extern		DESCRIPTOR_DATA	  *	descriptor_free;
extern		EXTRA_DESCR_DATA  *	extra_descr_free;
extern		NOTE_DATA	  *	note_free;
extern		OBJ_DATA	  *	obj_free;
extern		PC_DATA		  *	pcdata_free;

extern		char			bug_buf		[];
extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		char			log_buf		[];
extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;

extern struct social_olc socials[200];





/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN(	do_armor	);
DECLARE_DO_FUN( do_arrow	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN(	do_autoexit	);
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN(	do_bash		);
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN(	do_brief	);
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN( do_caltrops	);
DECLARE_DO_FUN(	do_cdisable	);
DECLARE_DO_FUN(	do_cenable	);
DECLARE_DO_FUN( do_changeclass	);
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN(	do_cmod		);
DECLARE_DO_FUN( do_coinflip	);
DECLARE_DO_FUN( do_color	);
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN(	do_compact	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_config	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN(	do_cskill	);
DECLARE_DO_FUN(	do_cview	);
DECLARE_DO_FUN(	do_cwrite	);
DECLARE_DO_FUN( do_death	);
DECLARE_DO_FUN( do_delay	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN( do_disarmtrap	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN( do_finger	);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_high		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN(	do_identify	);
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN(	do_info	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_joinffa	);
DECLARE_DO_FUN( do_join_private	);
DECLARE_DO_FUN(	do_joinwar	);
DECLARE_DO_FUN(	do_junk		);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN(	do_last		);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN(	do_motd		);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN(	do_music	);
DECLARE_DO_FUN(	do_nmodify	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN( do_noplay	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN( do_notify	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN(	do_osearch	);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN(	do_phase	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN( do_potion	);
DECLARE_DO_FUN( do_pull		);
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN( do_quietmode	);
DECLARE_DO_FUN(	do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN(	do_randomclass	);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN( do_reset	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN( do_rlog		);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN( do_scatter	);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN( do_set		);
DECLARE_DO_FUN(	do_shoot	);
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN( do_sic_mob	);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN(	do_snare	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_soul		);
DECLARE_DO_FUN( do_soulwrite	);
DECLARE_DO_FUN( do_specho	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN( do_start	);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_subwar	);
DECLARE_DO_FUN( do_tele		);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN( do_trap		);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN( do_use		);
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN( do_vostat	);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN( do_warflags	);
DECLARE_DO_FUN( do_warmessage	);
DECLARE_DO_FUN( do_warstart     );
DECLARE_DO_FUN( do_warstop      );
DECLARE_DO_FUN( do_wartime	);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_whereis	);
DECLARE_DO_FUN(	do_whereload	);
DECLARE_DO_FUN( do_whichway	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN(	do_wield	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN(	do_wizlist	);
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);



/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_banish_item	);
DECLARE_SPELL_FUN(	spell_become_rambo	);
DECLARE_SPELL_FUN(	spell_blackmantle	);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_brilliance	);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(	spell_confusion		);
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_illusory_blood );
DECLARE_SPELL_FUN(	spell_create_spring	);
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_curse		);
DECLARE_SPELL_FUN(	spell_dancing_sword	);
DECLARE_SPELL_FUN(	spell_death_spray	);
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_good	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_dragon_power	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_entangle		);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_farsee		);
DECLARE_SPELL_FUN(	spell_feeblemind	);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_fireshield	);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_general_purpose	);
DECLARE_SPELL_FUN(	spell_smoke_bomb	);
DECLARE_SPELL_FUN(	spell_steal_enchantment	);
DECLARE_SPELL_FUN(	spell_strength		);
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(	spell_ice_storm		);
DECLARE_SPELL_FUN(	spell_illusory_appearance );
DECLARE_SPELL_FUN(	spell_illusory_auras	);
DECLARE_SPELL_FUN(	spell_improved_invis	);
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_invulnerability	);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_ki_release	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(	spell_mesmerize		);
DECLARE_SPELL_FUN(	spell_miracle		);
DECLARE_SPELL_FUN(	spell_no_summon		);
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(	spell_personal_illusion	);
DECLARE_SPELL_FUN(	spell_phantasmal_weaponry );
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_protection	);
DECLARE_SPELL_FUN(	spell_psionic_blast	);
DECLARE_SPELL_FUN(	spell_ego_whip		);
DECLARE_SPELL_FUN(	spell_reflect		);
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_silence		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(	spell_slow		);
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	spell_tower_of_id	);
DECLARE_SPELL_FUN(	spell_vampiric_touch	);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_void_aura		);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_weapon_immunity	);
DECLARE_SPELL_FUN(	spell_whirling_wind	);
DECLARE_SPELL_FUN(	spell_wizard_eye	);
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);



/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined(_AIX)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(apollo)
int	atoi		args( ( const char *string ) );
void *	calloc		args( ( unsigned nelem, size_t size ) );
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(hpux)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(linux)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(macintosh)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(MIPS_OS)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(MSDOS)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(NeXT)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(sequent)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(sun)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
#if 	defined(SYSV)
size_t 	fread		args( ( void *ptr, size_t size, size_t n, 
				FILE *stream ) );
#else
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(ultrix)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR	""		/* Player files			*/
#define NULL_FILE	"proto.are"	/* To reserve one stream	*/
#endif

#if defined(MSDOS)
#define PLAYER_DIR	""		/* Player files                 */
#define NULL_FILE	"nul"		/* To reserve one stream	*/
#endif

#if defined(unix)
#define PLAYER_DIR	"../PKplr/"	/* Player files			*/
#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
#endif

#define AREA_LIST	"PK.area.lst"	/* List of areas		*/

#define BUG_FILE	"bugs.txt"      /* For 'bug' and bug( )		*/
#define IDEA_FILE	"ideas.txt"	/* For 'idea'			*/
#define TYPO_FILE	"typos.txt"     /* For 'typo'			*/
#define NOTE_FILE	"notes.txt"	/* For 'notes'			*/
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'		*/
#define REBOOT_FILE	"reboot.txt"	/* For 'reboot'			*/



/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN

/* act_comm.c */
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
bool	can_communicate	args( ( CHAR_DATA *ch, bool fEcho ) );
void	die_follower	args( ( CHAR_DATA *ch ) );
void	get_start_eq	args( ( CHAR_DATA *ch ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
bool	will_die	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
bool	check_no_rating_list args( ( CHAR_DATA *ch, bool fMessage ) );

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );

/* act_move.c */
int	get_direction	args( ( char *argument ) );
void	move_char	args( ( CHAR_DATA *ch, int door, bool fForce ) );
void	teleport_char	args( ( CHAR_DATA *ch, bool fWar ) );
bool can_traverse_water( CHAR_DATA *ch );
bool can_fly( CHAR_DATA *ch );
bool    is_door_closed( EXIT_DATA *pexit, CHAR_DATA *ch );
bool    is_door_locked( EXIT_DATA *pexit, CHAR_DATA *ch );
bool    is_door( EXIT_DATA *pexit, CHAR_DATA *ch );
void    open_door( EXIT_DATA *pexit, unsigned int door, CHAR_DATA *ch );
void    close_door( EXIT_DATA *pexit, unsigned int door, CHAR_DATA *ch );
void    lock_door( EXIT_DATA *pexit, unsigned int door, CHAR_DATA *ch );
void    unlock_door( EXIT_DATA *pexit, unsigned int door, CHAR_DATA *ch );


/* act_obj.c */
void	attempt_steal_object args( ( CHAR_DATA *ch, CHAR_DATA *victim,
				OBJ_DATA *obj ) );
void	use_staff	args( ( CHAR_DATA *ch, OBJ_DATA *device,
				char *argument ) );
void	use_wand	args( ( CHAR_DATA *ch, OBJ_DATA *device,
				char *argument ) );

/* act_wiz.c */
void	remove_snoop	args( ( DESCRIPTOR_DATA *d, struct snoop_data *snoop ));
void restore_character( CHAR_DATA *ch );
void    execute_echo( const char *argument );


/* archer.c */
void get_new_target_room( CHAR_DATA *ch );

/* cmod.c */
int	class_lookup	args( ( char *argument ) );
int	parse_class	args( ( char *argument ) );

/* color.c */
char *	process_color	args( ( int desc, char *string, int length ) );

/* comm.c */
void	close_socket	args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	args( ( DESCRIPTOR_DATA *d, const char *txt,
		    int length ) );
void	write_to_buffer2 args(( DESCRIPTOR_DATA *d, const char *txt, 
		    int length ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	act		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type,
			    bool fAlways ) );

/* db.c */
void *	alloc_mem	args( ( int sMem ) );
void *	alloc_perm	args( ( int sMem ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	area_update	args( ( void ) );
void	boot_db		args( ( void ) );
void	bug		args( ( const char *str, int param ) );
char *	capitalize	args( ( const char *str ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex, unsigned int phase ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex, unsigned int phase ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
int	dice		args( ( int number, int size ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
char *	fread_string	args( ( FILE *fp ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp ) );
void	free_char	args( ( CHAR_DATA *ch ) );
void	free_mem	args( ( void *pMem, int sMem ) );
void	free_snoop	args( ( struct snoop_data *snoop ) );
void	free_string	args( ( char *pstr ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
int	interpolate	args( ( int level, int value_00, int value_32 ) );
void	log_string	args( ( const char *str ) );
void	mudlog		args( ( const char *str, int level ) );
struct snoop_data *new_snoop_data args( ( void ) );
int	number_fuzzy	args( ( int number ) );
int	number_range	args( ( int from, int to ) );
int	number_percent	args( ( void ) );
int	number_door	args( ( void ) );
int	number_bits	args( ( int width ) );
int	number_mm	args( ( void ) );
void    set_join_status	args( ( CHAR_DATA *ch ) );
void	smash_tilde	args( ( char *str ) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
char *	str_dup		args( ( const char *str ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
void	tail_chain	args( ( void ) );

/* fight.c */
void	check_failed_combat args( ( CHAR_DATA *ch ) );
bool	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt, int resist ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	violence_update	args( ( void ) );

/* grenade.c */
void	explode_grenade	args( ( OBJ_DATA *obj ) );

/* handler.c */
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_age		args( ( CHAR_DATA *ch ) );
int	get_curr_str	args( ( CHAR_DATA *ch ) );
int	get_curr_int	args( ( CHAR_DATA *ch ) );
int	get_curr_wis	args( ( CHAR_DATA *ch ) );
int	get_curr_dex	args( ( CHAR_DATA *ch ) );
int	get_curr_con	args( ( CHAR_DATA *ch ) );
int	get_equip_points args( ( OBJ_DATA *obj ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
char	*anseval	args( ( const char *txt, CHAR_DATA *to ) );
bool	is_name		args( ( const char *str, char *namelist ) );
bool	is_name_prefix	args( ( const char *str, char *namelist ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
AFFECT_DATA *is_affected	args( ( CHAR_DATA *ch, int sn ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	char_to_war	args( ( CHAR_DATA *ch ) );
void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear ) );
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	obj_from_room	args( ( OBJ_DATA *obj, char *caller ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
OD *	create_money	args( ( int amount, unsigned int phase ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
void	extract_obj	args( ( OBJ_DATA *obj, char *caller ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_phase	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			    OBJ_DATA *list ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
void	heal_character	args( ( CHAR_DATA *ch, int amount ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	item_type_name	args( ( int item_type, int vnum ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
char *	extra_bit_name	args( ( int extra_flags ) );
void	phase_char	args( ( CHAR_DATA *ch, unsigned int phase ) );

/* high.c */
void	adjust_high_entry args( ( struct high_score_element **h,
				CHAR_DATA *ch, int value ) );
void	init_high_scores args( ( void ) );
void	adjust_high_ratings args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );


/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );

/* magic.c */
void	identify_item	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	reduce_smoke	args( ( CHAR_DATA *ch, int amount ) );
int	skill_lookup	args( ( const char *name ) );
int	slot_lookup	args( ( int slot ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
				    CHAR_DATA *victim, OBJ_DATA *obj ) );

/* pkmud.c */
bool	can_interact    args( ( CHAR_DATA *ch ) );
bool	in_a_war	args( ( CHAR_DATA *ch ) );
bool	in_main_war	args( ( CHAR_DATA *ch ) );
bool	in_ffa_war	args( ( CHAR_DATA *ch ) );
bool	in_private_war	args( ( CHAR_DATA *ch ) );
bool	in_same_war	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	in_static_war	args( ( CHAR_DATA *ch ) );
bool	mortal_in_main_war  args( ( CHAR_DATA *ch ) );
int	count_waiting_players args( ( void ) );

/* random.c */
void	check_random_equip args( ( CHAR_DATA *ch ) );
void	make_random_quiver args( ( OBJ_DATA *obj ) );
void	make_random_trap	args( ( OBJ_DATA *obj ) );
void	rename_object_random args( ( OBJ_DATA *obj , int table ) );
OBJ_DATA *create_random_potion( unsigned int phase );

/* save.c */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );

/* shoot.c */
CD *	search_dir_name	args( ( CHAR_DATA *ch, char *arg, int dir, int range ) );

/* skills.c */
void assign_zombie( CHAR_DATA *victim );

/* special.c */
SF *	spec_lookup	args( ( const char *name ) );
INTERACTIVE * interactive_lookup args( ( const char *name ) );

/* wear_fun.c */
WEAR_FUN * wear_fun_lookup	args( ( const char *name ) );

/* room_fun.c */
ROOM_SPECIAL * room_fun_lookup	args( ( const char *name ) );

/* obj_fun.c */
OBJECT_FUN * object_fun_lookup	args( ( const char *name ) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch ) );
void	end_war		args( ( void ) );
void	remove_from_war	args( ( CHAR_DATA *ch ) );
void	reset_mud	args( ( unsigned int phase ) );
void	set_war_status	args( ( int status ) );
void	update_handler	args( ( void ) );

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF

#endif /* __MERC_H_ */
