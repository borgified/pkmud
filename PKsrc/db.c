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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "global.h"

#if !defined(macintosh)
extern	int	_filbuf		args( (FILE *) );
#endif



/*
 * Globals.
 */
HELP_DATA *		help_first;
HELP_DATA *		help_last;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

CHAR_DATA *		char_free;
EXTRA_DESCR_DATA *	extra_descr_free;
NOTE_DATA *		note_free;
OBJ_DATA *		obj_free;
PC_DATA *		pcdata_free;
struct snoop_data *	snoop_free;

char			bug_buf		[2*MAX_INPUT_LENGTH];
CHAR_DATA *		char_list;
char *			help_greeting;
char *			help_greeting2;
char *			help_greeting3;
char *			help_greeting4;
char *			help_greeting5;
char			log_buf		[2*MAX_INPUT_LENGTH];
NOTE_DATA *		note_list;
OBJ_DATA *		object_list;
TIME_INFO_DATA		time_info;
WEATHER_DATA		weather_info;


sh_int		gsn_acid_breath;
sh_int		gsn_archery;
sh_int		gsn_armor;
sh_int		gsn_backstab;
sh_int		gsn_balance;
sh_int		gsn_bash;
sh_int		gsn_become_rambo;
sh_int		gsn_blackmantle;
sh_int		gsn_bless;
sh_int		gsn_blindness;
sh_int		gsn_brilliance;
sh_int		gsn_burning_hands;
sh_int		gsn_call_lightning;
sh_int		gsn_caltrops;
sh_int		gsn_cause_critical;
sh_int		gsn_cause_light;
sh_int		gsn_cause_serious;
sh_int		gsn_change_sex;
sh_int		gsn_charm_person;
sh_int		gsn_chill_touch;
sh_int		gsn_colour_spray;
sh_int		gsn_confusion;
sh_int		gsn_continual_light;
sh_int		gsn_control_weather;
sh_int		gsn_create_food;
sh_int		gsn_create_spring;
sh_int		gsn_create_water;
sh_int		gsn_cure_blindness;
sh_int		gsn_cure_critical;
sh_int		gsn_cure_light;
sh_int		gsn_cure_poison;
sh_int		gsn_cure_serious;
sh_int		gsn_curse;
sh_int		gsn_dancing_sword;
sh_int		gsn_death_spray;
sh_int		gsn_detect_evil;
sh_int		gsn_detect_hidden;
sh_int		gsn_detect_invis;
sh_int		gsn_detect_magic;
sh_int		gsn_detect_poison;
sh_int		gsn_disarm;
sh_int		gsn_disarm_traps;
sh_int		gsn_dispel_evil;
sh_int		gsn_dispel_good;
sh_int		gsn_dispel_magic;
sh_int		gsn_dodge;
sh_int		gsn_dragon_power;
sh_int		gsn_earthquake;
sh_int		gsn_enchant_weapon;
sh_int		gsn_energy_drain;
sh_int		gsn_enhanced_damage;
sh_int		gsn_entangle;
sh_int		gsn_explosion;
sh_int		gsn_faerie_fire;
sh_int		gsn_faerie_fog;
sh_int		gsn_farsee;
sh_int		gsn_find_traps;
sh_int		gsn_fire_breath;
sh_int		gsn_fireball;
sh_int		gsn_fireshield;
sh_int		gsn_flamestrike;
sh_int		gsn_fly;
sh_int		gsn_free_wheres;
sh_int		gsn_frost_breath;
sh_int		gsn_gate;
sh_int		gsn_gas_breath;
sh_int		gsn_harm;
sh_int		gsn_heal;
sh_int		gsn_follow;
sh_int		gsn_hide;
sh_int		gsn_ice_storm;
sh_int		gsn_illusory_appearance;
sh_int		gsn_illusory_auras;
sh_int		gsn_improved_invis;
sh_int		gsn_infravision;
sh_int		gsn_invis;
sh_int		gsn_invulnerability;
sh_int		gsn_ki_release;
sh_int		gsn_kick;
sh_int		gsn_know_aligment;
sh_int		gsn_lightning_bolt;
sh_int		gsn_lightning_breath;
sh_int		gsn_locate_object;
sh_int		gsn_magic_missile;
sh_int		gsn_mass_invis;
sh_int		gsn_mesmerize;
sh_int		gsn_miracle;
sh_int		gsn_no_summon;
sh_int		gsn_parry;
sh_int		gsn_pass_door;
sh_int		gsn_peek;
sh_int		gsn_phantasmal_weaponry;
sh_int		gsn_pick_lock;
sh_int		gsn_poison;
sh_int		gsn_protection;
sh_int		gsn_psionic_blast;
sh_int		gsn_ego_whip;
sh_int		gsn_read_magic;
sh_int		gsn_reflect;
sh_int		gsn_remove_curse;
sh_int		gsn_rescue;
sh_int		gsn_sanct;
sh_int		gsn_second_attack;
sh_int		gsn_set_traps;
sh_int		gsn_shield;
sh_int		gsn_shocking_grasp;
sh_int		gsn_silence;
sh_int		gsn_sixth_sense;
sh_int		gsn_sleep;
sh_int		gsn_slow;
sh_int		gsn_snare;
sh_int		gsn_sneak;
sh_int		gsn_smoke_bomb;
sh_int		gsn_steal;
sh_int		gsn_stone_skin;
sh_int	    	gsn_strangle;
sh_int		gsn_strength;
sh_int		gsn_summon;
sh_int		gsn_swim;
sh_int		gsn_teleport;
sh_int		gsn_third_attack;
sh_int		gsn_tower_id;
sh_int		gsn_track;
sh_int		gsn_use_staffs;
sh_int		gsn_use_wands;
sh_int		gsn_vampiric_touch;
sh_int		gsn_ventriloquate;
sh_int		gsn_void_aura;
sh_int		gsn_weaken;
sh_int		gsn_weapon_immunity;
sh_int		gsn_word_of_recall;



/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash		[MAX_KEY_HASH];
OBJ_INDEX_DATA *	obj_index_hash		[MAX_KEY_HASH];
ROOM_INDEX_DATA *	room_index_hash		[MAX_KEY_HASH];
char *			string_hash		[MAX_KEY_HASH];

AREA_DATA *		area_first;
AREA_DATA *		area_last;

char *			string_space;
char *			top_string;

int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_reset;
int			top_room;
int			top_shop;



/*
 * Memory management.
 * Increase MAX_STRING if you have too.
 * Tune the others only if you understand what you're doing.
 */
#define			MAX_STRING	2097152
#define			MAX_PERM_BLOCK	131072
#define			MAX_MEM_LIST	11

void *			rgFreeList	[MAX_MEM_LIST];
const int		rgSizeList	[MAX_MEM_LIST]	=
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};

int			nAllocString;
int			sAllocString;
int			nAllocPerm;
int			sAllocPerm;



/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea[MAX_INPUT_LENGTH];



/*
 * Local booting procedures.
 */
void	init_mm		args( ( void ) );

void	load_area	args( ( FILE *fp ) );
void	load_helps	args( ( FILE *fp ) );
void	load_mobiles	args( ( FILE *fp ) );
void	load_objects	args( ( FILE *fp ) );
void	load_resets	args( ( FILE *fp ) );
void	load_rooms	args( ( FILE *fp ) );
void	load_shops	args( ( FILE *fp ) );
void	load_specials	args( ( FILE *fp ) );
void	load_notes	args( ( void ) );

void	fix_exits	args( ( void ) );

void	reset_area	args( ( AREA_DATA * pArea , unsigned int phase ) );



/*
 * Big mama top level function.
 */
void boot_db( void )
{
  int i;
    extern void read_classes();
    extern void read_socials();
    extern void init_last_table( void );
    /*
     * Init some data space stuff.
     */
    {
	if ( ( string_space = (char *) calloc( 1, MAX_STRING ) ) == NULL )
	{
	    bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
	    exit( 1 );
	}
	top_string	= string_space;

	for ( i = 0; i < MAX_MEM_LIST; i++ )
	  rgFreeList[i] = NULL;

	fBootDb		= TRUE;
    }

    /*
     * Init random number generator.
     */
    {
	init_mm( );
    }


    /*
     * Set time and weather.
     */
    {
	long lhour, lday, lmonth;

	lhour		= (current_time - 650336715)
			/ (PULSE_TICK / PULSE_PER_SECOND);
	time_info.hour	= lhour  % 24;
	lday		= lhour  / 24;
	time_info.day	= lday   % 35;
	lmonth		= lday   / 35;
	time_info.month	= lmonth % 17;
	time_info.year	= lmonth / 17;

	     if ( time_info.hour <  5 ) weather_info.sunlight = SUN_DARK;
	else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
	else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
	else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
	else                            weather_info.sunlight = SUN_DARK;

	weather_info.change	= 0;
	weather_info.mmhg	= 960;
	if ( time_info.month >= 7 && time_info.month <=12 )
	    weather_info.mmhg += number_range( 1, 50 );
	else
	    weather_info.mmhg += number_range( 1, 80 );

	     if ( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
	else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
	else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
	else                                  weather_info.sky = SKY_CLOUDLESS;

    }

    /*
     * Assign gsn's for skills which have them.
     */
    {
	int sn;

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].pgsn != NULL )
		*skill_table[sn].pgsn = sn;
	}
    }

    /*
     * Read in all the area files.
     */
    {
	FILE *fpList;

	if ( ( fpList = fopen( AREA_LIST, "r" ) ) == NULL )
	{
	    perror( AREA_LIST );
	    exit( 1 );
	}

	for ( ; ; )
	{
	    strcpy( strArea, fread_word( fpList ) );
	    if ( strArea[0] == '$' )
		break;

	    if ( strArea[0] == '-' )
	    {
		fpArea = stdin;
	    }
	    else
	    {
		if ( ( fpArea = fopen( strArea, "r" ) ) == NULL )
		{
		    perror( strArea );
		    exit( 1 );
		}
	    }

	    for ( ; ; )
	    {
		char *word;

		if ( fread_letter( fpArea ) != '#' )
		{
		    bug( "Boot_db: # not found.", 0 );
		    exit( 1 );
		}

		word = fread_word( fpArea );

		     if ( word[0] == '$'               )                 break;
		else if ( !str_cmp( word, "AREA"     ) ) load_area    (fpArea);
		else if ( !str_cmp( word, "HELPS"    ) ) load_helps   (fpArea);
		else if ( !str_cmp( word, "MOBILES"  ) ) load_mobiles (fpArea);
		else if ( !str_cmp( word, "OBJECTS"  ) ) load_objects (fpArea);
		else if ( !str_cmp( word, "RESETS"   ) ) load_resets  (fpArea);
		else if ( !str_cmp( word, "ROOMS"    ) ) load_rooms   (fpArea);
		else if ( !str_cmp( word, "SHOPS"    ) ) load_shops   (fpArea);
		else if ( !str_cmp( word, "SPECIALS" ) ) load_specials(fpArea);
		else
		{
		    bug( "Boot_db: bad section name.", 0 );
		    exit( 1 );
		}
	    }

	    if ( fpArea != stdin )
		fclose( fpArea );
	    fpArea = NULL;
	}
	fclose( fpList );
    }

    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     * Load up the notes file.
     */
    {
	fix_exits( );
	fBootDb	= FALSE;
	area_update( );
	load_notes( );
      /*
       * Load in classes
       */
      read_classes();
      /* load socials */
      read_socials();
      init_high_scores( );
      init_last_table( );
    }

    return;
}


void set_join_status( CHAR_DATA *ch ) 
{
  extern int war_flags;
  extern int max_restore_hits;
  extern int max_restore_mana;

  if( IS_NPC(ch) )
    return;

  if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) )
  {
    if( ch->level != 30 )
      return;

    if( ch->hit*100/(ch->max_hit?ch->max_hit:1) < max_restore_hits )
      max_restore_hits = ch->hit*100/ch->max_hit;

    if( ch->max_mana > 0
    && ch->mana*100/ch->max_mana < max_restore_mana )
      max_restore_mana = ch->mana*100/ch->max_mana;

    if( max_restore_mana > max_restore_hits )
      max_restore_mana = max_restore_hits;

    return;
  }
  else
  {
    DESCRIPTOR_DATA *d;
    DESCRIPTOR_DATA *d_next;

    max_restore_hits = 80;
    max_restore_mana = 80;

    for( d = descriptor_list; d != NULL; d = d_next )
    {
      d_next = d->next;

      if ( d->character == NULL )
	continue;
	
      if ( !mortal_in_main_war( d->character ) )
	continue;
	
      if ( d->character->level != 30 )
	continue;
	
      if ( d->character->hit*100/d->character->max_hit < max_restore_hits )
	max_restore_hits = d->character->hit*100/d->character->max_hit;
	
      if ( d->character->max_mana > 1
      && d->character->mana*100/d->character->max_mana < max_restore_mana )
	max_restore_mana = d->character->mana*100/d->character->max_mana;
	
      if( max_restore_mana > max_restore_hits )
	max_restore_mana = max_restore_hits;
	
    }		
    return;
  }
  return;
}


/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
    AREA_DATA *pArea;

    pArea		= (AREA_DATA *) alloc_perm( sizeof(*pArea) );
    pArea->reset_first	= NULL;
    pArea->reset_last	= NULL;
    pArea->author	= fread_string( fp );
    pArea->name		= fread_string( fp );
    pArea->lifespan[WAR_NUM_DEAD] = 45;
    pArea->lifespan[WAR_NUM_MAIN] = 45;
    pArea->lifespan[WAR_NUM_FFA] = 45;
    pArea->lifespan[WAR_NUM_PRIVATE] = 45;
    pArea->nplayer	= 0;
    pArea->area_flags	= fread_number( fp );
    pArea->access_percent = fread_number( fp );

    if ( area_first == NULL )
	area_first = pArea;
    if ( area_last  != NULL )
	area_last->next = pArea;
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}



/*
 * Snarf a help section.
 */
void load_helps( FILE *fp )
{
    HELP_DATA *pHelp;

    for ( ; ; )
    {
	pHelp		= (HELP_DATA *) alloc_perm( sizeof(*pHelp) );
	pHelp->level	= fread_number( fp );
	pHelp->keyword	= fread_string( fp );
	if ( pHelp->keyword[0] == '$' )
	    break;
	pHelp->text	= fread_string( fp );

	if ( !str_cmp( pHelp->keyword, "greeting" ) )
	    help_greeting = pHelp->text;

	if ( !str_cmp( pHelp->keyword, "greeting2" ) )
	    help_greeting2 = pHelp->text;

	if ( !str_cmp( pHelp->keyword, "greeting3" ) )
	    help_greeting3 = pHelp->text;

	if ( !str_cmp( pHelp->keyword, "greeting4" ) )
	    help_greeting4 = pHelp->text;

	if ( !str_cmp( pHelp->keyword, "greeting5" ) )
	    help_greeting5 = pHelp->text;

	if ( help_first == NULL )
	    help_first = pHelp;
	if ( help_last  != NULL )
	    help_last->next = pHelp;

	help_last	= pHelp;
	pHelp->next	= NULL;
	top_help++;
    }

    return;
}



/*
 * Snarf a mob section.
 */
void load_mobiles( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_mobiles: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_mob_index( vnum ) != NULL )
	{
	    bug( "Load_mobiles: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pMobIndex			= (MOB_INDEX_DATA *) alloc_perm( sizeof(*pMobIndex) );
	pMobIndex->vnum			= vnum;
	pMobIndex->player_name		= fread_string( fp );
	pMobIndex->short_descr		= fread_string( fp );
	pMobIndex->long_descr		= fread_string( fp );
	pMobIndex->description		= fread_string( fp );

	pMobIndex->long_descr[0]	= UPPER(pMobIndex->long_descr[0]);
	pMobIndex->description[0]	= UPPER(pMobIndex->description[0]);

	pMobIndex->act			= fread_number( fp ) | ACT_IS_NPC;
	pMobIndex->affected_by		= fread_number( fp );
	pMobIndex->pShop		= NULL;
	pMobIndex->alignment		= fread_number( fp );
	letter				= fread_letter( fp );
	pMobIndex->level		= fread_number( fp );
	pMobIndex->level = UMIN( 50, pMobIndex->level );

	/*
	 * The unused stuff is for imps who want to use the old-style
	 * stats-in-files method.
	 */
  if ( letter != 'P' )
  {
	pMobIndex->hitroll		= fread_number( fp );	/* Unused */
	pMobIndex->ac			= fread_number( fp );	/* Unused */
	pMobIndex->hitnodice		= fread_number( fp );	/* Unused */
	/* 'd'		*/		  fread_letter( fp );	/* Unused */
	pMobIndex->hitsizedice		= fread_number( fp );	/* Unused */
	/* '+'		*/		  fread_letter( fp );	/* Unused */
	pMobIndex->hitplus		= fread_number( fp );	/* Unused */
	pMobIndex->damnodice		= fread_number( fp );	/* Unused */
	/* 'd'		*/		  fread_letter( fp );	/* Unused */
	pMobIndex->damsizedice		= fread_number( fp );	/* Unused */
	/* '+'		*/		  fread_letter( fp );	/* Unused */
	pMobIndex->damplus		= fread_number( fp );	/* Unused */
	pMobIndex->gold			= fread_number( fp );	/* Unused */
	/* xp can't be used! */		  fread_number( fp );	/* Unused */
	/* position	*/		  fread_number( fp );	/* Unused */
	/* start pos	*/		  fread_number( fp );	/* Unused */
  }

	/*
	 * Back to meaningful values.
	 */
	pMobIndex->sex			= fread_number( fp );

/* dont need this stopping boots, will assume good areas */
/*
	if ( letter != 'S' )
	{
	    bug( "Load_mobiles: vnum %d non-S.", vnum );
	    exit( 1 );
	}
*/

	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
    }

    return;
}



/*
 * Snarf an obj section.
 */
void load_objects( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_obj_index( vnum ) != NULL )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex			= (OBJ_INDEX_DATA *) alloc_perm( sizeof(*pObjIndex) );
	pObjIndex->vnum			= vnum;
	pObjIndex->name			= fread_string( fp );
	pObjIndex->short_descr		= fread_string( fp );
	pObjIndex->description		= fread_string( fp );
	/* Action description */	  fread_string( fp );

	pObjIndex->short_descr[0]	= LOWER(pObjIndex->short_descr[0]);
	pObjIndex->description[0]	= UPPER(pObjIndex->description[0]);

	pObjIndex->item_type		= fread_number( fp );
	pObjIndex->extra_flags		= fread_number( fp );
	pObjIndex->wear_flags		= fread_number( fp );
	pObjIndex->value[0]		= fread_number( fp );
	pObjIndex->value[1]		= fread_number( fp );
	pObjIndex->value[2]		= fread_number( fp );
	pObjIndex->value[3]		= fread_number( fp );
	pObjIndex->weight		= fread_number( fp );
	pObjIndex->cost			= fread_number( fp );
	pObjIndex->load_percent		= fread_number( fp );

	if ( pObjIndex->item_type == ITEM_KEY )
	    pObjIndex->load_percent = 100;
	else if ( pObjIndex->item_type == ITEM_DRINK_CON )
	    pObjIndex->load_percent = 3;

	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= (AFFECT_DATA *) alloc_perm( sizeof(*paf) );
		paf->type		= -1;
		paf->duration		= -1;
		paf->location		= fread_number( fp );
		paf->modifier		= fread_number( fp );
		paf->bitvector		= 0;
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
	    }

	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= (EXTRA_DESCR_DATA *) alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
	    }

	    else
	    {
		ungetc( letter, fp );
		break;
	    }
	}

	/*
	 * Translate spell "slot numbers" to internal "skill numbers."
	 */
	switch ( pObjIndex->item_type )
	{
	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
	    pObjIndex->value[1] = slot_lookup( pObjIndex->value[1] );
	    pObjIndex->value[2] = slot_lookup( pObjIndex->value[2] );
	    pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
	    pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
	    break;

	case ITEM_QUIVER:
	    pObjIndex->value[1] = slot_lookup( pObjIndex->value[1] );
	    break;
	case ITEM_TRAP:
	    pObjIndex->value[0] = slot_lookup( pObjIndex->value[0] );
	    break;
	}

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;
	top_obj_index++;
    }

    return;
}



/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
    RESET_DATA *pReset;

    if ( area_last == NULL )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	char letter;

	if ( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	pReset		= (RESET_DATA *) alloc_perm( sizeof(*pReset) );
	pReset->command	= letter;
	/* if_flag */	  fread_number( fp );
	pReset->arg1	= fread_number( fp );
	pReset->arg2	= fread_number( fp );
	pReset->arg3	= (letter == 'G' || letter == 'R')
			    ? 0 : fread_number( fp );
			  fread_to_eol( fp );
        memset( pReset->instance, 0, sizeof( pReset->instance ) );

	if ( area_last->reset_first == NULL )
	    area_last->reset_first	= pReset;
	if ( area_last->reset_last  != NULL )
	    area_last->reset_last->next	= pReset;

        memset( pReset->instance, 0, sizeof( pReset->instance ) );
	    
	area_last->reset_last	= pReset;
	pReset->next		= NULL;
	top_reset++;
    }

    return;
}



/*
 * Snarf a room section.
 */
void load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;
    unsigned int instance;

    if ( area_last == NULL )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int door;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_room_index( vnum ) != NULL )
	{
	    bug( "Load_rooms: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pRoomIndex			= (ROOM_INDEX_DATA *) alloc_perm( sizeof(*pRoomIndex) );
	pRoomIndex->people		= NULL;
	pRoomIndex->contents		= NULL;
	pRoomIndex->extra_descr		= NULL;
	pRoomIndex->area		= area_last;
	pRoomIndex->vnum		= vnum;
	pRoomIndex->name		= fread_string( fp );
	pRoomIndex->description		= fread_string( fp );
	/* Area number */		  fread_number( fp );
	pRoomIndex->room_flags		= fread_number( fp );
	pRoomIndex->sector_type		= fread_number( fp );
	pRoomIndex->light		= 0;
	pRoomIndex->last_exit_dir	= -1;
	pRoomIndex->last_char		= NULL;
	for ( door = 0; door <= 5; door++ )
	    pRoomIndex->exit[door] = NULL;

        pRoomIndex->bfs_next       = NULL;
        pRoomIndex->bfs_identifier = 0;
        pRoomIndex->bfs_direction  = -1;

	for ( ; ; )
	{
	    letter = fread_letter( fp );

	    if ( letter == 'S' )
		break;

	    if ( letter == 'D' )
	    {
		EXIT_DATA *pexit;
		int locks;

		door = fread_number( fp );
		if ( door < 0 || door > 5 )
		{
		    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
		    exit( 1 );
		}

		pexit			= (EXIT_DATA *) alloc_perm( sizeof(*pexit) );
		pexit->description	= fread_string( fp );
		pexit->keyword		= fread_string( fp );
		memset( pexit->exit_info, 0, sizeof( pexit->exit_info ) );
		locks			= fread_number( fp );
		pexit->key		= fread_number( fp );
		pexit->vnum		= fread_number( fp );


                for ( instance = 0; instance < NUM_PHASES; instance++ )
                {
                  switch ( locks )
                  {
                    case 1: pexit->exit_info[instance] = EX_ISDOOR;                break;
                    case 2: pexit->exit_info[instance] = EX_ISDOOR | EX_PICKPROOF; break;
                  }
                }

		pRoomIndex->exit[door]	= pexit;
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= (EXTRA_DESCR_DATA *) alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pRoomIndex->extra_descr;
		pRoomIndex->extra_descr	= ed;
		top_ed++;
	    }
	    else
	    {
		bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
		exit( 1 );
	    }
	}

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;
    }

    return;
}



/*
 * Snarf a shop section.
 */
void load_shops( FILE *fp )
{
    SHOP_DATA *pShop;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int iTrade;

	pShop			= (SHOP_DATA *) alloc_perm( sizeof(*pShop) );
	pShop->keeper		= fread_number( fp );
	if ( pShop->keeper == 0 )
	    break;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[iTrade]	= fread_number( fp );
	pShop->profit_buy	= fread_number( fp );
	pShop->profit_sell	= fread_number( fp );
	pShop->open_hour	= fread_number( fp );
	pShop->close_hour	= fread_number( fp );
				  fread_to_eol( fp );
	pMobIndex		= get_mob_index( pShop->keeper );
	pMobIndex->pShop	= pShop;

	if ( shop_first == NULL )
	    shop_first = pShop;
	if ( shop_last  != NULL )
	    shop_last->next = pShop;

	shop_last	= pShop;
	pShop->next	= NULL;
	top_shop++;
    }

    return;
}



/*
 * Snarf spec proc declarations.
 */
void load_specials( FILE *fp )
{
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	OBJ_INDEX_DATA *pObjIndex;
	ROOM_INDEX_DATA *pRoomIndex;
	char letter;

	switch ( letter = fread_letter( fp ) )
	{
	default:
	    bug( "Load_specials: letter '%c' not *MS.", letter );
	    exit( 1 );

	case 'S':
	    return;

	case '*':
	    break;

	case 'M':
	    pMobIndex		= get_mob_index	( fread_number ( fp ) );
	    pMobIndex->spec_fun	= spec_lookup	( fread_word   ( fp ) );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;

	case 'I':
	    pMobIndex		= get_mob_index	( fread_number ( fp ) );
	    pMobIndex->interactive	= interactive_lookup	( fread_word   ( fp ) );
	    if ( pMobIndex->interactive == 0 )
	    {
		bug( "Load_specials: 'I': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;

	case 'W':
            pObjIndex		= get_obj_index (fread_number ( fp ) );
            pObjIndex->wear_fun	= wear_fun_lookup	(fread_word ( fp ) );

	    if ( pObjIndex->wear_fun == 0 )
	    {
		bug( "Load_specials: 'W': vnum %d.", pObjIndex->vnum );
		exit( 1 );
	    }
	    break;

	case 'O':
            pObjIndex		= get_obj_index (fread_number ( fp ) );
            pObjIndex->special	= object_fun_lookup	(fread_word ( fp ) );

	    if ( pObjIndex->special == 0 )
	    {
		bug( "Load_specials: 'O': vnum %d.", pObjIndex->vnum );
		exit( 1 );
	    }
	    break;

	case 'R':
            pRoomIndex		= get_room_index (fread_number ( fp ) );
            pRoomIndex->special	= room_fun_lookup	(fread_word ( fp ) );

	    if ( pRoomIndex->special == 0 )
	    {
		bug( "Load_specials: 'R': vnum %d.", pRoomIndex->vnum );
		exit( 1 );
	    }
	    break;
	}

	fread_to_eol( fp );
    }
}



/*
 * Snarf notes file.
 */
void load_notes( void )
{
    FILE *fp;
    NOTE_DATA *pnotelast;

    if ( ( fp = fopen( NOTE_FILE, "r" ) ) == NULL )
	return;

    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	char letter;

	do
	{
	    letter = getc( fp );
	    if ( feof(fp) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace(letter) );
	ungetc( letter, fp );

	pnote		= (NOTE_DATA *) alloc_perm( sizeof(*pnote) );

	if ( str_cmp( fread_word( fp ), "sender" ) )
	    break;
	pnote->sender	= fread_string( fp );

	if ( str_cmp( fread_word( fp ), "date" ) )
	    break;
	pnote->date	= fread_string( fp );

	if ( str_cmp( fread_word( fp ), "to" ) )
	    break;
	pnote->to_list	= fread_string( fp );

	if ( str_cmp( fread_word( fp ), "subject" ) )
	    break;
	pnote->subject	= fread_string( fp );

	if ( str_cmp( fread_word( fp ), "text" ) )
	    break;
	pnote->text	= fread_string( fp );

	if ( note_list == NULL )
	    note_list		= pnote;
	else
	    pnotelast->next	= pnote;

	pnotelast	= pnote;
    }

    strcpy( strArea, NOTE_FILE );
    fpArea = fp;
    bug( "Load_notes: bad key word.", 0 );
    exit( 1 );
    return;
}



/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits( void )
{
    ROOM_INDEX_DATA *pRoomIndex;
    EXIT_DATA *pexit;
    int iHash;
    int door;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    bool fexit;

	    fexit = FALSE;
	    for ( door = 0; door <= 5; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[door] ) != NULL )
		{
		    fexit = TRUE;
		    if ( pexit->vnum <= 0 )
			pexit->to_room = NULL;
		    else
			pexit->to_room = get_room_index( pexit->vnum );
		}
	    }

	    if ( !fexit )
		SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
	}
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
/* this is _not_ a bug */
/*
	    for ( door = 0; door <= 5; door++ )
	    {
		if ( ( pexit     = pRoomIndex->exit[door]       ) != NULL
		&&   ( to_room   = pexit->to_room               ) != NULL
		&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
		&&   pexit_rev->to_room != pRoomIndex )
		{
		    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
			pRoomIndex->vnum, door,
			to_room->vnum,    rev_dir[door],
			(pexit_rev->to_room == NULL)
			    ? 0 : pexit_rev->to_room->vnum );
		    bug( buf, 0 );
		}
	    }
*/
	}
    }

    return;
}



/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
  AREA_DATA *pArea;

  for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
  {
    /* Currently only resetting phase 0.  Eventually update
     * other zones as well. */
    pArea->lifespan[WAR_NUM_DEAD]++;
    pArea->lifespan[WAR_NUM_MAIN]++;

    if (pArea->lifespan[WAR_NUM_DEAD] >= 3 )
    {
      reset_area( pArea, WAR_NUM_DEAD );
      pArea->lifespan[WAR_NUM_DEAD] = number_range( 0, 3 );
    }

    if (pArea->lifespan[WAR_NUM_MAIN] >= 20 )
    {
      reset_area( pArea, WAR_NUM_MAIN );
      pArea->lifespan[WAR_NUM_MAIN] = number_range( 0, 3 );
    }
  }
  return;
}



/*
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea, unsigned int phase )
{
  RESET_DATA *pReset;
  CHAR_DATA *mob;
  OBJ_DATA *last_obj = NULL;
  bool last;
  extern int war_flags;

  mob 	= NULL;
  last	= TRUE;

  if ( phase >= NUM_PHASES )
  {
    bug( "Reset_area: bad phase %d.", phase );
    return;
  }

  for ( pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next )
  {
    ROOM_INDEX_DATA *pRoomIndex;
    MOB_INDEX_DATA *pMobIndex;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_INDEX_DATA *pObjToIndex;
    EXIT_DATA *pexit;
    OBJ_DATA *obj;
    int load_chance = 0;

    switch ( pReset->command )
    {
      default:
	bug( "Reset_area: bad command %c.", pReset->command );
	break;

      case 'M':
	if ( ( pMobIndex = get_mob_index( pReset->arg1 ) ) == NULL )
	{
	  bug( "Reset_area: 'M': bad vnum %d.", pReset->arg1 );
	  continue;
	}

	if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	{
	  bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
	  continue;
	}

        if ( pReset->instance[phase] != NULL )
        {
	  last = FALSE;
          mob = NULL;
	  break;
        }

	mob = create_mobile( pMobIndex, phase );
        mob->pReset = pReset;
        pReset->instance[phase] = mob;

        check_random_equip( mob );

	char_to_room( mob, pRoomIndex );
	last  = TRUE;
	break;

      case 'O':
	if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	{
	  bug( "Reset_area: 'O': bad vnum %d.", pReset->arg1 );
	  continue;
	}

	if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	{
	  bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
	  continue;
	}

        /* This makes items placed in containers can load if
         * previously taken. */
        if ( pReset->instance[phase] != NULL )
        {
	  last = TRUE;
          last_obj = (OBJ_DATA *) pReset->instance[phase];
	  continue;
        }

        if ( pObjIndex->item_type == ITEM_FOUNTAIN
	&& !IS_SET(war_flags, WAR_FLAG_FOUNTAINS ) )
	{
	  last = FALSE;
	  continue;
        }

        if( pObjIndex->item_type == ITEM_KEY )
	  pObjIndex->load_percent = 100;

        load_chance = pObjIndex->load_percent;

        if ( IS_SET( war_flags, WAR_FLAG_LOADALL )
          && pObjIndex->item_type != ITEM_DRINK_CON )
          load_chance = 100;

        if ( pRoomIndex->vnum == 1 )
          load_chance = 100;

        if (number_percent() > load_chance )
        {
	  last = FALSE;
          last_obj = NULL;
	  continue;
        }

	obj       = create_object( pObjIndex, phase );
	obj_to_room( obj, pRoomIndex );

        obj->pReset = pReset;
        pReset->instance[phase] = obj;

	last = TRUE;
        last_obj = obj;
	break;

      case 'P':
        if ( pReset->instance[phase] != NULL )
        {
	  continue;
        }

	if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	{
	  bug( "Reset_area: 'P': bad vnum %d.", pReset->arg1 );
	  continue;
	}

        /* Can't put an item if previous item doesn't exist. */
        if ( last == FALSE || last_obj == NULL
          || last_obj->pIndexData->vnum != pReset->arg3 )
        {
          continue;
        }

        if ( pObjIndex->item_type == ITEM_FOUNTAIN
          && !IS_SET(war_flags, WAR_FLAG_FOUNTAINS ) )
	{
	  continue;
        }

        if( pObjIndex->item_type == ITEM_KEY )
	  pObjIndex->load_percent = 100;

        load_chance = pObjIndex->load_percent;

        if ( IS_SET( war_flags, WAR_FLAG_LOADALL ) )
          load_chance = 100;

        if (number_percent() > load_chance )
        {
          continue;
        }

        obj = create_object( pObjIndex, phase );
        obj_to_obj( obj, last_obj );

        obj->pReset = pReset;
        pReset->instance[phase] = obj;

        break;

	case 'G':
	case 'E':
	    if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( !last )
		continue;

	    if ( mob == NULL )
	    {
		bug( "Reset_area: 'E' or 'G': null mob for vnum %d.",
		    pReset->arg1 );
		last = FALSE;
		continue;
	    }

	    if ( mob->pIndexData->pShop != NULL )
	    {
		obj = create_object( pObjIndex, phase );
		if(obj->item_type == ITEM_QUIVER)
			make_random_quiver(obj);
		if(obj->item_type == ITEM_TRAP)
			make_random_trap(obj);
		SET_BIT( obj->extra_flags, ITEM_INVENTORY );
	    }
	    else
	    {
		if (number_percent() > pObjIndex->load_percent
		&& ( !IS_SET(war_flags, WAR_FLAG_LOADALL )
                  || pObjIndex->item_type == ITEM_DRINK_CON ) )
                  continue;

		obj = create_object( pObjIndex, phase );
	    }
	    obj_to_char( obj, mob );
	    if ( pReset->command == 'E' )
		equip_char( mob, obj, pReset->arg3 );
	    last = TRUE;
	    break;

	case 'D':
	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'D': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL )
		continue;

	    switch ( pReset->arg3 )
	    {
	    case 0:
		REMOVE_BIT( pexit->exit_info[phase], EX_CLOSED );
		REMOVE_BIT( pexit->exit_info[phase], EX_LOCKED );
		break;

	    case 1:
		SET_BIT(    pexit->exit_info[phase], EX_CLOSED );
		REMOVE_BIT( pexit->exit_info[phase], EX_LOCKED );
		break;

	    case 2:
		SET_BIT(    pexit->exit_info[phase], EX_CLOSED );
		SET_BIT(    pexit->exit_info[phase], EX_LOCKED );
		break;
	    }

	    last = TRUE;
	    break;

	case 'R':
	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    {
		int d0;
		int d1;

		for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
		{
		    d1                   = number_range( d0, pReset->arg2-1 );
		    pexit                = pRoomIndex->exit[d0];
		    pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
		    pRoomIndex->exit[d1] = pexit;
		}
	    }
	    break;
	}
    }

    return;
}



/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex, unsigned int phase )
{
    CHAR_DATA *mob;

    if ( pMobIndex == NULL )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    if ( char_free == NULL )
    {
	mob		= (CHAR_DATA *) alloc_perm( sizeof(*mob) );
    }
    else
    {
	mob		= char_free;
	char_free	= char_free->next;
    }

    clear_char( mob );
    mob->pIndexData	= pMobIndex;
    mob->pReset         = NULL;
    mob->phase          = phase;

    mob->name		= pMobIndex->player_name;
    mob->short_descr	= pMobIndex->short_descr;
    mob->long_descr	= pMobIndex->long_descr;
    mob->description	= pMobIndex->description;
    mob->spec_fun	= pMobIndex->spec_fun;
    mob->interactive	= pMobIndex->interactive;

    mob->level		= number_fuzzy( pMobIndex->level );
    mob->act		= pMobIndex->act;
    mob->affected_by	= pMobIndex->affected_by;
    mob->alignment	= pMobIndex->alignment;


/* economy change */
/*
    mob->gold           = pMobIndex->gold;
*/
    mob->gold		= mob->level;
    mob->sex		= pMobIndex->sex;
    mob->armor		= stat_table[mob->level].armor * 10;

    mob->max_hit	= dice( 1, stat_table[mob->level].hit_die)
			  + stat_table[mob->level].hit_constant;

    mob->hit		= mob->max_hit;
	    
    /*
     * Insert in list.
     */
    mob->next		= char_list;
    char_list		= mob;
    pMobIndex->count++;
    return mob;
}



/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, unsigned int phase )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;

    if ( pObjIndex == NULL )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	exit( 1 );
    }

    if ( obj_free == NULL )
    {
	obj		= (OBJ_DATA *) alloc_perm( sizeof(*obj) );
    }
    else
    {
	obj		= obj_free;
	obj_free	= obj_free->next;
    }

    *obj		= obj_zero;
    obj->pIndexData	= pObjIndex;
    obj->pReset         = NULL;
    obj->in_room	= NULL;
    obj->killer		= NULL;
    obj->wear_loc	= -1;

    obj->name		= pObjIndex->name;
    obj->short_descr	= pObjIndex->short_descr;
    obj->description	= pObjIndex->description;
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags	= pObjIndex->extra_flags;
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->value[0]	= pObjIndex->value[0];
    obj->value[1]	= pObjIndex->value[1];
    obj->value[2]	= pObjIndex->value[2];
    obj->value[3]	= pObjIndex->value[3];
    obj->weight		= pObjIndex->weight;
    obj->cost           = pObjIndex->cost;

/* and the wear function */
    obj->wear_fun	= pObjIndex->wear_fun;
    obj->special	= pObjIndex->special;

    obj->phase          = phase;

    /*
     * Mess with object properties.
     */
    switch ( obj->item_type )
    {
    default:
	bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
	break;

    case ITEM_LIGHT:
    case ITEM_TREASURE:
    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_FOOD:
    case ITEM_BOAT:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_QUIVER:
    case ITEM_TRAP:
	break;

    case ITEM_FOUNTAIN:
	break;

    case ITEM_SCROLL:
	if ( IS_SET( obj->extra_flags, ITEM_RENAME ) )
	    rename_object_random( obj, TABLE_SCROLL );
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	if ( IS_SET( obj->extra_flags, ITEM_RENAME ) )
	    rename_object_random( obj, TABLE_STAFF );
	break;

    case ITEM_WEAPON:
	break;

    case ITEM_GRENADE:
	break;

    case ITEM_ARMOR:
	break;

    case ITEM_POTION:
        SET_BIT( obj->wear_flags, ITEM_HOLD );
    case ITEM_PILL:
	obj->value[0]	= number_fuzzy( number_fuzzy( obj->value[0] ) );
	break;

    case ITEM_MONEY:
	obj->value[0]	= UMIN( obj->value[0], dice( 1, 100 ) );
	break;

    case ITEM_JEWELRY:
	if ( IS_SET( obj->extra_flags, ITEM_RENAME ) )
	    rename_object_random( obj, TABLE_JEWELRY );
	break;
    }

    obj->next		= object_list;
    object_list		= obj;
    pObjIndex->count++;

    return obj;
}



/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;

    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->logon			= current_time;
    ch->armor			= 100;
    ch->position		= POS_STANDING;
    ch->practice		= 0;
    ch->hit			= 20;
    ch->max_hit			= 20;
    ch->hit_regen_bonus		= 0;
    ch->mana			= 100;
    ch->max_mana		= 100;
    ch->mana_regen_bonus		= 0;
    ch->move_lag		= 0;
    ch->social.name = &str_empty[0];
    ch->social.charc = &str_empty[0];
    ch->social.vict = &str_empty[0];
    ch->social.room = &str_empty[0];
    ch->social.roomnv = &str_empty[0];
    ch->social.charnv = &str_empty[0];
    ch->resist_fire = 0;
    ch->resist_cold = 0;
    ch->resist_lightning = 0;
    ch->resist_physical = 0;
    ch->resist_magic = 0;
    ch->resist_drain = 0;
    ch->private_war = NULL;
    ch->hunting = NULL;
    return;
}


struct snoop_data *new_snoop_data( void )
{
    struct snoop_data *snoop;

    if ( snoop_free == NULL )
    {
	snoop = (struct snoop_data *) alloc_perm( sizeof( *snoop ) );
    }
    else
    {
	snoop = snoop_free;
	snoop_free = snoop->next;
    }

    return snoop;
}

void free_snoop( struct snoop_data *snoop )
{
    snoop->next = snoop_free;
    snoop_free = snoop;
}


/*
 * Free a character.
 */
void free_char( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;
    int i;
    char str_free_char[10] = "free_char";

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	extract_obj( obj, str_free_char );
    }

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	affect_remove( ch, paf );
    }

    free_string( ch->name		);
    free_string( ch->short_descr	);
    free_string( ch->long_descr		);
    free_string( ch->description	);
    free_string( ch->social.name	);
    free_string( ch->social.charc	);
    free_string( ch->social.vict	);
    free_string( ch->social.room	);
    free_string( ch->social.roomnv	);
    free_string( ch->social.charnv	);

    if ( ch->pcdata != NULL )
    {
	free_string( ch->pcdata->pwd		);
	free_string( ch->pcdata->bamfin		);
	free_string( ch->pcdata->bamfout	);
	free_string( ch->pcdata->title		);
        for ( i = 0; i < MAX_CLASS; i++ )
        {
          ch->pcdata->class_kills[i] = 0;
          ch->pcdata->class_deaths[i] = 0;
        }
	ch->pcdata->next = pcdata_free;
	ch->pcdata->ratings_victims = 0;
	ch->pcdata->ratings_killers = 0;
	ch->pcdata->rated_kills = 0;
	ch->pcdata->rated_deaths = 0;
	pcdata_free      = ch->pcdata;
    }

    ch->next	     = char_free;
    char_free	     = ch;
    return;
}



/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed != NULL; ed = ed->next )
    {
	if ( is_name( name, ed->keyword ) )
	    return ed->description;
    }
    return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
	  pMobIndex != NULL;
	  pMobIndex  = pMobIndex->next )
    {
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
	  pObjIndex != NULL;
	  pObjIndex  = pObjIndex->next )
    {
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_obj_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
    ROOM_INDEX_DATA *pRoomIndex;

    if ( vnum < 0 )
      return NULL;

    for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
	  pRoomIndex != NULL;
	  pRoomIndex  = pRoomIndex->next )
    {
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    return c;
}



/*
 * Read a number from a file.
 */
int fread_number( FILE *fp )
{
    int number;
    bool sign;
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit(c) )
    {
	bug( "Fread_number: bad format.", 0 );
	exit( 1 );
    }

    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}



/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 * This function takes 40% to 50% of boot-up time.
 */
char *fread_string( FILE *fp )
{
    char *plast;
    char c;

    plast = top_string + sizeof(char *);
    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
    {
	bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
	exit( 1 );
    }

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    if ( ( *plast++ = c ) == '~' )
	return &str_empty[0];

    for ( ;; )
    {
	/*
	 * Back off the char type lookup,
	 *   it was too dirty for portability.
	 *   -- Furey
	 */
	switch ( *plast = getc( fp ) )
	{
	default:
	    plast++;
	    break;

	case EOF:
	    bug( "Fread_string: EOF", 0 );
	    exit( 1 );
	    break;

	case '\n':
	    plast++;
	    *plast++ = '\r';
	    break;

	case '\r':
	    break;

	case '~':
	    plast++;
	    {
		union
		{
		    char *	pc;
		    char	rgc[sizeof(char *)];
		} u1;
		int ic;
		int iHash;
		char *pHash;
		char *pHashPrev;
		char *pString;

		plast[-1] = '\0';
		iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
		for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
		{
		    for ( ic = 0; ic < (int) sizeof(char *); ic++ )
			u1.rgc[ic] = pHash[ic];
		    pHashPrev = u1.pc;
		    pHash    += sizeof(char *);

		    if ( top_string[sizeof(char *)] == pHash[0]
		    &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
			return pHash;
		}

		if ( fBootDb )
		{
		    pString		= top_string;
		    top_string		= plast;
		    u1.pc		= string_hash[iHash];
		    for ( ic = 0; ic < (int) sizeof(char *); ic++ )
			pString[ic] = u1.rgc[ic];
		    string_hash[iHash]	= pString;

		    nAllocString += 1;
		    sAllocString += top_string - pString;
		    return pString + sizeof(char *);
		}
		else
		{
		    return str_dup( top_string + sizeof(char *) );
		}
	    }
	}
    }
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( c != '\n' && c != '\r' );

    do
    {
	c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp )
{
    static char word[MAX_INPUT_LENGTH];
    char *pword;
    char cEnd;

    do
    {
	cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( cEnd == '\'' || cEnd == '"' )
    {
	pword   = word;
    }
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }

    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }

    bug( "Fread_word: word too long.", 0 );
    exit( 1 );
    return NULL;
}



/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
    void *pMem;
    int iList;

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[iList] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Alloc_mem: size %d too large.", sMem );
	exit( 1 );
    }

    if ( rgFreeList[iList] == NULL )
    {
	pMem		  = alloc_perm( rgSizeList[iList] );
    }
    else
    {
	pMem              = rgFreeList[iList];
	rgFreeList[iList] = * ((void **) rgFreeList[iList]);
    }

    return pMem;
}



/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
    int iList;

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[iList] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Free_mem: size %d too large.", sMem );
	exit( 1 );
    }

    * ((void **) pMem) = rgFreeList[iList];
    rgFreeList[iList]  = pMem;

    return;
}



/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
    static char *pMemPerm;
    static int iMemPerm;
    void *pMem;

    while ( sMem % sizeof(long) != 0 )
	sMem++;
    if ( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( ( pMemPerm = (char *) calloc( 1, MAX_PERM_BLOCK ) ) == NULL )
	{
	    perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
}



/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *str_dup( const char *str )
{
    char *str_new;

    if ( str[0] == '\0' )
	return &str_empty[0];

    if ( str >= string_space && str < top_string )
	return (char *) str;

    str_new = (char *) alloc_mem( strlen(str) + 1 );
    strcpy( str_new, str );
    return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void free_string( char *pstr )
{
    if ( pstr == NULL
    ||   pstr == &str_empty[0]
    || ( pstr >= string_space && pstr < top_string ) )
	return;

    free_mem( pstr, strlen(pstr) + 1 );
    return;
}


#define ATYPE_NORMAL   0
#define ATYPE_ENABLED  1
#define ATYPE_DISABLED 2

bool area_match( AREA_DATA *pArea, int type )
{
  if ( IS_SET( pArea->area_flags, AREA_NOSHOW ) )
    return FALSE;

  if ( type == ATYPE_NORMAL )
    return TRUE;

  if ( type == ATYPE_ENABLED )
  {
    if ( !IS_SET( pArea->area_flags, AREA_DISABLED ) )
      return TRUE;
    else
      return FALSE;
  }

  if ( type == ATYPE_DISABLED )
  {
    if ( IS_SET( pArea->area_flags, AREA_DISABLED ) )
      return TRUE;
    else
      return FALSE;
  }

  return TRUE;
}

void do_areas( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  AREA_DATA *pArea1;
  AREA_DATA *pArea2;
  int iArea = 0;
  int iAreaHalf;
  int type = ATYPE_NORMAL;

  one_argument( argument, arg );
  if ( arg[0] == 'e' || arg[0] == 'E' )
  {
    send_to_char( "Enabled areas:\n\r", ch );
    type = ATYPE_ENABLED;
  }
  else if ( arg[0] == 'd' || arg[0] == 'D' )
  {
    send_to_char( "Disabled areas:\n\r", ch );
    type = ATYPE_DISABLED;
  }

  for ( pArea1 = area_first; pArea1 != NULL; pArea1 = pArea1->next )
  {
    if ( area_match( pArea1, type ) )
      iArea++;
  }

  iAreaHalf = ( iArea + 1 ) / 2;

  pArea1    = area_first;
  pArea2    = area_first;

  iArea = 0;
  for ( pArea2 = area_first; pArea2 != NULL; pArea2 = pArea2->next )
  {
    if ( area_match( pArea2, type ) )
      iArea++;

    if ( iArea > iAreaHalf )
      break;
  }

  pArea1 = area_first;

  for ( iArea = 0; iArea < iAreaHalf; iArea++ )
  {
    while ( !area_match( pArea1, type ) )
      pArea1 = pArea1->next;

    sprintf( buf, "%-9s %-25s %3d ",
      pArea1->author, pArea1->name, pArea1->access_percent );
    send_to_char( buf, ch );

    if ( pArea2 != NULL )
    {
        sprintf( buf, "%-9s %-25s %3d",
          pArea2->author, pArea2->name, pArea2->access_percent );
        send_to_char( buf, ch );
    }

    send_to_char( "\n\r", ch );

    pArea1 = pArea1->next;

    if ( pArea2 != NULL )
    do
    {
      pArea2 = pArea2->next;
    } while ( pArea2 != NULL && !area_match( pArea2, type ) );
  }

  return;
}



void do_memory( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "Affects %5d\n\r", top_affect    ); send_to_char( buf, ch );
    sprintf( buf, "Areas   %5d\n\r", top_area      ); send_to_char( buf, ch );
    sprintf( buf, "ExDes   %5d\n\r", top_ed        ); send_to_char( buf, ch );
    sprintf( buf, "Exits   %5d\n\r", top_exit      ); send_to_char( buf, ch );
    sprintf( buf, "Helps   %5d\n\r", top_help      ); send_to_char( buf, ch );
    sprintf( buf, "Mobs    %5d\n\r", top_mob_index ); send_to_char( buf, ch );
    sprintf( buf, "Objs    %5d\n\r", top_obj_index ); send_to_char( buf, ch );
    sprintf( buf, "Resets  %5d\n\r", top_reset     ); send_to_char( buf, ch );
    sprintf( buf, "Rooms   %5d\n\r", top_room      ); send_to_char( buf, ch );
    sprintf( buf, "Shops   %5d\n\r", top_shop      ); send_to_char( buf, ch );

    sprintf( buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
	nAllocString, sAllocString, MAX_STRING );
    send_to_char( buf, ch );

    sprintf( buf, "Perms   %5d blocks  of %7d bytes.\n\r",
	nAllocPerm, sAllocPerm );
    send_to_char( buf, ch );

    return;
}



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch ( number_bits( 2 ) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}



/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int power;
    int number;

    if ( ( to = to - from + 1 ) <= 1 )
	return from;

    for ( power = 2; power < to; power <<= 1 )
	;

    while ( ( number = number_mm( ) & (power - 1) ) >= to )
	;

    return from + number;
}



/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    while ( ( percent = number_mm( ) & (128-1) ) > 99 )
	;

    return 1 + percent;
}



/*
 * Generate a random door.
 */
int number_door( void )
{
    int door;

    while ( ( door = number_mm( ) & (8-1) ) > 5 )
	;

    return door;
}



int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}



/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static	int	rgiState[2+55];

void init_mm( )
{
    int *piState;
    int iState;

    piState	= &rgiState[2];

    piState[-2]	= 55 - 55;
    piState[-1]	= 55 - 24;

    piState[0]	= ((int) current_time) & ((1 << 30) - 1);
    piState[1]	= 1;
    for ( iState = 2; iState < 55; iState++ )
    {
	piState[iState] = (piState[iState-1] + piState[iState-2])
			& ((1 << 30) - 1);
    }
    return;
}



int number_mm( void )
{
    int *piState;
    int iState1;
    int iState2;
    int iRand;

    piState		= &rgiState[2];
    iState1	 	= piState[-2];
    iState2	 	= piState[-1];
    iRand	 	= (piState[iState1] + piState[iState2])
			& ((1 << 30) - 1);
    piState[iState1]	= iRand;
    if ( ++iState1 == 55 )
	iState1 = 0;
    if ( ++iState2 == 55 )
	iState2 = 0;
    piState[-2]		= iState1;
    piState[-1]		= iState2;
    return iRand >> 6;
}



/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    switch ( size )
    {
    case 0: return 0;
    case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );

    return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_32 )
{
    return value_00 + level * (value_32 - value_00) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
	if ( *str == '~' )
	    *str = '-';
    }

    return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
/*	bug( "Str_cmp: null astr.", 0 );*/
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
/*	bug( "Strn_cmp: null astr.", 0 ); */
	return TRUE;
    }

    if ( bstr == NULL )
    {
/*	bug( "Strn_cmp: null bstr.", 0 ); */
	return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;
    int ichar;
    char c0;

    if ( ( c0 = LOWER(astr[0]) ) == '\0' )
	return FALSE;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);

    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
    {
	if ( c0 == LOWER(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;
    else
	return TRUE;
}



/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
}



/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    if ( IS_NPC(ch) || str[0] == '\0' )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
	perror( file );
	send_to_char( "Could not open the file!\n\r", ch );
    }
    else
    {
	fprintf( fp, "[%5d] %s: %s\n",
	    ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
    char buf[MAX_STRING_LENGTH];
    FILE *fp;

    if ( fpArea != NULL )
    {
	int iLine;
	int iChar;

	if ( fpArea == stdin )
	{
	    iLine = 0;
	}
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
	    {
		while ( getc( fpArea ) != '\n' )
		    ;
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf );

	if ( ( fp = fopen( "shutdown.txt", "a" ) ) != NULL )
	{
	    fprintf( fp, "[*****] %s\n", buf );
	    fclose( fp );
	}
    }

    strcpy( buf, "[*****] BUG: " );
    sprintf( buf + strlen(buf), str, param );
    log_string( buf );

    fclose( fpReserve );
    if ( ( fp = fopen( BUG_FILE, "a" ) ) != NULL )
    {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}



/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
    char *strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    fprintf( stderr, "%s :: %s~\n", strtime, str );
    return;
}

void mudlog( const char *str, int level )
{
	DESCRIPTOR_DATA *d;

	for( d = descriptor_list; d != NULL; d = d->next )
	{
		if(d->connected == CON_PLAYING && d->character != NULL &&IS_SET(d->character->act, PLR_NOTIFY ) 
		&& (get_trust(d->character) >= level ))
		{
			send_to_char("Notify> ", d->character );
			send_to_char( str, d->character );
			send_to_char( "\n\r", d->character );
			continue;
		}
	}
	return;
}


/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain( void )
{
    return;
}
