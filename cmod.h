/*
 * cmod.h
 */


struct class_info
{
  char	*class_name;
  char	*who_name;
  int	hits;
  int	mana;
  int	strength;
  int	intelligence;
  int	wisdom;
  int	dexterity;
  int	constitution;
  int	thac0;
  int	min_rating;
  bool	enabled;
  int	random;
  int	skill_percent[MAX_SKILL];
  int	skill_cost[MAX_SKILL];
  int	skill_delay[MAX_SKILL];
  int	regen_hit;
  int	regen_mana;
  int	rating_12;
  int	rating_11;
  int	base_armor;
  int	resist_fire;
  int	resist_cold;
  int	resist_lightning;
  int	resist_physical;
  int	resist_magic;
  int	resist_drain;
};

extern       struct  class_info      class_table     [MAX_CLASS];


