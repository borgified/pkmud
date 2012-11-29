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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"



const struct affect_loc_info  affect_loc_table   [] =
{
  { "strength", APPLY_STR },
  { "dexterity", APPLY_DEX },
  { "intelligence", APPLY_INT },
  { "wisdom", APPLY_WIS },
  { "constitution", APPLY_CON },
  { "mana", APPLY_MANA },
  { "hits", APPLY_HIT },
  { "hps", APPLY_HIT },
  { "ac", APPLY_AC },
  { "armor", APPLY_AC },
  { "hitroll", APPLY_HITROLL },
  { "hit roll", APPLY_HITROLL },
  { "damroll", APPLY_DAMROLL },
  { "damage roll", APPLY_DAMROLL },
  { "save vs paralysis", APPLY_SAVING_PARA },
  { "save vs rod", APPLY_SAVING_ROD },
  { "save vs petrificiation", APPLY_SAVING_PETRI },
  { "save vs breath", APPLY_SAVING_BREATH },
  { "save vs spell", APPLY_SAVING_SPELL },
  { "save vs magic", APPLY_SAVING_SPELL },
  { "hit regen", APPLY_REGEN_HIT },
  { "mana regen", APPLY_REGEN_MANA },
  { "skill pick", APPLY_SKILL_PICK },
  { "skill balance", APPLY_SKILL_BALANCE },
  { "skill sixth sense", APPLY_SKILL_SIXTH_SENSE },
  { "skill bash", APPLY_SKILL_BASH },
  { "skill backstab", APPLY_SKILL_BACKSTAB },
  { "resist fire", APPLY_RESIST_FIRE },
  { "resist cold", APPLY_RESIST_COLD },
  { "resist lightning", APPLY_RESIST_LIGHTNING },
  { "resist magic", APPLY_RESIST_MAGIC },
  { "resist physical", APPLY_RESIST_PHYSICAL },
  { "resist drain", APPLY_RESIST_DRAIN },
  { "last", 0 }
};


const struct item_type_info  item_type_table   [] =
{
  { "light", ITEM_LIGHT },
  { "scroll", ITEM_SCROLL },
  { "wand", ITEM_WAND },
  { "staff", ITEM_STAFF },
  { "weapon", ITEM_WEAPON },
  { "treasure", ITEM_TREASURE },
  { "armor", ITEM_ARMOR },
  { "potion", ITEM_POTION },
  { "furniture", ITEM_FURNITURE },
  { "trash", ITEM_TRASH },
  { "container", ITEM_CONTAINER },
  { "drink container", ITEM_DRINK_CON },
  { "key", ITEM_KEY },
  { "food", ITEM_FOOD },
  { "money", ITEM_MONEY },
  { "boat", ITEM_BOAT },
  { "corpse npc", ITEM_CORPSE_NPC },
  { "corpse pc", ITEM_CORPSE_PC },
  { "fountain", ITEM_FURNITURE },
  { "pill", ITEM_PILL },
  { "jewelry", ITEM_JEWELRY },
  { "quiver", ITEM_QUIVER },
  { "trap", ITEM_TRAP },
  { "last", 0 }
};



/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
/* carrying capacity modified such that - strength can be heinous,
    i am going to add no movement if more carried than allocated.. */
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 105,  9 },
    {  0,  0, 110, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 120, 12 },
    {  0,  0, 130, 13 }, /* 13  */
    {  0,  0, 140, 14 },
    {  0,  0, 150, 15 }, /* 15  */
    {  0,  1, 200, 16 },
    {  1,  1, 250, 22 },
    {  1,  2, 400, 25 }, /* 18  */
    {  1,  3, 425, 30 },
    {  2,  3, 450, 35 }, /* 20 */
    {  2,  4, 475, 40 },
    {  2,  5, 500, 45 },
    {  3,  6, 600, 50 },
    {  4,  7, 800, 55 },
    {  5,  8, 999, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
/* percent per session, rather irrelevant */
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 85 },
    { 99 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
/* irrelevant unless i get a really long term site, i just
      made it so that you get less session... */
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 1 },	/* 10 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 1 },	/* 15 */
    { 1 },
    { 1 },
    { 1 },	/* 18 */
    { 1 },
    { 1 },	/* 20 */
    { 2 },
    { 2 },
    { 3 },
    { 3 },
    { 4 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
/* dex modified such that there is no real affect till lots of +dex */
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 20 },
    { - 30 },
    { - 40 },
    { - 43 },
    { - 47 },   /* 20 */
    { - 50 },
    { - 53 },
    { - 57 },
    { - 60 },
    { - 65 }    /* 25 */
};



const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  5, 99 },
    {  5, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 }    /* 25 */
};


/*
 * mob stats by level
 * ( (d#) (const#) (thac0) (ac) (ndice) (sizedice) (dambonus) )
 */
const	struct	mob_stats	stat_table	[MOB_LEV_MAX]	=
{
	{  10,    0,20, 10 },	/* 0 */
	{  12,   10,20,  9 },	/* 1 */
	{  13,   22,19,  8 },	/* 2 */
	{  12,   35,18,  7 },	/* 3 */
	{  13,   47,17,  6 },	/* 4 */
	{  12,   60,16,  5 },	/* 5 */
	{  13,   72,15,  4 },	/* 6 */
	{  12,   85,14,  4 },	/* 7 */
	{  13,   97,13,  3 },	/* 8 */
	{  12,  110,12,  3 },	/* 9 */
	{  13,  122,11,  2 },	/* 10 */
	{  12,  135,10,  2 },	/* 11 */
	{  13,  147, 9,  2 },	/* 12 */
	{  12,  160, 8,  2 },	/* 13 */
	{  13,  172, 7,  1 },	/* 14 */
	{  13,  185, 6,  1 },	/* 15 */
	{  13,  197, 5,  1 },	/* 16 */
	{  12,  210, 4,  1 },	/* 17 */
	{  13,  221, 3,  0 },	/* 18 */
	{  12,  235, 2,  0 },	/* 19 */
	{  13,  247, 1,  0 },	/* 20 */
	{  90,  260, 1, -1 },	/* 21 */
	{  50,  350, 1, -1 },	/* 22 */
	{  50,  400, 1, -2 },	/* 23 */
	{  50,  450, 1, -3 },	/* 24 */
	{  50,  500, 1, -4 },	/* 25 */
	{  50,  550, 1, -6 },	/* 26 */
	{  50,  600, 1, -7 },	/* 27 */
	{  50,  650, 1, -8 },	/* 28 */
	{ 200,  700, 1, -9 },	/* 29 */
	{ 100,  900, 1,-10 },	/* 30 */
	{ 100, 1000, 1,-11 },	/* 31 */
	{ 100, 1100, 1,-11 },	/* 32 */
	{ 100, 1200, 1,-12 },	/* 33 */
	{ 100, 1300, 1,-12 },	/* 34 */
	{ 150, 1400, 1,-13 },	/* 35 */
	{ 150, 1550, 1,-13 },	/* 36 */
	{ 150, 1700, 1,-14 },	/* 37 */
	{ 200, 1850, 1,-14 },	/* 38 */
	{ 200, 2050, 1,-15 },	/* 39 */
	{ 200, 2250, 1,-15 },	/* 40 */
	{ 250, 2450, 1,-15 },	/* 41 */
	{ 250, 2700, 1,-15 },	/* 42 */
	{ 250, 2950, 1,-15 },	/* 43 */
	{ 300, 3200, 1,-15 },	/* 44 */
	{ 300, 3500, 1,-15 },	/* 45 */
	{ 300, 3800, 1,-15 },	/* 46 */
	{ 350, 4100, 1,-15 },	/* 47 */
	{ 350, 4450, 1,-15 },	/* 48 */
	{ 350, 4800, 1,-15 },	/* 49 */
	{ 400, 5150, 1,-15 },	/* 50 */  /* 73 average damage */
	{ 400, 5500, 1,-15 },	/* 51 */
	{ 450, 5950, 1,-15 },	/* 52 */
	{ 450, 6400, 1,-15 },	/* 53 */
	{ 450, 6850, 1,-15 },	/* 54 */
	{ 500, 7400, 1,-15 },	/* 55 */
	{ 500, 7900, 1,-15 },	/* 56 */
	{ 500, 8400, 1,-15 },	/* 57 */
	{ 550, 8900, 1,-15 },	/* 58 */
	{ 550, 9450, 1,-15 },	/* 59 */
	{ 550,10000, 1,-15 },	/* 60 */
	{ 600,10550, 1,-15 },	/* 61 */
	{ 600,11150, 1,-15 },	/* 62 */
	{ 600,11750, 1,-15 },	/* 63 */
	{ 650,12350, 1,-15 },	/* 64 */
	{ 650,13000, 1,-15 },	/* 65 */
	{ 650,13650, 1,-15 },	/* 66 */
	{ 700,14300, 1,-15 },	/* 67 */
	{ 700,15000, 1,-15 },	/* 68 */
	{ 700,15700, 1,-15 },	/* 69 */
	{ 750,16400, 1,-15 },	/* 70 */
	{ 750,17150, 1,-15 },	/* 71 */
	{ 750,17900, 1,-15 },	/* 72 */
	{ 800,18650, 1,-15 },	/* 73 */
	{ 800,19450, 1,-15 },	/* 74 */
	{ 800,20250, 1,-15 },	/* 75 */
	{ 850,21100, 1,-15 },	/* 76 */
	{ 850,22800, 1,-15 },	/* 77 */
	{ 850,23650, 1,-15 },	/* 78 */
	{ 900,24500, 1,-15 },	/* 79 */
	{ 900,25400, 1,-15 },	/* 80 */
	{ 900,26300, 1,-15 },	/* 81 */
	{ 950,27200, 1,-15 },	/* 82 */
	{ 950,28150, 1,-15 },	/* 83 */
	{ 950,29100, 1,-15 },	/* 84 */
	{1000,30050, 1,-15 },	/* 85 */
	{1000,31050, 1,-15 },	/* 86 */
	{1000,32050, 1,-15 },	/* 87 */
	{1050,33050, 1,-15 },	/* 88 */
	{1050,34100, 1,-15 },	/* 89 */
	{1050,35150, 1,-15 },	/* 90 */
	{1100,36200, 1,-15 },	/* 91 */
	{1100,37300, 1,-15 },	/* 92 */
	{1100,38400, 1,-15 },	/* 93 */
	{1150,39500, 1,-15 },	/* 94 */
	{1150,40650, 1,-15 },	/* 95 */
	{1150,41800, 1,-15 },	/* 96 */
	{1200,42950, 1,-15 },	/* 97 */
	{1200,44150, 1,-15 },	/* 98 */
	{1200,46150, 1,-15 },	/* 99 */
	{1250,47150, 1,-15 },	/*100 */
	{1250,48400, 1,-15 },	/*101 */
	{1250,49650, 1,-15 }	/*102 */
};


/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
    { "water",			"clear",	{ 25,25,  0 }	},  /*  0 */
    { "beer",			"amber",	{  0,10,  0 }	},
    { "nitrous oxide",		"gasseous",	{  0,400, 0 }	},
    { "gasoline",		"red-brown",	{200,200, 1 }	},
    { "liquid nitrogen",	"gasseous",	{-100,250,1 }	},

    { "whisky",			"golden",	{-20,40,  0 }	},  /*  5 */
    { "lemonade",		"pink",		{ 50,25,  0 }	},
    { "firebreather",		"boiling",	{250,-75, 1 }	},
    { "local specialty",	"everclear",	{ 25,40,  0 }	},
    { "slime mold juice",	"green",	{-100,-100, 0 }	},

    { "milk",			"white",	{ 35,100, 0 }	},  /* 10 */
    { "jolt",			"bubbling",	{400, 0,  2 }	},
    { "sour milk",		"white",	{-40,-100,0 }	},
    { "blood",			"red",		{400,400, 4 }	},  /* 13 */
    { "apple juice",		"amber",	{ 10,35,  0 }	},

    { "cocha cola",		"bubbling",	{ 15,15,  0 }	},  /* 15 */
    { "hemlock",		"clear",	{-80,-80, 0 }   },
    { "pehpsi cola",		"bubbling",	{-150,-35,0 }   },
    { "urine",                  "golden",	{-60,-90, 0 }   },
    { "sludge",			"black",	{100,100, 1 }   },

    { "carbon monoxide",	"gasseous",	{  0,-350,0 }   },  /* 20 */
    { "drano",			"clear",	{-300,-300,0 }   },
    { "plasma",			"bright red",	{  0,  0, 1 }   },
    { "liquified spam",		"pink",		{-100,-100,4}   },
    { "kool aid",		"red",		{  0,  0, 0 }   }
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const	struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

    {
	"reserved",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT( 0),	 0,	 0,
	"",			"",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"armor",
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_armor,		SLOT( 1),	 5,	1,
	"",			"You feel less protected.",
	"brown",	"brown",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"banish item",
	spell_banish_item,	TAR_CHAR_HINDRANCE,	POS_STANDING,
	NULL,			SLOT( 322 ),	 50,	12,
	"",			"!Banish Item!",
	"banish item",	"banish item",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"blackmantle",
	spell_blackmantle,	TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_blackmantle,	SLOT( 305),	 50,	8,
	"",			"You stop rotting.",
	"lumpy black",	"black",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"bless",
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_bless,		SLOT( 3),	 5,	2,
	"",			"You feel less righteous.",
	"white",	"white",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"blindness",
	spell_blindness,	TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_blindness,		SLOT( 4),	 5,	3,
	"",			"You feel a cloak of blindness dissolve.",
	"black",	"black",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"brilliance",
	spell_brilliance,	TAR_CHAR_SELF,	POS_STANDING,
	&gsn_brilliance,	SLOT(330),	100,	12,
	"",			"You feel stupid.",
	"silver",	"silver",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"burning hands",
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_burning_hands,	SLOT( 5),	15,	6,
	"flaming hands",	"!Burning Hands!",
	"burning red",	"burning red",
	"You burned $N to death!",
	"You have been burned to death by $n!",
	"$n has burned $N to death!",
	"You burn $N with your hot little hands!",
	"You cry out in pain as $n grabs you with burning hands!",
	"$N cries out as $n burns $M!",
	"You miss $N with your burning hands.",
	"$n reaches for you with his burning hands, but misses luckily.",
	"$n tries to burn $N, but $E escapes."
    },

    {
	"call lightning",
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	&gsn_call_lightning,	SLOT( 6),	50,	12,
	"column of lightning",	"!Call Lightning!",
	"neon blue",	"neon blue",
	"A column of lightning arcs from the sky, vaporizing $N.",
	"A column of lightning arcs from the sky, vaporizing you.",
	"A column of lightning arcs from the sky, vaporizing $N.",
	"A column of lightning arcs from the sky, singing $N's body.",
	"A column of lightning arcs from the sky, singing your body.",
	"A column of lightning arcs from the sky, singing $N's body.",
	"A column of lightning flashes in the sky.",
	"A column of lightning flashes in the sky.",
	"A column of lightning flashes in the sky."
    },

    {
	"cause critical",
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_cause_critical,	SLOT(63),	20,	8,
	"spell",		"!Cause Critical!",
	"blazing pink",	"blazing pink",
	"You deftly touch $N's neck, snapping it with a crackle of energy.",
	"$n deftly touches your neck, snapping it with a crackle of energy.",
	"$n deftly touches $N's neck, snapping it with a crackle of energy.",
	"A crackle of energy from your fingers causes $N's bones to creak.",
	"A crackle of energy from $n's fingers causes your bones to creak.",
	"A crackle of energy from $n's fingers causes $N's bones to creak.",
	"Your fingers crackle with energy, which dissipates quickly.",
	"$n's fingers crackle with energy as $e tries to touch you.",
	"$n's fingers crackle briefly with energy."
    },

    {
	"cause light",
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_cause_light,	SLOT(62),	15,	4,
	"spell",		"!Cause Light!",
	"light pink",	"light pink",
	"You scrape $N's skull, removing the top layer of skin and bone.",
	"$n rips the front of your skull off...",
	"$n scrapes $N's skull, removing the top layer of skin and bone.",
	"You cause minor abrasions to $N's skin.",
	"$n causes minor abrasions to your skin.",
	"$n causes minor abrasions to $N's skin.",
	"$N quickly steps aside as you scrape at $M.",
	"You quickly step aside as $n scrapes at you.",
	"$N quickly steps aside as $n scrapes at $M."
    },

    {
	"cause serious",
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_cause_serious,	SLOT(64),	17,	6,
	"spell",		"!Cause Serious!",
	"hot pink",	"hot pink",
	"A quick touch to $N's body forces all the blood from $S body.",
	"$n quickly pushes against your body, forcing all your blood out.",
	"$n pushes at $N, forcing all the blood from $S body.",
	"You draw blood as your energized hands touch $N.",
	"$n draws blood as $e touches you.",
	"$n draws blood as $e touches $N.",
	"$N dodges your push.",
	"$n pushes as you slowly, and you step aside.",
	"$n pushes at $N, but not quickly enough."
    },

    {
	"change sex",
	spell_change_sex,	TAR_CHAR_HINDRANCE,	POS_FIGHTING,
	&gsn_change_sex,	SLOT(82),	15,	12,
	"",			"Your body feels familiar again.",
	"pink",		"pink",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"charm person",
	spell_charm_person,	TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_charm_person,	SLOT( 7),	 5,	1,
	"",			"You feel more self-confident.",
	"squishy",	"squishy",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"chill touch",
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_chill_touch,	SLOT( 8),	25,	12,
	"freezing hand",	"You feel less cold.",
	"freezing",	"freezing",
	"You chill $N, remember to put flowers on $S grave.",
	"You feel chilled by $n, and then you feel no more... R.I.P.",
	"$n touches $N, who slumps to the ground as a dead lump, rather chilly isn't it.",
	"You chill $N, slowing his life energy.",
	"You feel your life flowing away as $n chills you.",
	"$n chills $N, who suddenly seems less lively.",
	"The atmosphere turns chilly as you miss $N.",
	"You dodge $n and $s chill touch.",
	"$n tries unsuccessfully to chill $N."
    },

    {
	"colour spray",
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_colour_spray,	SLOT(10),	15,	12,
	"rainbow of destruction",	"The rainbow in your eyes dissipates.",
	"rainbow",	"rainbow",
	"Your private rainbow turns $N into a colourful corpse.",
	"Your head is filled with all the colours of the rainbow before it bursts.",
	"$n turns $N into a colourful corpse.",
	"Your rainbow hits $N in the face - WOWIE!",
	"A sudden burst of colours sends you reeling.",
	"$n throws colours at $N who in vain tries to escape.",
	"Your spray of colours displays fireworks, but misses $N",
	"$n's spray of colours displays fireworks, but misses you.",
	"$n's spray of colours displays fireworks, but misses $N."
    },

    {
	"confusion",
	spell_confusion,	TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_confusion,		SLOT( 316 ),	30,	12,
	"confusion",		"You feel far less confused.",
	"backwards",	"backwards",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"continual light",
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	&gsn_continual_light,	SLOT(57),	 7,	2,
	"",			"!Continual Light!",
	"bright",	"bright",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"control weather",
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	&gsn_control_weather,	SLOT(11),	25,	4,
	"",			"!Control Weather!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"create food",
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	&gsn_create_food,	SLOT(12),	 5,	2,
	"",			"!Create Food!",
	"edible",	"edible",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"create illusory blood",
	spell_create_illusory_blood, TAR_IGNORE,	POS_STANDING,
	NULL,			SLOT(336),	 5,	2,
	"",			"!Create Illusory Blood!",
	"red",		"red",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"create spring",
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	&gsn_create_spring,	SLOT(80),	200,	12,
	"",			"!Create Spring!",
	"liquidy",	"liquidy",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"create water",
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	&gsn_create_water,	SLOT(13),	 5,	2,
	"",			"!Create Water!",
	"liquidy",	"liquidy",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"cure blindness",
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_cure_blindness,	SLOT(14),	 5,	2,
	"",			"!Cure Blindness!",
	"burgundy",	"burgundy",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"cure critical",
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_cure_critical,	SLOT(15),	20,	10,
	"",			"!Cure Critical!",
	"orange",	"orange",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"cure light",
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_cure_light,	SLOT(16),	10,	3,
	"",			"!Cure Light!",
	"pink",		"pink",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"cure poison",
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_cure_poison,	SLOT(43),	 5,	2,
	"",			"!Cure Poison!",
	"aquamarine",	"aquamarine",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"cure serious",
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_cure_serious,	SLOT(61),	15,	12,
	"",			"!Cure Serious!",
	"pinkish orange",	"pinkish orange",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"curse",
	spell_curse,		TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_curse,		SLOT(17),	20,	5,
	"curse",		"The curse wears off.",
	"black",	"black",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"dancing sword",
	spell_dancing_sword,	TAR_OBJ_INV,		POS_STANDING,
	&gsn_dancing_sword,	SLOT(328),	50,	20,
	"slash",	"!Dancing Sword!",
	"bug",		"bug",
	"You remove $N's skull from $S body as you slash $M!",
	"Your head is removed from your body as $n slashes it off.",
	"$N's head is removed from $S body as $n slashes it off.",
	"You fly through the air and slash $N.",
	"$n flies through the air and slashes you.",
	"$n flies through the air and slashes $N.",
	"$N ducks under your slash.",
	"You duck under $n's slash.",
	"$N ducks under $n's slash."
    },

    {
	"death spray",
	spell_death_spray,	TAR_IGNORE,		POS_FIGHTING,
	&gsn_death_spray,	SLOT(300),	60,	20,
	"spray of death",	"!Death Spray!",
	"black",	"black",
	"$N turns into a lump of fried meat as your spray strikes $M!",
	"You are cooked well done by $n's burning black spray...",
	"$N turns into a lump of fried meat as $n's spray strikes $M.",
	"$N is blackened as your spray sends $M sprawling.",
	"Your skin is blackened as $n's spray sends you sprawling.",
	"$N is blackened as $n's spray sends $M sprawling.",
	"$N ducks around your spray.",
	"You duck around $n's spray.",
	"$N ducks around $n's spray."
    },

    {
	"detect evil",
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_detect_evil,	SLOT(18),	 5,	2,
	"",			"The red in your vision disappears.",
	"beige",	"beige",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"detect hidden",
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_detect_hidden,	SLOT(44),	 5,	2,
	"",			"You feel less aware of your suroundings.",
	"orange",	"orange",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"detect invis",
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_detect_invis,	SLOT(19),	 5,	2,
	"",			"You no longer see invisible objects.",
	"yellow",	"yellow",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"detect magic",
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_detect_magic,	SLOT(20),	 5,	2,
	"",			"The detect magic wears off.",
	"amber",	"amber",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"detect poison",
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	&gsn_detect_poison,	SLOT(21),	 5,	2,
	"",			"!Detect Poison!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"dispel evil",
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_dispel_evil,	SLOT(22),	10,	12,
	"patron deity",		"!Dispel Evil!",
	"holy",		"holy",
	"$N is completely dissolved by your goodness.",
	"$n dissolves you, you regret having been so evil, and die...",
	"$n completely dissolves $N.",
	"$N shivers and suffers from $S evilness.",
	"$N makes your evil soul hurt and suffer.",
	"$n makes $N's evil spirit shiver and suffer.",
	"$N is unaffected by your goodness.",
	"$n thinks $e is really good, and tries to dispel you.",
	"$N laughs at $n as $e tries to dispel $M."
    },

    {
	"dispel good",
	spell_dispel_good,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_dispel_good,	SLOT(310),	10,	12,
	"patron deity",		"!Dispel Good!",
	"evil",		"evil",
	"$N is completely dissolved by your evilness.",
	"$n dissolves you, you regret having been so good, and die...",
	"$n completely dissolves $N.",
	"$N shivers and suffers from $S goodness.",
	"$N makes your good soul hurt and suffer.",
	"$n makes $N's good spirit shiver and suffer.",
	"$N is unaffected by your evilness.",
	"$n thinks $e is really evil, and tries to dispel you.",
	"$N laughs at $n as $e tries to dispel $M."
    },

    {
	"dispel magic",
	spell_dispel_magic,	TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_dispel_magic,	SLOT(59),	15,	3,
	"",			"!Dispel Magic!",
	"fuscia",	"fuscia",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"earthquake",
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	&gsn_earthquake,	SLOT(23),	50,	12,
	"earthquake",		"!Earthquake!",
	"shakin",	"shakin",
	"$N is crushed as a fissure from your earthquake opens beneath $M.",
	"You are swallowed by a fissure created by $n's earthquake.",
	"$N is crushed as a fissure from $n's earthquake opens beneath $M.",
	"$N is knocked to the ground.",
	"You are knocked to the ground.",
	"$N is knocked to the ground.",
	"$N stays on $S feet.",
	"You stay on your feet.",
	"$N stays on $S feet."
    },

    {
	"ego whip",
	spell_ego_whip,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_ego_whip,		SLOT( 312 ),	33,	4,
	"powerful whip",	"!Ego Whip!",
	"you",		"you",
	"You mentally force $N's brain to explode into nothingness.",
	"$n mentally forces your brain to explode into nothingness.",
	"$n glares at $N, causing $S brain to explode into nothingness.",
	"You smash at $N's mind with great psionic force.",
	"$n smashes at your mind with great psionic force.",
	"$n glares at $N.",
	"Your mental attack misses $N and hits a rock.",
	"$n's mental attack misses you.",
	"$n glares at $N"
    },

    {
	"energy drain",
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_energy_drain,	SLOT(327),	15,	12,
	"drain",	"!Energy Drain!",
	"grey",	"grey",
	"$N dies as you drain the last life from $M.",
	"You die as $n drains the last life from your body.",
	"$N dies as $n drains the last life from $M.",
	"You drain some of $N's energy.",
	"$n drains some of your energy.",
	"$n drains some of $N's energy.",
	"You need more draining lessons.",
	"Luckily for you, $n slept through draining lessons.",
	"$n fails to drain $N - what a waste of mana!"
    },

    {
	"enchant weapon",
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	&gsn_enchant_weapon,	SLOT(24),	30,	4,
	"",			"!Enchant Weapon!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"entangle",
	spell_entangle,		TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_entangle,		SLOT( 333 ),	 20,	12,
	"",			"The roots around your feet loosen.",
	"dull brown",	"dull brown",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"faerie fire",
	spell_faerie_fire,	TAR_CHAR_HINDRANCE,	POS_FIGHTING,
	&gsn_faerie_fire,	SLOT(72),	 5,	12,
	"faerie fire",		"The pink aura around you fades away.",
	"hot pink",	"hot pink",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"faerie fog",
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	&gsn_faerie_fog,	SLOT(73),	12,	4,
	"faerie fog",		"!Faerie Fog!",
	"purple",	"purple",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"farsee",
	spell_farsee,		TAR_CHAR_SELF,	POS_STANDING,
	&gsn_farsee,		SLOT(309),	 20,	12,
	"",			"Your eyesight goes back to normal.",
	"shiny",	"shiny",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"feeblemind",
	spell_feeblemind,	TAR_CHAR_HINDRANCE,	POS_STANDING,
	NULL,			SLOT(331),	50,	12,
	"spell",		"You don't feel as dumb anymore.",
	"dull silver",	"dull silver",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"fireball",
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_fireball,		SLOT(26),	15,	12,
	"ball of flames",		"!Fireball!",
	"golden",	"golden",
	"Your fireball hits $N with full force, causing an immediate death!",
	"As you burst into flames of death, $n grins evilly...",
	"The heat from a fireball from $n turns $N into a charred corpse!",
	"You throw a fireball at $N and have the satisfaction of seeing $M enveloped in flames!",
	"You are enveloped in flames from a fireball sent by $n -- OUCH!!",
	"$n smirks as $s fireball explodes into the face of $N!",
	"Your fireball blooms a few feet before $N, who claps $S hands in wonder.",
	"$n aims a fireball at you, but it blooms a few feet in front of you.",
	"A fireball from $n misses $N - what great fireworks around here."
    },

    {
	"fireshield",
	spell_fireshield,	TAR_CHAR_SELF,	POS_STANDING,
	&gsn_fireshield,	SLOT(307),	 200,	8,
	"",			"You are no longer flaming.",
	"flaming",	"flaming",
	"$N is charred to a smoldering crisp as $E touches your shield of fire.",
	"You are charred to a smoldering crisp as you touch $n's shield of fire.",
	"$N is charred to a smoldering crisp as $E touches $n's shield of fire.",
	"$N is burned as $E touches your shield of fire.",
	"You are burned as you touch $n's shield of fire.",
	"$N is burned as $E touches $n's shield of fire.",
	"$N is unaffected by your shield of fire.",
	"You are unaffected by $n's shield of fire.",
	"$N is unaffected by $n's shield of fire."
    },

    {
	"flamestrike",
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_flamestrike,	SLOT(65),	15,	12,
	"column of flame",		"!Flamestrike!",
	"blazing",	"blazing",
	"A column of flame incinerates $N.",
	"A column of flame incinerates you.",
	"A column of flame incinerates $N.",
	"A column of flame torches $N's body.",
	"A column of flame torches your body.",
	"A column of flame torches $N's body.",
	"A column of flame barely misses $N.",
	"A column of flame barely misses you.",
	"A column of flame barely misses $N."
    },

    {
	"fly",
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_fly,		SLOT(56),	10,	3,
	"",			"You slowly float to the ground.",
	"airy",		"airy",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"gate",
	spell_gate,		TAR_IGNORE,	POS_STANDING,
	&gsn_gate,		SLOT(83),	100,	24,
	"",			"!Gate!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"harm",
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_harm,		SLOT(27),	50,	12,
	"harm spell",		"!Harm!",
	"blue",		"blue",
	"$N's collapses under immense pain.",
	"You collapse under immense pain.",
	"$N's collapses under immense pain.",
	"$N's collapses under immense pain.",
	"You collapse under immense pain.",
	"$N's collapses under immense pain.",
	"$N twinges as he feels your spell pass.",
	"You feel a twinge as if immense power just missed you.",
	"$N twinges slightly."
    },

    {
	"heal",
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_heal,		SLOT(28),	30,	12,
	"",			"!Heal!",
	"blue",		"blue",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"ice storm",
	spell_ice_storm,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_ice_storm,		SLOT(70),	25,	18,
	"spray of ice",	"!Ice Storm!",
	"frozen",	"frozen",
	"You create a giant block of ice above $N, causing a fast and fatal crushing.",
	"$n crushes you with a giant block of conjured ice, R.I.P.",
	"$n waves $s hands and drops a large chunk of ice on $N, who is now flat.",
	"$N is thrashed to a bloody pulp by your ice storm.",
	"You are thrashed to a bloody pulp by $n's ice storm!",
	"$n thrashes $N to a bloody pulp with $s ice storm!",
	"$N dodges the block of ice you toss at $M.",
	"$n conjures a block of ice in your direction, but you avoid it.",
	"$n conjures a block of ice in $N's direction, but $E avoids it."
    },

    {
	"illusory appearance",
	spell_illusory_appearance, TAR_CHAR_SELF,		POS_STANDING,
	&gsn_illusory_appearance,	SLOT(329),	50,	12,
	"",			"Your outer form returns to normal.",
	"wavering",	"wavering",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"illusory auras",
	spell_illusory_auras, 	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_illusory_auras,	SLOT(338),	50,	12,
	"",			"You stop shimmering.",
	"wavering",	"wavering",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"improved invis",
	spell_improved_invis,	TAR_CHAR_SELF,	POS_STANDING,
	&gsn_improved_invis,	SLOT(306),	 100,	8,
	"",			"You are no longer invisible.",
	"utterly transparent",	"utterly transparent",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"infravision",
	spell_infravision,	TAR_CHAR_SELF,	POS_STANDING,
	&gsn_infravision,	SLOT(77),	 5,	3,
	"",			"You no longer see in the dark.",
	"red",		"red",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"invis",
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		SLOT(29),	 35,	2,
	"",			"You are no longer invisible.",
	"clear",	"clear",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"invulnerability",
	spell_invulnerability,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invulnerability,	SLOT( 317 ),	30,	12,
	"invulnerability",	"The shell around you collapses.",
	"creamy",	"creamy",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"know alignment",
	spell_know_alignment,	TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_know_aligment,	SLOT(58),	 9,	12,
	"",			"!Know Alignment!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"lightning bolt",
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_lightning_bolt,	SLOT(30),	15,	9,
	"bolt of lightning",	"!Lightning Bolt!",
	"neon blue",	"neon blue",
	"Your lightning bolt shatters $N to pieces!",
	"You feel enlightened by the lightning bolt $n sends, and then all is darkness.",
	"$N recieves the full blast of a lightning bolt from $n ... and is no more.",
	"The lightning bolt hits $N with full impact!",
	"YOU'RE HIT!  A lightning bolt from $n has reached it's goal!",
	"$N wavers under the impact of the lightning sent by $n!",
	"Your lightning bolt scores the ground in front of $N.",
	"The ground before you is scored as $n misses you with lightning.",
	"$N jumps backwards in time to avoid a lightning bolt sent by $n."
    },

    {
	"locate object",
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	&gsn_locate_object,	SLOT(31),	15,	4,
	"",			"!Locate Object!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"magic missile",
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_magic_missile,	SLOT(32),	15,	4,
	"magic missile",	"!Magic Missile!",
	"shiny",	"shiny",
	"The magic missile tears away the last life of $N.",
	"You only have time to notice $n uttering strange words before everything is dark.",
	"The magic missile sent by $n causes $N to stagger and collapse in a lifeless heap.",
	"You watch in self pride as the missile hits $N.",
	"You stagger as a magic missile from $n hits you.",
	"$n throws a magic missile at $N, who staggers under the blow.",
	"The magic missile penetrates the air miles above $N.",
	"You don't even have to duck as $n sends a missile way over your head.",
	"$n tries in vain to correct the course of the magic missile meant to hit $N"
    },

    {
	"mass invis",
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	SLOT(69),	20,	4,
	"",			"!Mass Invis!",
	"clear",	"clear",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"mesmerize",
	spell_mesmerize,	TAR_CHAR_HINDRANCE,	POS_FIGHTING,
	&gsn_mesmerize,		SLOT(337),	15,	12,
	"",			"The sky seems less interesting.",
	"sky blue",	"sky blue",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"miracle",
	spell_miracle,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_miracle,		SLOT(303),	50,	12,
	"miracle",		"!Miracle!",
	"lumpy black",	"lumpy black",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"no summon",
	spell_no_summon,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_no_summon,		SLOT(301),	50,	12,
	"no summon",		"You feel less heavy.",
	"heavy",	"heavy",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"pass door",
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_pass_door,		SLOT(74),	10,	4,
	"",			"You feel solid again.",
	"translucent",	"translucent",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"personal illusion",
	spell_personal_illusion, TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(335),	50,	4,
	"",			"!Personal Illusion!",
	"dual colored",	"dual colored",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"phantasmal weaponry",
	spell_phantasmal_weaponry, TAR_CHAR_SELF,	POS_STANDING,
	&gsn_phantasmal_weaponry, SLOT( 323 ),	10,	12,
	"",			"Your shadow fades to one.",
	"sparkling",	"sparkling",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"poison",
	spell_poison,		TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_poison,		SLOT(33),	40,	3,
	"poison",		"You feel less sick.",
	"green",	"green",
	"You shrivel and die of poison.",
	"You shrivel and die of poison.",
	"$n suddenly shrivels and dies.",
	"You shiver and suffer.",
	"You shiver and suffer.",
	"$n shivers and suffers",
	"You resist the poison's effects.",
	"You resist the poison's effects.",
	"$n shivers."
    },

    {
	"protection",
	spell_protection,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_protection,	SLOT(34),	 5,	3,
	"",			"You feel less protected.",
	"dull white",	"dull white",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"psionic blast",
	spell_psionic_blast,	TAR_IGNORE,		POS_FIGHTING,
	&gsn_psionic_blast,	SLOT( 313 ),	33,	8,
	"psionic blast",	"You feel less dizzy.",
	"psychadelic",	"psychadelic",
	"The blast of power smashes $n's body into a bloody pulp.",
	"The blast of power smashes your body into a bloody pulp.",
	"The blast of power smashes $n's body into a bloody pulp.",
	"The energy slams into $N and $E goes soaring!",
	"The energy slams into you and you go soaring!",
	"The energy slams into $N and $E goes soaring!",
	"The energy slams into $N and $E goes soaring!",
	"The energy slams into you and you go soaring!",
	"The energy slams into $N and $E goes soaring!"
    },

    {
	"reflect",
	spell_reflect,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_reflect,		SLOT( 325 ),	 50,	12,
	"",			"The air around you stops tingling.",
	"bright",	"bright",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"remove curse",
	spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_remove_curse,	SLOT(35),	 5,	4,
	"",			"!Remove Curse!",
	"black",	"black",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"sanctuary",
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_sanct,		SLOT(36),	75,	12,
	"",			"The white aura around your body fades.",
	"purple",	"purple",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"shield",
	spell_shield,		TAR_CHAR_SELF,	POS_STANDING,
	&gsn_shield,		SLOT(67),	12,	4,
	"",			"Your force shield shimmers then fades away.",
	"brown",	"brown",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"shocking grasp",
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_shocking_grasp,	SLOT(37),	15,	8,
	"shocking grasp",	"!Shocking Grasp!",
	"electric",	"electric",
	"$N dies while looking rather shocked.",
	"$n shocks you, unfortunately, your heard can't stand it...",
	"$n reveals with a shock that $N has left $S body.",
	"You get a good hold of the shocked $N.",
	"You get a shock as $n gets too close to you.",
	"$N gets shocked as $n grasps at $M.",
	"$N rather shockingly evades your grasp.",
	"You nimbly dodge a shocking grasp from $n.",
	"$N shockingly evades the grasp of $n."
    },

    {
	"silence",
	spell_silence,		TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_silence,		SLOT(100),	15,	12,
	"",			"The muscles in your mouth relax.",
	"dull grey",	"dull grey",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"sleep",
	spell_sleep,		TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_sleep,		SLOT(38),	15,	12,
	"",			"You feel less tired.",
	"light blue",	"starry",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"slow",
	spell_slow,		TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_slow,		SLOT(101),	15,	12,
	"",			"Your body returns to it's normal speed.",
	"viscous",	"dull grey",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"stone skin",
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_stone_skin,		SLOT(66),	12,	4,
	"",			"Your skin feels soft again.",
	"stony",	"stony",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"strength",
	spell_strength,		TAR_CHAR_SELF,	POS_STANDING,
	&gsn_strength,		SLOT(39),	10,	3,
	"",			"You feel weaker.",
	"silver",	"silver",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"summon",
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	&gsn_summon,		SLOT(40),	50,	24,
	"",			"!Summon!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"teleport",
	spell_teleport,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_teleport, 		SLOT( 2),	35,	12,
	"",			"!Teleport!",
	"shiny",	"shiny",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"tower of id",
	spell_tower_of_id,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_tower_id,		SLOT( 311 ),	33,	3,
	"",			"Your shield dies away.",
	"flat",		"flat",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"vampiric touch",
	spell_vampiric_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_vampiric_touch,	SLOT(25),	25,	12,
	"freezing touch",	"!Vampiric Touch!",
	"blood red",	"blood red",
	"You draw the last blood from $N and leave a lifeless corpse!",
	"$n grows stronger and stronger, as you die.",
	"$n drinks the last drop of blood from $N's body, leaving a corpse!",
	"Your forces are restored from the blood of $N.",
	"$n sucks away your life forces and adds them to $s own.",
	"$n draws life forces from $N.",
	"You need more draining lessons.",
	"Luckily for you, $n slept through draining lessons.",
	"$n fails to drain $N - what a waste of mana!"
    },

    {
	"ventriloquate",
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	&gsn_ventriloquate,	SLOT(41),	 5,	1,
	"",			"!Ventriloquate!",
	"bug",	"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"void aura",
	spell_void_aura,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_void_aura,		SLOT( 326 ),	 50,	12,
	"",			"The black aura about you fades.",
	"black",	"black",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"weaken",
	spell_weaken,		TAR_CHAR_HINDRANCE,	POS_STANDING,
	&gsn_weaken,		SLOT(68),	30,	6,
	"spell",		"You feel stronger.",
	"dull silver",	"dull silver",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"weapon immunity",
	spell_weapon_immunity,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_weapon_immunity,	SLOT(334),	50,	12,
	"spell",		"The metallica aura around you fades.",
	"bright gold",	"bright gold",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"whirling wind",
	spell_whirling_wind,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(332),	15,	12,
	"blast of wind",	"!Whirling Wind!",
	"wispy",	"wispy",
	"A blast of wind batters $N into a bloody corpse.",
	"A blast of wind batters you into a bloody corpse.",
	"A blast of wind batters $N into a bloody corpse.",
	"A blast of wind batters $N.",
	"A blast of wind batters you.",
	"A blast of wind batters $N.",
	"$N stands solid as a blast of wind blows past.",
	"You stand solid as a blast of wind blows past.",
	"$N stands solid as a blast of wind blows past."
    },

    {
	"wizard eye",
	spell_wizard_eye,	TAR_IGNORE,		POS_STANDING,
	NULL,		SLOT(0),	 20,	12,
	"",			"!Wizard Eye!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"word of recall",
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_word_of_recall,	SLOT(42),	 50,	4,
	"",			"!Word of Recall!",
	"clear red",	"clear red",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

/*
 * Dragon breath
 * changed wait state from 4 to 40
 */
    {
	"acid breath",
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_acid_breath,	SLOT(200),	 0,	 12,
	"blast of acid",	"!Acid Breath!",
	"acidic",	"acidic",
	"Your spray of acid completely dissolves $N.",
	"$n's spray of acid completely dissolves you.",
	"$n's spray of acid completely dissolves $N.",
	"Your acid spray scars $N's skin.",
	"$n's acid spray scars your skin.",
	"$n's acid spray scars $N's skin.",
	"Your blast of acid barely misses $N.",
	"$n throws acid at Your, but you barely manage to dodge it.",
	"$n throws acid at $N, but $E barely manages to dodge it."
    },

    {
	"fire breath",
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_fire_breath,	SLOT(201),	 0,	 12,
	"blast of flame",	"!Fire Breath!",
	"firey",	"firey",
	"Your fireball hits $N with full force, causing an immediate death!",
	"As you burst into flames of death, $n grins evilly...",
	"The heat from a fireball from $n turns $N into a charred corpse!",
	"You throw a fireball at $N and have the satisfaction of seeing $M enveloped in flames!",
	"You are enveloped in flames from a fireball sent by $n -- OUCH!!",
	"$n smirks as $s fireball explodes into the face of $N!",
	"Your fireball blooms a few feet before $N, who claps $S hands in wonder.",
	"$n aims a fireball at you, but it blooms a few feet in front of you.",
	"A fireball from $n misses $N - what great fireworks around here."
    },

    {
	"frost breath",
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_frost_breath,	SLOT(202),	 0,	 12,
	"blast of frost",	"!Frost Breath!",
	"frosty",	"frosty",
	"Your frozen breath turns $N into an unliving icicle.",
	"$n's frozen breath turns you into an unliving icicle.",
	"$n's frozen breath turns $N into an unliving icicle.",
	"Your frozen breath causes ice to form on $N's body.",
	"$n's frozen breath causes ice to form on your body.",
	"$n's frozen breath causes ice to form on $N's body.",
	"$N jumps out of the way of your frozen breath.",
	"You jump out of the way of $n's frozen breath.",
	"$N jumps out of the way of $n's frozen breath."
    },

    {
	"gas breath",
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	&gsn_gas_breath,	SLOT(203),	 0,	 12,
	"blast of gas",		"!Gas Breath!",
	"smelly",	"smelly",
	"Your cloud of toxic breath suffocates $N.",
	"$n's cloud of toxic breath suffocates you.",
	"$n's cloud of toxic breath suffocates $N.",
	"Your green breath causes $N to cough and wheeze.",
	"$n's green breath causes you to cough and wheeze.",
	"$n's green breath causes $N to cough and wheeze.",
	"Your hazardous breath dissipates before it reaches $N.",
	"$n's hazardous breath dissipates before it reaches you.",
	"$n's hazardous breath dissipates before it reaches $N."
    },

    {
	"lightning breath",
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_lightning_breath,	SLOT(204),	 0,	 12,
	"blast of lightning",	"!Lightning Breath!",
	"neon blue",	"neon blue",
	"$N is shattered as a lightning bolt arcs from your teeth.",
	"A burst of electricity shatters you moments after appearing in $n's teeth.",
	"$N is shattered as $n's lightning bolt arcs from $s teeth.",
	"A burst of blue flame envelops $N as your lightning bolt crackles in the air.",
	"You are burned by electricity as $n arcs a lightning bolt at you.",
	"$N is enveloped in blue flame as $n fries $m with a lightning bolt.",
	"Your lightning bolt scores the ground in front of $N.",
	"The ground before you is scored as $n misses you with lightning.",
	"$N jumps backwards in time to avoid a lightning bolt sent by $n.",
    },

/*
 * Fighter and thief skills.
 */
    {
	"archery",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_archery,		SLOT( 0),	 0,	1,
	"archery",		"!Archery!",
	"bug",		"bug",
	"$N's head splits in two as an arrow hits $M between the eyes.",
	"Your head is split in two as an arrow hits you between the eyes",
	"$N's head splits in two as an arrow hits $M between the eyes.",
	"The arrow hits $N solidly!",
	"The arrow hits you!",
	"The arrow hits $N!",
	"The arrow barely misses $N!",
	"The arrow barely misses you!",
	"The arrow barely misses $N."
    },

    {
	"backstab",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		SLOT( 0),	 0,	24,
	"backstab",		"!Backstab!",
	"bug",		"bug",
	"$N makes a strange sound but is suddenly very silent as you place $p in $S back.",
	"Suddenly $n stabs you in the back,  R.I.P....",
	"$n places $p in the back of $N resulting in some strange noices, a lot of blood, and a corpse.",
	"$N makes a strange sound as you place $p in $S back!",
	"Suddenly $n stabs you in the back!",
	"$n places $p in the back of $N, resulting in some strange noises and blood!",
	"$N quickly avoids your backstab and you nearly cut your finger!",
	"$n tries to backstab you, but you avoid $m!",
	"$n tries to backstab $N, but nearly cuts $s finger."
    },

    {
	"balance",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_balance,		SLOT( 0),	 0,	0,
	"",			NULL,
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"bash",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_bash,		SLOT( 0),	 100,	12,
	"",			"!Bash!",
	"bug",		"bug",
	"$N doesn't recover from your bash!  $E is dead!",
	"As $n bashes at you, you give up living and die.",
	"$N dies as $n bashes $M.",
	"You send $N sprawling with a powerful bash.",
	"$n sends you sprawling with a powerful bash.",
	"$n sends $N sprawling with a powerful bash.",
	"As $N avoids your bash, you topple over and fall to the ground.",
	"You dodge a bash from $n, who loses $s balance and falls.",
	"$N avoids being bashed by $n, who loses $s balance and falls."
    },

    {
	"disarm",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		SLOT( 0),	 0,	12,
	"",			"!Disarm!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"dodge",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dodge,		SLOT( 0),	 100,	 0,
	"",			"!Dodge!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"enhanced damage",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_damage,	SLOT( 0),	 100,	 100,
	"",			"!Enhanced Damage!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"follow",
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_follow,		SLOT( 0),	 0,	12,
	"",			"!Follow!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"hide",
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		SLOT( 0),	 0,	12,
	"",			"!Hide!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"ki release",
	spell_ki_release,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_ki_release,	SLOT( 314 ),	33,	15,
	"ki release",		"!Ki Release!",
	"radiant",	"radiant",
	"Your qi blasts a hole through $N's body.",
	"$n's qi blasts a hole through your body.",
	"$n's qi blasts a hole through $N's body."
	"You release all your qi and send $N reeling!",
	"$n releases all $s qi and sends you reeling.",
	"$n releases all $s qi and sends $N reeling.",
	"You release all your qi and send $N reeling!",
	"$n releases all $s qi and sends you reeling.",
	"$n releases all $s qi and sends $N reeling."
    },

    {
	"kick",
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		SLOT( 0),	 0,	 36,
	"kick",			"!Kick!",
	"bug",		"bug",
	"Your kick at $N's face splits $S head open - yummy.",
	"$n aims a kick at your face which splits your head in two.",
	"$n neatly kicks $N's head into pieces - brains ooze out all over.",
	"Your kick hits $N in the solar plexus.",
	"You're hit in solar plexus, wow, this is breathtaking !!.",
	"$n kicks $N in solar plexus, $N is rendered breathless.",
	"You miss your kick at $N's groin, much to $S relief.",
	"$n misses a kick at your groin, you breathe lighter now.",
	"$n misses a kick at $N's groin."
    },

    {
	"parry",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_parry,		SLOT( 0),	 100,	 0,
	"",			"!Parry!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"peek",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		SLOT( 0),	 0,	 0,
	"",			"!Peek!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"pick lock",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		SLOT( 0),	 0,	8,
	"",			"!Pick!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"rescue",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		SLOT( 0),	 0,	8,
	"",			"!Rescue!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"second attack",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_second_attack,	SLOT( 0),	 0,	 0,
	"",			"!Second Attack!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"sixth sense",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sixth_sense,	SLOT( 0),	 0,	0,
	"",			NULL,
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"snare",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_snare,		SLOT( 0),	 0,	0,
	"",			NULL,
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"sneak",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		SLOT( 0),	 0,	0,
	"",			NULL,
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"steal",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		SLOT( 0),	 0,	12,
	"",			"!Steal!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"steal enchantment",
	spell_steal_enchantment, TAR_CHAR_HINDRANCE,	POS_STANDING,
	NULL,			SLOT( 324 ),	50,	12,
	"",			"!Steal Enchantment!",
	"buggy",	"buggy",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"swim",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_swim,		SLOT( 0),	 0,	12,
	"",			"!Swim!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"third attack",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_third_attack,	SLOT( 0),	 0,	 0,
	"",			"!Third Attack!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"track",
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_track,		SLOT( 0),	 0,	12,
	"",			"!Track!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
	"general purpose",
	spell_general_purpose,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(501),	0,	12,
	"general purpose ammo",	"!General Purpose Ammo!",
	"lead",		"lead",
	"You blast a hole through $N's head.",
	"$n blasts a hole through your head.",
	"$n blasts a hole through $N's head.",
	"You fire a shot at $N, drawing blood.",
	"$n's shot draws blood as it tears through your body.",
	"$n's shot draws blood as it tears through $N's body.",
	"Your shot misses $N",
	"$n fires at you, but misses.",
	"$n fires at $N, but misses."
    },

    {
	"high explosive",
	spell_high_explosive,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(502),	0,	12,
	"high explosive ammo",	"!High Explosive Ammo!",
	"heavy metal",	"heavy metal",
	"Your shot explodes with $N's body, spreading it over a mile.",
	"$n's shot explodes with your body, spreading it over a mile.",
	"$n's shot explodes with $N's body, spreading it over a mile.",
	"You fire a shot at $N, drawing blood.",
	"$n's shot draws blood as it tears through your body.",
	"$n's shot draws blood as it tears through $N's body.",
	"Your shot misses $N",
	"$n fires at you, but misses.",
	"$n fires at $N, but misses."
    },

    {
	"dragon power",
	spell_dragon_power,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_dragon_power,	SLOT(302),	100,	4,
	"dragon power",		"You feel less powerful.",
	"scaly",	"scaly",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"become rambo",
	spell_become_rambo,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_become_rambo,	SLOT(304),	 100,	4,
	"become rambo",		"Your mutated muscles shrivel to withered flesh.",
	"potent",	"potent",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"read magic",
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_read_magic,	SLOT( 0),	 0,	12,
	"",			"!Read Magic!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {

	"use wands",
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_use_wands,		SLOT( 0),	 0,	12,
	"",			"!Use Wands!",
	"bug",		"bug",
        "The wand shatters into a million pieces, many of them impaling you thoroughly.",
        "The wand shatters into a million pieces, many of them impaling you thoroughly.",
        "A wand shatters into a million pieces as $n attempts to use it, many of them impaling $m.",
        "The wand explodes in your hands.",
        "The wand explodes in your hands.",
        "A wand explodes in $n's hands.",
        "The wand explodes in your hands.",
        "The wand explodes in your hands.",
        "A wand explodes in $n's hands."
    },

    {

	"use staffs",
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_use_staffs,	SLOT( 0),	 0,	12,
	"",			"!Use Staffs!",
	"bug",		"bug",
        "The staff shatters into a million pieces, many of them impaling you thoroughly.",
        "The staff shatters into a million pieces, many of them impaling you thoroughly.",
        "A staff shatters into a million pieces as $n attempts to use it, many of them impaling $m.",
        "The staff explodes in your hands.",
        "The staff explodes in your hands.",
        "A staff explodes in $n's hands.",
        "The staff explodes in your hands.",
        "The staff explodes in your hands.",
        "A staff explodes in $n's hands."
    },

    {
	"smoke bomb",
	spell_smoke_bomb,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_smoke_bomb,	SLOT( 315 ),	33,	16,
	"smoke bomb",		"!Smoke Bomb!",
	"smoky",	"smoky",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"caltrops",
	spell_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_caltrops,		SLOT( 0 ),	0,	0,
	"caltrops",		"!Caltrops!",
	"bug",		"bug",
	"$N steps on some caltrops hard, and leaks all $S blood through $S foot.",
	"You step on some caltrops hard, and leak all your blood through your foot.",
	"$N steps on some caltrops hard, and leaks all $S blood through $S foot.",
	"$N steps on some caltrops.",
	"OUCH!  You step on some caltrops.",
	"$N steps on some caltrops.",
	"$N's boot absorbs a few caltrops.",
	"Your boot absorbs a few caltrops.",
	"$N's boot absorbs a few caltrops."
    },

    {
	"disarm traps",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_disarm_traps,	SLOT( 318 ),	30,	30,
	"disarm traps",		"!Disarm Traps!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },
  
    {
	"set traps",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_set_traps,		SLOT( 319 ),	30,	30,
	"set traps",		"!Set Traps!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"find traps",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_find_traps,	SLOT( 320 ),	30,	30,
	"find traps",		"!Find Traps!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    },

    {
	"free wheres",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_free_wheres,	SLOT( 321 ),	30,	12,
	"free wheres",		"!Free Wheres!",
	"bug",		"bug",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
};
