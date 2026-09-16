/*   random.c, a module for merc2.1 by Robert L. Peckham */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "global.h"

#define BASE_VNUM 9500
#define POTION_VNUM 9600
#define MAGIC_VNUM 9700
#define VARIOUS_VNUM 9800
#define LOC_BODY 0
#define LOC_ARM 1
#define LOC_LEG 2
#define LOC_FOOT 3
#define LOC_SHIELD 4
#define LOC_HEAD 5
#define LOC_HANDS 6
#define TYPE_LEATHER 0
#define TYPE_STUDDED 10
#define TYPE_SCALE 20
#define TYPE_CHAIN 30
#define TYPE_ELVEN 40
#define TYPE_BRONZE 50
#define TYPE_PLATE 60
#define TYPE_FIELD 70
#define MAX_ADJECTIVE 206
#define MAX_MATERIAL 80
#define MAX_PREC_ADJ 78
#define MAX_PREC_MAT 64
#define MAX_SCROLL 81

static char * const adjective_table [] =
{
	"burnt", "withered", "gnarled", "black", "serpentine", "shimmering",
	"glowing", "bloody", "scarred", "sparkling", "pale", 
/* 10 */
	"dull", "brilliant", "glittering", "dark", "white", "flickering",
	"fiery", "heavy", "crystalline", "neon", "common",
/* 21 */
	"ornate", "jewelled", "rainbow", "glinting", "curious", "long",
	"short", "old", "ancient", "thick", "forked", "spiraled",
/* 33 */
	"engraved", "runed", "throbbing", "pulsating", "mystical",
	"plain", "simple", "pointy", "blunt", "bent", "vibrating",
/* 44 */
	"twitching", "shadowy", "bumpy", "dirty", "smoky", "distinctive",
	"horned", "rusty", "flaming", "blazing", "freezing",
/* 55 */
	"twisted", "curved", "chromatic", "prismatic", "spiny", "thin",
	"shiny", "fractal",
/* 63 */

	"obscure", "mighty", "coiled", "tattered", "winged", "square", "round",
	"odd", "spiked", "jagged", "scaled", "tarnished", "noble",
/* 76 */
	"bright", "battered", "sturdy", "smooth", "rough", "cracked",
	"shattered", "vibrant", "iridescent", "slender", "mysterious",
/* 87 */
	"muddy", "oval", "intricate", "carved", "spectral", "grimy",
	"barbed", "sacred", "holy", "frisky", "random", "harmonic",
/* 99 */
	"tangled", "cloudy", "crushed", "brittle", "radiant", "drab",
	"murky", "blurry", "hazy", "illuminated", "burnished", "polished",
/* 111 */
	"colorless", "faint", "dim", "colorful", "vivid", "luminous",
	"bleak", "dingy", "filthy", "glazed", "glossy", "flaring", "sizzling",
/* 124 */
	"scorching", "shriveled", "stout", "glorious", "splendid",
	"magnificent", "superb", "imposing", "impressive", "majestic",
/* 134 */
	"stunning", "beautiful", "ravishing", "varnished", "veneered",
	"winding", "convoluted", "meandering", "crooked", "serrated",
/* 144 */
	"notched", "scored", "grim", "blistered", "occult", "knotted",
	"wavering", "unscathed", "quavering", "trembling", "massive",
/* 155 */
	"antique", "age", "ageless", "fine", "pronged", "tined", "prickly",
	"inscribed", "unadorned", "unembellished", "elaborate", "striking",
/* 167 */
	"arched", "bowed", "resounding", "sonorous", "raging", "furious",
	"smoldering", "fuming", "jarring", "merciless", "frosty",
/* 178 */
	"resilient", "flexible", "elastic", "unbreakable", "shrounded",
	"forceful", "potent", "delicate", "ragged", "frazzled", "wiry",
/* 189 */
	"sleek", "peculiar", "eccentric", "formidable", "dreadful",
	"stalwart", "frightful", "horrible", "shoddy", "jaded", "rugged",
/* 200 */
	"course", "crude", "unformed", "splintered", "fragmented", "ruinous"
/* 206 */
/* EDIT MAX_ADJECTIVE ABOVE IF ADD MORE */
};

static char * const material_table [] =
{
	"iron", "glass", "golden", "silver", "pearl", "diamond", "ruby",
	"sapphire", "emerald", "copper", "birch", "oak", "elm",
/* 12 */
	"pine", "steel", "platinum", "quartz", "bone", "ivory", "ebony",
	"redwood", "mithril", "ash", "malachite", "obsidian",
/* 24 */
	"onyx", "coral", "jade", "pearl", "electrum", "stone", "marble",
	"shell", "moonstone", "opal", "granite", "maple",
/* 36 */
	"adamantite", "adamantine", "ice", "eggshell", "flint", "lead",
	"pewter", "bark", "wax", "clay", "graphite", "cherrywood",
/* 48 */
	"leather", "dragon tooth", "antler", "nickel", "titanium", "brass",
	"amethyst", "wicker", "rose", "reed", "bamboo", "balsa",
/* 60 */
	"teak", "pyrite", "eucalyptus",
/* 63 */

	"vortex", "astral", "guardian", "chaos", "star", "vengeance",
	"minteral", "warp", "energy", "agate", "turquoise", "jasper",
/* 75 */
	"zircon", "amber", "aquamarine", "garnet", "topaz"
/* 80 */
/* EDIT MAX_MATERIAL ABOVE IF ADD MORE */
};


static char * const precious_adjective_table [] =
{
	"shimmering", "glowing", "sparkling", "pale", "dull",
	"brilliant", "glittering", "dark", "flickering", "fiery",
/* 9 */
	"neon", "ornate", "common", "rainbow", "glinting", "curious",
	"old", "ancient", "engraved", "runed", "throbbing", "pulsating",
/* 21 */
	"plain", "simple", "vibrating", "dirty", "rusty", "blazing",
	"chromatic", "thin", "thick", "fractal", "strange", "odd",
/* 33 */
	"tarnished", "noble", "bright", "battered", "cracked", "shattered",
	"vibrant", "iridescent", "intricate", "carved", "grimy", "brittle",
/* 45 */
	"radiant", "flashy", "scintillating", "twinkling", "dazzling",
	"dull", "blurry", "hazy", "illuminated", "burnished", "polished",
/* 56 */
	"faint", "dim", "vivid", "luminous", "glazed", "glossy", "flaring",
	"sizzling", "scorching", "splended", "glorious", "magnificent",
/* 68 */
	"superb", "impressive", "majestic", "stunning", "ravishing",
	"perfect", "antique", "grim", "aged", "delicate"
/* 78 */
};


static char * const precious_material_table [] =
{
	"glass", "golden", "gold", "silver", "pearl", "diamond",
	"ruby", "sapphire", "emerald", "copper", "steel", "iron",
/* 11 */
	"platinum", "quartz", "ivory", "mithril", "obsidian", "onyx",
	"coral", "jade", "peridot", "electrum", "marble", "moonstone",
/* 23 */
	"sea shell", "opal", "adamantite", "pewter", "titanium",
	"brass", "amethyst", "rose", "pyrite", "azurite", "agate",
/* 34 */
	"blue quartz", "lapis lazuli", "tiger eye", "turquoise",
	"bloodstone", "jasper", "rock crystal", "zircon", "amber",
/* 43 */
	"aquamarine", "garnet", "topaz", "tourmaline", "fire opal",
	"jacinth", "star ruby", "star sapphire", "malachite", "hematite",
/* 53 */
	"rhodochrosite", "carnelian", "chalcedony", "chrysoprase",
	"citrine", "sardonyx", "smoky quartz", "star rose quartz",
/* 61 */
	"alexandrite", "chrysoberyl", "spinel"
/* 64 */
};


static char * const scroll_table [] =
{
	"blackened", "stretched", "printed", "mass produced", "cursed",
	"tied", "encoded", "radiating", "dark", "darkened", "translucent",
/* 10 */
	"bark", "invisible ink", "hard", "flaked", "gilded", "modly",
	"vital", "frail", "pliable", "parchment", "tattered", "rolled",
/* 22 */
	"bound", "old", "strange", "runed", "encrypted", "bloodridden",
	"gold inlaid", "engraved", "crinkled", "folded", "painted",
/* 33 */
	"stone", "irrational", "speckled", "ivory", "brittle", "decomposing",
	"white", "yellow", "papyrus", "flayed skin", "pressed leather",
/* 44 */
	"dirty", "dusty", "thin", "scratched", "silver inlaid", "wax",
	"chalked", "faded", "crimped", "ordinary", "fried", "dragon",
/* 56 */
	"wet", "bronzed", "long", "torn", "heavy", "cotton", "sealed",
	"burnt", "half eaten", "bonded", "messy", "inked", "clay",
/* 69 */
	"wax stained", "crumpled", "washed out", "lamb skin", "odd",
	"fringed", "moss covered", "dry", "brazen", "fine printed",
/* 79 */
	"stained", "soiled"
/* 81 */
};

OBJ_DATA *create_random_armor( unsigned int phase )
{
	OBJ_INDEX_DATA *pObjIndex;
	int vnum = BASE_VNUM;
	int percent = number_percent();

	vnum += number_bits(3);

	if (percent <= 15)
		vnum += TYPE_LEATHER;
	else if (percent <= 25)
		vnum += TYPE_STUDDED;
	else if (percent <= 35)
		vnum += TYPE_SCALE;
	else if (percent <= 53)
		vnum += TYPE_CHAIN;
	else if (percent <= 55)
		vnum += TYPE_ELVEN;
	else if (percent <= 75)
		vnum += TYPE_BRONZE;
	else if (percent <= 90)
		vnum += TYPE_PLATE;
	else
		vnum += TYPE_FIELD;

	if ((pObjIndex = get_obj_index(vnum)) == NULL)
		return NULL;

	return create_object( pObjIndex, phase );
}


int calculate_saving_modifier( int bonus )
{
	int mod;

	mod = number_bits(2) - 2 + bonus;

	if (mod == 0)
		mod = -1;

	return mod;
}


int calculate_attrib_modifier( int bonus )
{
	int mod;

	mod = number_bits(4) - 6 + bonus;

	if (mod == 0)
		mod = 1;

	return mod;
}


int calculate_armor_modifier( int bonus )
{
	int mod;

	mod = number_bits(4) - 10 + bonus;

	if (mod == 0)
		mod = -1;

	return mod;
}


int calculate_stat_modifier( int bonus )
{
	int mod;

	mod = 1;

	while (!number_bits(3))
		mod++;

	if (!number_bits(3))
		mod *= -1;

	mod += bonus;

	if (mod == 0)
		mod = 1;

	return mod;
}



void add_random_magic( OBJ_DATA *obj )
{
	int modifier;
	AFFECT_DATA *paf;

	if (affect_free == NULL)
	{
		paf = (AFFECT_DATA *) alloc_perm( sizeof(*paf));
	}
	else
	{
		paf = affect_free;
		affect_free = affect_free->next;
	}

	paf->type = -1;
	paf->duration = -1;
	paf->bitvector = 0;

	modifier = number_percent();

	if (modifier <= 7)
		paf->location = APPLY_SAVING_PARA;
	else if (modifier <= 14)
		paf->location = APPLY_SAVING_ROD;
	else if (modifier <= 21)
		paf->location = APPLY_SAVING_PETRI;
	else if (modifier <= 28)
		paf->location = APPLY_SAVING_BREATH;
	else if (modifier <= 35)
		paf->location = APPLY_SAVING_SPELL;
	else if (modifier <= 40)
		paf->location = APPLY_HITROLL;
	else if (modifier <= 45)
		paf->location = APPLY_DAMROLL;
	else if (modifier <= 70)
		paf->location = APPLY_AC;
	else if (modifier <= 72)
		paf->location = APPLY_STR;
	else if (modifier <= 74)
		paf->location = APPLY_DEX;
	else if (modifier <= 76)
		paf->location = APPLY_INT;
	else if (modifier <= 78)
		paf->location = APPLY_WIS;
	else if (modifier <= 80)
		paf->location = APPLY_CON;
	else if (modifier <= 90)
		paf->location = APPLY_MANA;
	else
		paf->location = APPLY_HIT;

	switch(paf->location)
	{
		default:
			modifier = 0;
			break;
		case APPLY_SAVING_PARA:
		case APPLY_SAVING_ROD:
		case APPLY_SAVING_PETRI:
		case APPLY_SAVING_BREATH:
		case APPLY_SAVING_SPELL:
			modifier = calculate_saving_modifier( 0 );
			break;
		case APPLY_AC:
			modifier = calculate_armor_modifier( 0 );
			break;
		case APPLY_STR:
		case APPLY_DEX:
		case APPLY_INT:
		case APPLY_WIS:
		case APPLY_CON:
		case APPLY_HITROLL:
		case APPLY_DAMROLL:
			modifier = calculate_stat_modifier( 0 );
			break;
		case APPLY_MANA:
		case APPLY_HIT:
			modifier = calculate_attrib_modifier( 0 );
			break;
	}

	paf->modifier = modifier;
	paf->next = obj->affected;
	obj->affected = paf;

	SET_BIT( obj->extra_flags, ITEM_MAGIC );

	return;
}


void add_random_extra( OBJ_DATA *obj )
{
	if (!number_bits(4))
		SET_BIT( obj->extra_flags, ITEM_GLOW );
	if (!number_bits(4))
		SET_BIT( obj->extra_flags, ITEM_HUM );
	if (!number_bits(4))
		SET_BIT( obj->extra_flags, ITEM_INVIS );
	if (!number_bits(4))
		SET_BIT( obj->extra_flags, ITEM_BLESS );

	return;
}


int select_potion_color( void )
{
  int color;

  color = number_percent();

  if (color <= 10)
    color = gsn_cure_poison;
  else if (color <= 20)
    color = gsn_bless;
  else if (color <= 30)
    color = gsn_armor;
  else if (color <= 40)
    color = gsn_detect_evil;
  else if ( color <= 43 )
    color = gsn_phantasmal_weaponry;
  else if (color <= 50)
    color = gsn_blindness;
  else if (color <= 60)
    color = gsn_cure_critical;
  else if (color <= 70)
    color = gsn_detect_invis;
  else if (color <= 75)
    color = gsn_farsee;
  else if (color <= 80)
    color = gsn_detect_magic;
  else if (color <= 85)
    color = gsn_sanct;
  else if (color <= 92)
    color = gsn_strength;
  else if (color <= 95)
    color = gsn_brilliance;
  else
    color = gsn_heal;

  return color;
}

void make_random_trap( OBJ_DATA *obj )
{
	char *type;
	char buf[MAX_STRING_LENGTH];

	switch( number_range(1,10) )
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		type = "small wooden trigger";
		obj->value[0] = 0;
		break;
	case 6:
		type = "green spiked ball";
		obj->value[0] = 53;
		break;
	case 7:
		type = "black hole";
		obj->value[0] = 63;
		break;
	case 8:
		type = "brilliant white sphere";
		obj->value[0] = 4;
		break;
	case 9:
		type = "small circular grey ball";
		obj->value[0] = 109;
		break;
	case 10:
		type = "small puzzle box";
		obj->value[0] = 111;
		break;
	default:
		type = "small puzzle box";
		obj->value[0] = 111;
		break;
	}

	sprintf(buf,"a %s",type);
	free_string(obj->short_descr);
	obj->short_descr = str_dup(type);

	sprintf(buf,"%s trap",type);
	free_string(obj->name);
	obj->name = str_dup(type);

	sprintf(buf,"a %s is lying on the ground here.",type);
	free_string(obj->description);
	obj->description = str_dup(type);

	return;
}

#define MAX_QUIVER 28
sh_int *quiver_colors[MAX_QUIVER] =
{
  &gsn_blackmantle,
  &gsn_blindness,
  &gsn_call_lightning,
  &gsn_chill_touch,
  &gsn_colour_spray,
  &gsn_confusion,
  &gsn_curse,
  &gsn_death_spray,
  &gsn_earthquake,
  &gsn_faerie_fire,
  &gsn_fire_breath,
  &gsn_fireball,
  &gsn_flamestrike,
  &gsn_frost_breath,
  &gsn_gas_breath,
  &gsn_harm,
  &gsn_ki_release,
  &gsn_lightning_bolt,
  &gsn_lightning_breath,
  &gsn_magic_missile,
  &gsn_poison,
  &gsn_psionic_blast,
  &gsn_ego_whip,
  &gsn_silence,
  &gsn_sleep,
  &gsn_slow,
  &gsn_vampiric_touch,
  &gsn_weaken
};

void make_random_quiver( OBJ_DATA *obj )
{
  int color;
  char buf[MAX_STRING_LENGTH];

  if ( number_percent() < 50 )
    SET_BIT(obj->extra_flags, ITEM_RANGED_MAGIC);

  color = *quiver_colors[number_range( 0, MAX_QUIVER - 1 )];
  obj->value[1] = color;

  sprintf(buf, "A leather quiver with %s arrows",
    skill_table[color].arrow_color );
  free_string(obj->short_descr);
  obj->short_descr = str_dup(buf);

  sprintf(buf, "leather quiver %s arrow",
    skill_table[color].arrow_color );
  free_string(obj->name);
  obj->name = str_dup(buf);

  sprintf(buf, "A leather quiver with %s arrows is resting here.",
    skill_table[color].arrow_color );
  free_string(obj->description);
  obj->description = str_dup(buf);

  return;
}

OBJ_DATA *create_random_potion( unsigned int phase )
{
  OBJ_DATA *obj;
  int percent;
  char buf[MAX_STRING_LENGTH];
  int vnum;

  if (number_bits(2))
  {
    percent = number_percent();

    if (percent <= 8)
      vnum = 9650; /* burgundy*/
    else if (percent <= 15)
      vnum = 9651; /* white */
    else if (percent <= 23)
      vnum = 9652; /* orange */
    else if (percent <= 31)
      vnum = 9653; /* pink */
    else if (percent <= 36)
      vnum = 9654; /* lavender */
    else if (percent <= 41)
      vnum = 9655; /* yellow */
    else if (percent <= 46)
      vnum = 9656; /* amber */
    else if (percent <= 51)
      vnum = 9657; /* blue */
    else if (percent <= 56)
      vnum = 9658; /* clear */
    else if (percent <= 58)
      vnum = 9659; /* purple */
    else if (percent <= 63)
      vnum = 9660; /* clear red */
    else if (percent <= 68)
      vnum = 9661; /* silver */
    else if (percent <= 73)
      vnum = 9662; /* gummi berry juice */
    else if (percent <= 79)
      vnum = 9663; /* beige */
    else if (percent <= 84)
       vnum = 9664; /* tan */
    else if (percent <= 87)
      vnum = 9666; /* lumpy black */
    else if (percent <= 92)
      vnum = 9667; /* vileness */
    else if (percent <= 94)
      vnum = 9668; /* chalky */
    else if (percent <= 95)
      vnum = 9670; /* aquamrine */
    else
      vnum = 9669; /* azure */

    obj = create_object( get_obj_index( vnum ), phase );
  }
  else
  {
    obj = create_object( get_obj_index( 9699 ), phase );

    obj->value[1] = select_potion_color();
    obj->value[2] = select_potion_color();


    free_string( obj->name );
    free_string( obj->short_descr );
    free_string( obj->description );

    if (number_bits(3))
    {
      sprintf( buf, "potion %s %s",
	skill_table[obj->value[1]].spell_color,
	skill_table[obj->value[2]].spell_color );
      obj->name = str_dup( buf );
      sprintf( buf, "a %s potion with %s swirls",
	skill_table[obj->value[1]].spell_color,
	skill_table[obj->value[2]].spell_color );
      obj->short_descr = str_dup( buf );
      sprintf( buf, "a %s potion with %s swirls lies here.",
	skill_table[obj->value[1]].spell_color,
	skill_table[obj->value[2]].spell_color );
      obj->description = str_dup( buf );
    }
    else
    {
      obj->value[3] = select_potion_color();
      sprintf( buf, "potion %s %s %s",
      skill_table[obj->value[1]].spell_color,
	skill_table[obj->value[2]].spell_color,
	skill_table[obj->value[3]].spell_color );
      obj->name = str_dup( buf );
      sprintf( buf, "a %s potion with %s swirls and %s spots",
	skill_table[obj->value[1]].spell_color,
	skill_table[obj->value[2]].spell_color,
	skill_table[obj->value[3]].spell_color );
      obj->short_descr = str_dup( buf );
      sprintf( buf, "a %s potion with %s swirls and %s spots lies here.",
	skill_table[obj->value[1]].spell_color,
	skill_table[obj->value[2]].spell_color,
	skill_table[obj->value[3]].spell_color );
      obj->description = str_dup( buf );
    }
    if ( obj != NULL )
      obj->cost = dice(7,100);

  }

  return obj;
}


OBJ_DATA *create_random_jewelry( unsigned int phase )
{
  OBJ_DATA *obj;

/* bracelets are 9597, amulets 9597, rings 9599 */
  obj = create_object( get_obj_index( 9596+dice(1,3) ), phase );

  do
  {
    add_random_magic( obj );
  } while ( !number_bits(3) );
  add_random_extra( obj );

  obj->cost = 75;
  while (!number_bits(1))
    obj->cost += number_bits(6);

  return obj;
}


void rename_object_random( OBJ_DATA *obj , int table )
{
	char buf[MAX_STRING_LENGTH];
	char *nbuf;
	char *abuf;

	switch (table)
	{
		case TABLE_JEWELRY:
			nbuf = precious_material_table[number_range(0,MAX_PREC_MAT)];
			abuf = precious_adjective_table[number_range(0,MAX_PREC_ADJ)];
			break;
		case TABLE_STAFF:
		default:
			nbuf = material_table[number_range(0,MAX_MATERIAL)];
			abuf = adjective_table[number_range(0,MAX_ADJECTIVE)];
			break;
		case TABLE_SCROLL:
			nbuf = scroll_table[number_range(0,MAX_SCROLL)];
			abuf = str_empty;
			break;
	}

	sprintf(buf, "%s%s%s %s", 
		abuf, (table == TABLE_SCROLL ? "" : " "), nbuf,
		obj->name );
	free_string( obj->name );
	obj->name = str_dup( buf );

	sprintf(buf, "the %s",
		obj->name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, "%s lies here.",
		obj->short_descr );
	free_string( obj->description );
	obj->description = str_dup( buf );

	return;
}



OBJ_DATA *create_random_magic( unsigned int phase )
{
	OBJ_DATA *obj;
	int vnum;

int staff_spell_table [10] =
{
	gsn_vampiric_touch,
	gsn_blindness,
	gsn_sleep,
	gsn_bless,
	gsn_cure_critical,
	gsn_cure_light,
	gsn_teleport,
	gsn_word_of_recall,
	gsn_fly,
	gsn_infravision
};

int wand_spell_table [11] =
{
	gsn_lightning_bolt,
	gsn_flamestrike,
	gsn_poison,
	gsn_invis,
	gsn_dispel_evil,
	gsn_dispel_magic,
	gsn_detect_poison,
	gsn_colour_spray,
	gsn_blindness,
	gsn_curse,
	gsn_faerie_fire
};

	switch (number_bits(2))
	{
		case 0:
			obj = create_object( get_obj_index( 9799 ), phase );
			obj->value[3] = staff_spell_table[number_range(0,9)];
			obj->value[1] = 5 + number_bits(2);
			break;
		case 1:
			obj = create_object( get_obj_index( 9798 ), phase );
			obj->value[3] = wand_spell_table[number_range(0,10)];
			obj->value[1] = 10 + number_bits(3);
			break;
		default:
			vnum = 9790;
			vnum += number_bits(3);
			obj = create_object( get_obj_index( vnum ), phase );
			break;
	}

	add_random_magic( obj );
	add_random_extra( obj );

	return obj;
}

void do_trap( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;

	obj = create_object(get_obj_index(5), ch->phase);

	make_random_trap(obj);
	obj_to_char(obj, ch );

	send_to_char("Ok.\n\r",ch);
	return;
}

void do_arrow( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;

	obj = create_object(get_obj_index(4), ch->phase);
	make_random_quiver(obj);

	obj_to_char(obj, ch );
	send_to_char("Ok.\n\r", ch );
	return;
}

void do_potion( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char arg3[MAX_STRING_LENGTH];
	int value;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if( arg1[0] == '\0' )
	{
		obj = create_random_potion( ch->phase );
		obj_to_char( obj, ch );
		send_to_char( "Ok.\n\r", ch );
		return;
	}
	else if( !str_cmp( arg1, "random" ) )
	{
		char buf[MAX_STRING_LENGTH];
		int x;

		obj = create_object( get_obj_index(9699), ch->phase);

                free_string( obj->name );
                free_string( obj->short_descr );
                free_string( obj->description );

                sprintf( buf, "%s's potion", ch->name );
                obj->name               = str_dup( buf );
                obj->short_descr        = str_dup( buf );
                sprintf( buf, "A potion made by %s is here!", ch->name );
                obj->description        = str_dup( buf );

		for( x = 1; x <= 3; x++ )
		{
			obj->value[x] = number_range(0, 110);
		}
		send_to_char( "Ok.\n\r", ch );
		obj_to_char( obj, ch );
		return;
	}
	else
	{
		char buf[MAX_STRING_LENGTH];

		obj = create_object( get_obj_index(9699), ch->phase );

		free_string( obj->name );
		free_string( obj->short_descr );
		free_string( obj->description );

		sprintf( buf, "%s's potion", ch->name );
		obj->name		= str_dup( buf );
		obj->short_descr	= str_dup( buf );
		sprintf( buf, "A potion made by %s is here!", ch->name );
		obj->description	= str_dup( buf );

		if( arg1[0] != '\0' )
		{
			if( is_number(arg1) )
			{
				value		= atoi( arg1 );
				obj->value[1]	= value;
			}
			else
			{
				if( skill_lookup(arg1) <= 0 )
					send_to_char("Argument 1 is not a spell, skipping.\n\r", ch );
				else
					obj->value[1] = skill_lookup(arg1);
			}
		}
	
		if( arg2[0] != '\0' )
		{
			if( is_number(arg2) )
			{
				value		= atoi( arg2 );
				obj->value[2]	= value;
			}
			else
			{
				if( skill_lookup(arg2) <= 0 )
					send_to_char("Argument 2 is not a spell, skipping.\n\r", ch );
				else
					obj->value[2] = skill_lookup(arg2);
			}
		}

		if( arg3[0] != '\0' )
		{
			if( is_number(arg3) )
			{
				value		= atoi( arg3 );
				obj->value[3]	= value;
			}
			else
			{
				if( skill_lookup(arg3) <= 0 )
					send_to_char("Argument 3 is not a spell, skipping.\n\r", ch );
				else
					obj->value[3] = skill_lookup(arg3);
			}
		}

		obj->value[0]	= ch->level;
		obj_to_char( obj, ch );
		send_to_char( "Ok.\n\r", ch );
		return;
	}
	return;
}		

void do_armor( CHAR_DATA *ch, char *arg )
{
	OBJ_DATA *obj;

	obj = create_random_armor( ch->phase );

	if (obj != NULL)
	{
		obj_to_char( obj, ch );
		add_random_magic( obj );
		add_random_extra( obj );
		send_to_char( "Ok.\n\r", ch );
	}

	return;
}



void check_random_equip( CHAR_DATA *ch )
{
	OBJ_DATA *obj;

	int percent;

/* gold */
	if (number_percent() < (ch->level * 2) && ch->level <= 40 )
		ch->gold += UMIN( 5, number_range(1,ch->level) );


	while(number_percent() < UMIN( 80, ch->level*3 ) )
	{

		percent = number_percent();

		if (percent <= 2)
/* various */
		{
			obj = create_random_armor( ch->phase );
		}
		else if (percent <= 20)
		{
			obj = create_random_jewelry( ch->phase );
		}
		else if (percent <= 40)
/* weapon */
		{
			obj = create_random_armor( ch->phase );
		}
		else if (percent <= 70)
		{
			obj = create_random_armor( ch->phase );
			if (obj != NULL)
			{
			while(!number_bits(1))
				add_random_magic( obj );
			add_random_extra( obj );
			}
		}
		else if (percent <= 80)
		{
/*
			obj= create_random_armor( ch->phase );
*/
			obj = create_random_magic( ch->phase );
		}
		else
		{
			obj = create_random_potion( ch->phase );
/*
			obj = create_random_magic( ch->phase );
*/
			add_random_extra( obj );
		}

			if (obj != NULL)
		obj_to_char( obj, ch );
	}

	return;

}
