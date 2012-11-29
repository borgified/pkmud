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



bool    check_social    args( ( CHAR_DATA *ch, char *command,
			    char *argument ) );



/*
 * Command logging types.
 */
#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2



/*
 * Log-all switch.
 */
bool                            fLogAll         = FALSE;



/*
 * Command table.
 */
const   struct  cmd_type        cmd_table       [] =
{
    /*
     * Common movement commands.
     */
    { "north",          do_north,       POS_STANDING,    0,  LOG_NORMAL },
    { "east",           do_east,        POS_STANDING,    0,  LOG_NORMAL },
    { "south",          do_south,       POS_STANDING,    0,  LOG_NORMAL },
    { "west",           do_west,        POS_STANDING,    0,  LOG_NORMAL },
    { "up",             do_up,          POS_STANDING,    0,  LOG_NORMAL },
    { "down",           do_down,        POS_STANDING,    0,  LOG_NORMAL },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "buy",            do_buy,         POS_RESTING,     0,  LOG_NORMAL },
    { "cast",           do_cast,        POS_FIGHTING,    0,  LOG_NORMAL },
    { "exits",          do_exits,       POS_RESTING,     0,  LOG_NORMAL },
    { "get",            do_get,         POS_RESTING,     0,  LOG_NORMAL },
    { "inventory",      do_inventory,   POS_DEAD,        0,  LOG_NORMAL },
    { "kill",           do_kill,        POS_FIGHTING,    0,  LOG_NORMAL },
    { "hit",            do_kill,        POS_FIGHTING,    0,  LOG_NORMAL },
    { "look",           do_look,        POS_RESTING,     0,  LOG_NORMAL },
    { "order",          do_order,       POS_RESTING,     0,  LOG_NORMAL },
    { "rest",           do_rest,        POS_RESTING,     0,  LOG_NORMAL },
    { "shoot",          do_shoot,       POS_FIGHTING,    0,  LOG_NORMAL },
    { "set",		do_set,		POS_STANDING,	 0,  LOG_NORMAL },
    { "stand",          do_stand,       POS_SLEEPING,    0,  LOG_NORMAL },
    { "tell",           do_tell,        POS_RESTING,     0,  LOG_NORMAL },
    { "wield",          do_wield,       POS_RESTING,     0,  LOG_NORMAL },
    { "wizhelp",        do_wizhelp,     POS_DEAD,       36,  LOG_NORMAL },

    /*
     * Informational commands.
     */
    { "areas",          do_areas,       POS_DEAD,        0,  LOG_NORMAL },
    { "bug",            do_bug,         POS_DEAD,        0,  LOG_ALWAYS },
    { "commands",       do_commands,    POS_DEAD,        0,  LOG_NORMAL },
    { "compare",        do_compare,     POS_RESTING,     0,  LOG_NORMAL },
    { "consider",       do_consider,    POS_RESTING,     0,  LOG_NORMAL },
    { "credits",        do_credits,     POS_DEAD,        0,  LOG_NORMAL },
    { "equipment",      do_equipment,   POS_DEAD,        0,  LOG_NORMAL },
    { "examine",        do_examine,     POS_RESTING,     0,  LOG_NORMAL },
    { "help",           do_help,        POS_DEAD,        0,  LOG_NORMAL },
    { "identify",       do_identify,    POS_DEAD,        0,  LOG_NORMAL },
    { "idea",           do_idea,        POS_DEAD,        0,  LOG_ALWAYS },
    { "last",           do_last,        POS_DEAD,        0,  LOG_NORMAL },
    { "motd",           do_motd,        POS_DEAD,        0,  LOG_NORMAL },
    { "score",          do_score,       POS_DEAD,        0,  LOG_NORMAL },
    { "socials",        do_socials,     POS_DEAD,        0,  LOG_NORMAL },
    { "time",           do_time,        POS_DEAD,        0,  LOG_NORMAL },
    { "typo",           do_typo,        POS_DEAD,        0,  LOG_ALWAYS },
    { "weather",        do_weather,     POS_RESTING,     0,  LOG_NORMAL },
    { "who",            do_who,         POS_DEAD,        0,  LOG_NORMAL },
    { "wizlist",        do_wizlist,     POS_DEAD,        0,  LOG_NORMAL },

    /*
     * Configuration commands.
     */
    { "autoexit",       do_autoexit,    POS_DEAD,        0,  LOG_NORMAL },
    { "brief",          do_brief,       POS_DEAD,        0,  LOG_NORMAL },
    { "channels",       do_channels,    POS_DEAD,        0,  LOG_NORMAL },
    { "compact",        do_compact,     POS_DEAD,        0,  LOG_NORMAL },
    { "config",         do_config,      POS_DEAD,        0,  LOG_NORMAL },
    { "color",		do_color,	POS_DEAD,	 0,  LOG_NORMAL },
    { "colour",		do_color,	POS_DEAD,	 0,  LOG_NORMAL },
    { "description",    do_description, POS_DEAD,        0,  LOG_NORMAL },
    { "password",       do_password,    POS_DEAD,        0,  LOG_NEVER  },
    { "title",          do_title,       POS_DEAD,        0,  LOG_NORMAL },

    /*
     * Communication commands.
     */
    { "answer",         do_answer,      POS_SLEEPING,    0,  LOG_NORMAL },
    { "auction",        do_auction,     POS_SLEEPING,    0,  LOG_NORMAL },
    { "chat",           do_chat,        POS_SLEEPING,    0,  LOG_NORMAL },
    { ".",              do_chat,        POS_SLEEPING,    0,  LOG_NORMAL },
    { "death",          do_death,       POS_SLEEPING,   39,  LOG_NORMAL },
    { "emote",          do_emote,       POS_RESTING,     0,  LOG_NORMAL },
    { ",",              do_emote,       POS_RESTING,     0,  LOG_NORMAL },
    { "info",           do_info,        POS_SLEEPING,    0,  LOG_NORMAL },
    { "music",          do_music,       POS_SLEEPING,    0,  LOG_NORMAL },
    { "nmodify",        do_nmodify,     POS_RESTING,     0,  LOG_NORMAL },
    { "note",           do_note,        POS_RESTING,     0,  LOG_NORMAL },
    { "question",       do_question,    POS_SLEEPING,    0,  LOG_NORMAL },
    { "reply",          do_reply,       POS_RESTING,     0,  LOG_NORMAL },
    { "say",            do_say,         POS_RESTING,     0,  LOG_NORMAL },
    { "'",              do_say,         POS_RESTING,     0,  LOG_NORMAL },
    { "shout",          do_shout,       POS_RESTING,     0,  LOG_NORMAL },
    { "subwar",         do_subwar,      POS_SLEEPING,    0,  LOG_NORMAL },
    { "yell",           do_yell,        POS_RESTING,     0,  LOG_NORMAL },
    { "war",		do_warmessage,	POS_RESTING,	38,  LOG_NORMAL },
    /*
     * Object manipulation commands.
     */
    { "brandish",       do_use,    	POS_RESTING,     0,  LOG_NORMAL },
    { "close",          do_close,       POS_STANDING,     0,  LOG_NORMAL },
    { "coinflip",       do_coinflip,    POS_RESTING,     0,  LOG_NORMAL },
    { "drink",          do_drink,       POS_RESTING,     0,  LOG_NORMAL },
    { "drop",           do_drop,        POS_RESTING,     0,  LOG_NORMAL },
    { "eat",            do_eat,         POS_RESTING,     0,  LOG_NORMAL },
    { "fill",           do_fill,        POS_RESTING,     0,  LOG_NORMAL },
    { "give",           do_give,        POS_RESTING,     0,  LOG_NORMAL },
    { "hold",           do_wear,        POS_RESTING,     0,  LOG_NORMAL },
    { "junk",           do_junk,        POS_RESTING,     0,  LOG_NORMAL },
    { "list",           do_list,        POS_RESTING,     0,  LOG_NORMAL },
    { "lock",           do_lock,        POS_STANDING,    0,  LOG_NORMAL },
    { "open",           do_open,        POS_STANDING,    0,  LOG_NORMAL },
    { "pick",           do_pick,        POS_RESTING,     0,  LOG_NORMAL },
    { "put",            do_put,         POS_RESTING,     0,  LOG_NORMAL },
    { "quaff",          do_quaff,       POS_RESTING,     0,  LOG_NORMAL },
    { "recite",         do_recite,      POS_RESTING,     0,  LOG_NORMAL },
    { "remove",         do_remove,      POS_RESTING,     0,  LOG_NORMAL },
    { "sell",           do_sell,        POS_RESTING,     0,  LOG_NORMAL },
    { "take",           do_get,         POS_RESTING,     0,  LOG_NORMAL },
    { "sacrifice",      do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL },
    { "unlock",         do_unlock,      POS_STANDING,    0,  LOG_NORMAL },
    { "use",		do_use,		POS_RESTING,	 0,  LOG_NORMAL },
    { "value",          do_value,       POS_RESTING,     0,  LOG_NORMAL },
    { "wear",           do_wear,        POS_RESTING,     0,  LOG_NORMAL },
    { "zap",            do_use,         POS_RESTING,     0,  LOG_NORMAL },

    /*
     * Combat commands.
     */
    { "backstab",       do_backstab,    POS_STANDING,    0,  LOG_NORMAL },
    { "bs",             do_backstab,    POS_STANDING,    0,  LOG_NORMAL },
    { "disarm",         do_disarm,      POS_FIGHTING,    0,  LOG_NORMAL },
    { "disarmtrap",	do_disarmtrap,	POS_STANDING,	 0,  LOG_NORMAL },
    { "flee",           do_flee,        POS_FIGHTING,    0,  LOG_NORMAL },
    { "caltrops",	do_caltrops,	POS_STANDING,	 0,  LOG_NORMAL },
    { "kick",           do_kick,        POS_FIGHTING,    0,  LOG_NORMAL },
    { "rescue",         do_rescue,      POS_FIGHTING,    0,  LOG_NORMAL },
    { "bash",           do_bash,        POS_FIGHTING,   10,  LOG_NORMAL },

    /*
     * Miscellaneous commands.
     */
    { "follow",         do_follow,      POS_RESTING,     0,  LOG_NORMAL },
    { "hide",           do_hide,        POS_RESTING,     0,  LOG_NORMAL },
    { "practice",       do_practice,    POS_SLEEPING,    0,  LOG_NORMAL },
    { "qui",            do_qui,         POS_DEAD,        0,  LOG_NORMAL },
    { "quit",           do_quit,        POS_DEAD,        0,  LOG_NORMAL },
    { "quietmode",	do_quietmode,	POS_DEAD,	 0,  LOG_NORMAL },
    { "recall",         do_recall,      POS_FIGHTING,    0,  LOG_NORMAL },
    { "/",              do_recall,      POS_FIGHTING,    0,  LOG_NORMAL },
    { "rent",           do_rent,        POS_DEAD,        0,  LOG_NORMAL },
    { "save",           do_save,        POS_DEAD,        0,  LOG_NORMAL },
    { "sleep",          do_sleep,       POS_SLEEPING,    0,  LOG_NORMAL },
    { "snare",          do_snare,       POS_STANDING,    0,  LOG_NORMAL },
    { "sneak",          do_sneak,       POS_STANDING,    0,  LOG_NORMAL },
    { "steal",          do_steal,       POS_STANDING,    0,  LOG_NORMAL },
    { "visible",        do_visible,     POS_SLEEPING,    0,  LOG_NORMAL },
    { "wake",           do_wake,        POS_SLEEPING,    0,  LOG_NORMAL },
    { "where",          do_where,       POS_RESTING,     0,  LOG_NORMAL },
    { "whereis",        do_whereis,     POS_RESTING,     0,  LOG_NORMAL },
    { "whereload",      do_whereload,   POS_RESTING,     0,  LOG_NORMAL },


    /* various war related commands, added by Rip.. */
    { "start",          do_start,       POS_RESTING,    30,  LOG_NORMAL },
    { "noplay",         do_noplay,      POS_DEAD,       30,  LOG_NORMAL },
    { "finger",         do_finger,      POS_DEAD,       30,  LOG_NORMAL },
    { "high",           do_high,        POS_DEAD,       30,  LOG_NORMAL },
    { "changeclass",    do_changeclass, POS_DEAD,       30,  LOG_NORMAL },
    { "ffa",            do_joinffa,     POS_STANDING,    0,  LOG_NORMAL },
    { "private",        do_join_private, POS_STANDING,   0,  LOG_NORMAL },
    { "joinwar",        do_joinwar,     POS_STANDING,   30,  LOG_NORMAL },
    { "teleport",       do_tele,        POS_STANDING,   30,  LOG_NORMAL },
    { "randomclass",	do_randomclass,	POS_DEAD,	30,  LOG_NORMAL },

    /* ex-god commands */
    { "ofind",          do_ofind,       POS_DEAD,       30,  LOG_NORMAL },
    { "osearch",        do_osearch,     POS_DEAD,       30,  LOG_NORMAL },
    { "vostat",         do_vostat,      POS_DEAD,       30,  LOG_NORMAL },

    /* class stuff */
    { "cdisable",       do_cdisable,	POS_DEAD,       40,  LOG_ALWAYS },
    { "cenable",        do_cenable,	POS_DEAD,       40,  LOG_ALWAYS },
    { "cmod",	        do_cmod,	POS_DEAD,       40,  LOG_ALWAYS },
    { "cskill",	        do_cskill,	POS_DEAD,       40,  LOG_ALWAYS },
    { "cview",          do_cview,       POS_DEAD,       30,  LOG_NORMAL },
    { "cwrite",         do_cwrite,      POS_DEAD,       40,  LOG_ALWAYS },
    { "rlog",		do_rlog,	POS_DEAD,	40,  LOG_NORMAL },


    /*
     * Immortal commands.
     */
    { "advance",        do_advance,     POS_DEAD,       40,  LOG_ALWAYS },
    { "trust",          do_trust,       POS_DEAD,       40,  LOG_ALWAYS },
    { "whichway",       do_whichway,    POS_DEAD,       40,  LOG_ALWAYS },
    { "sicem",          do_sic_mob,     POS_DEAD,       40,  LOG_ALWAYS },
    { "reset",          do_reset,       POS_DEAD,       40,  LOG_NORMAL },
    { "armor",          do_armor,       POS_DEAD,       40,  LOG_NORMAL },
    { "delay",          do_delay,       POS_DEAD,       40,  LOG_NORMAL },
    { "warstop",        do_warstop,     POS_DEAD,       40,  LOG_NORMAL },

    { "scatter",	do_scatter,	POS_DEAD,	39,  LOG_NORMAL },
    { "arrow",		do_arrow,	POS_DEAD,	39,  LOG_NORMAL },
    { "trap",		do_trap,	POS_DEAD,	39,  LOG_NORMAL },
    { "potion",		do_potion,	POS_DEAD,	39,  LOG_NORMAL },
    { "wartime",	do_wartime,	POS_DEAD,	39,  LOG_NORMAL },
    { "warflags",	do_warflags,	POS_DEAD,	39,  LOG_NORMAL }, 
    { "allow",          do_allow,       POS_DEAD,       39,  LOG_ALWAYS },
    { "ban",            do_ban,         POS_DEAD,       39,  LOG_ALWAYS },
    { "deny",           do_deny,        POS_DEAD,       40,  LOG_ALWAYS },
    { "disconnect",     do_disconnect,  POS_DEAD,       39,  LOG_ALWAYS },
    { "freeze",         do_freeze,      POS_DEAD,       39,  LOG_ALWAYS },
    { "reboo",          do_reboo,       POS_DEAD,       39,  LOG_NORMAL },
    { "reboot",         do_reboot,      POS_DEAD,       39,  LOG_ALWAYS },
    { "shutdow",        do_shutdow,     POS_DEAD,       39,  LOG_NORMAL },
    { "shutdown",       do_shutdown,    POS_DEAD,       39,  LOG_ALWAYS },
    { "users",          do_users,       POS_DEAD,       30,  LOG_NORMAL },
    { "wizlock",        do_wizlock,     POS_DEAD,       39,  LOG_ALWAYS },
    { "notify",		do_notify,	POS_DEAD,	39,  LOG_NORMAL },

    { "force",          do_force,       POS_DEAD,       38,  LOG_ALWAYS },
    { "mload",          do_mload,       POS_DEAD,       38,  LOG_ALWAYS },
    { "mset",           do_mset,        POS_DEAD,       39,  LOG_ALWAYS },
    { "noemote",        do_noemote,     POS_DEAD,       38,  LOG_NORMAL },
    { "notell",         do_notell,      POS_DEAD,       38,  LOG_NORMAL },
    { "oload",          do_oload,       POS_DEAD,       30,  LOG_NORMAL },
    { "oset",           do_oset,        POS_DEAD,       39,  LOG_ALWAYS },
    { "purge",          do_purge,       POS_DEAD,       38,  LOG_NORMAL },
    { "restore",        do_restore,     POS_DEAD,       37,  LOG_ALWAYS },
    { "rset",           do_rset,        POS_DEAD,       38,  LOG_ALWAYS },
    { "silence",        do_silence,     POS_DEAD,       38,  LOG_NORMAL },
    { "sla",            do_sla,         POS_DEAD,       38,  LOG_NORMAL },
    { "slay",           do_slay,        POS_DEAD,       38,  LOG_ALWAYS },
    { "sset",           do_sset,        POS_DEAD,       39,  LOG_ALWAYS },
    { "transfer",       do_transfer,    POS_DEAD,       0,  LOG_ALWAYS },

    { "at",             do_at,          POS_DEAD,       37,  LOG_ALWAYS },
    { "bamfin",         do_bamfin,      POS_DEAD,       37,  LOG_NORMAL },
    { "bamfout",        do_bamfout,     POS_DEAD,       37,  LOG_NORMAL },
    { "specho",		do_specho,	POS_DEAD,	40,  LOG_ALWAYS },
    { "echo",           do_echo,        POS_DEAD,       38,  LOG_ALWAYS },
    { "goto",           do_goto,        POS_DEAD,       30,  LOG_NORMAL },
    { "gossip",		do_chat,	POS_DEAD,	 0,  LOG_NORMAL },
    { "holylight",      do_holylight,   POS_DEAD,       37,  LOG_NORMAL },
    { "invis",          do_invis,       POS_DEAD,       37,  LOG_NORMAL },
    { "log",            do_log,         POS_DEAD,       38,  LOG_ALWAYS },
    { "soul",		do_soul,	POS_DEAD,	38,  LOG_NORMAL },
    { "soulwrite",	do_soulwrite,	POS_DEAD,	38,  LOG_NORMAL },
    { "memory",         do_memory,      POS_DEAD,       37,  LOG_NORMAL },
    { "mfind",          do_mfind,       POS_DEAD,       37,  LOG_NORMAL },
    { "mstat",          do_mstat,       POS_DEAD,       37,  LOG_ALWAYS },
    { "mwhere",         do_mwhere,      POS_DEAD,       37,  LOG_NORMAL },
    { "ostat",          do_ostat,       POS_DEAD,       37,  LOG_NORMAL },
    { "peace",          do_peace,       POS_DEAD,       37,  LOG_NORMAL },
    { "recho",          do_recho,       POS_DEAD,       38,  LOG_ALWAYS },
    { "return",         do_return,      POS_DEAD,       39,  LOG_NORMAL },
    { "rstat",          do_rstat,       POS_DEAD,       30,  LOG_NORMAL },
    { "slookup",        do_slookup,     POS_DEAD,       37,  LOG_NORMAL },
    { "snoop",          do_snoop,       POS_DEAD,       30,  LOG_ALWAYS },
    { "switch",         do_switch,      POS_DEAD,       39,  LOG_ALWAYS },
/* warstarter!! */
    { "warstart",       do_warstart,    POS_DEAD,       39,  LOG_ALWAYS },

    { "immtalk",        do_immtalk,     POS_DEAD,       36,  LOG_NORMAL },
    { ":",              do_immtalk,     POS_DEAD,       36,  LOG_NORMAL },


    /*
     * End of list.
     */
    { "",               0,              POS_DEAD,        0,  LOG_NORMAL }
};



/*
 * The social table.
 * Add new socials here.
 * Alphabetical order is not required.
 */
const   struct  social_type     social_table [] =
{
    {
	"accuse",
	"Accuse whom?",
	"$n is in an accusing mood.",
	"You look accusingly at $M.",
	"$n looks accusingly at $N.",
	"$n looks accusingly at you.",
	"You accuse yourself.",
	"$n seems to have a bad conscience."
    },

    {
	"applaud",
	"Clap, clap, clap.",
	"$n gives a round of applause.",
	"You clap at $S actions.",
	"$n claps at $N's actions.",
	"$n gives you a round of applause.  You MUST'VE done something good!",
	"You applaud at yourself.  Boy, are we conceited!",
	"$n applauds at $mself.  Boy, are we conceited!"
    },

    {
	"bark",
	"Woof!  Woof!",
	"$n barks like a dog.",
	"You bark at $M.",
	"$n barks at $N.",
	"$n barks at you.",
	"You bark at yourself.  Woof!  Woof!",
	"$n barks at $mself.  Woof!  Woof!"
    },

    {
	"beer",
	"You down a cold, frosty beer.",
	"$n downs a cold, frosty beer.",
	"You draw a cold, frosty beer for $N.",
	"$n draws a cold, frosty beer for $N.",
	"$n draws a cold, frosty beer for you.",
	"You draw yourself a beer.",
	"$n draws $mself a beer."
    },

    {
	"beg",
	"You beg the gods for mercy.",
	"The gods fall down laughing at $n's request for mercy.",
	"You desperately try to squeeze a few coins from $M.",
	"$n begs you for money.",
	"$n begs $N for a gold piece!",
	"Begging yourself for money doesn't help.",
	"$n begs $mself for money."
    },

    {
	"blush",
	"Your cheeks are burning.",
	"$n blushes.",
	"You get all flustered up seeing $M.",
	"$n blushes as $e sees $N here.",
	"$n blushes as $e sees you here.  Such an effect on people!",
	"You blush at your own folly.",
	"$n blushes as $e notices $s boo-boo."
    },

    {
	"bounce",
	"BOIINNNNNNGG!",
	"$n bounces around.",
	"You bounce onto $S lap.",
	"$n bounces onto $N's lap.",
	"$n bounces onto your lap.",
	"You bounce your head like a basketball.",
	"$n plays basketball with $s head."
    },

    {
	"bow",
	"You bow deeply.",
	"$n bows deeply.",
	"You bow before $M.",
	"$n bows before $N.",
	"$n bows before you.",
	"You kiss your toes.",
	"$n folds up like a jack knife and kisses $s own toes."
    },

    {
	"burp",
	"You burp loudly.",
	"$n burps loudly.",
	"You burp loudly to $M in response.",
	"$n burps loudly in response to $N's remark.",
	"$n burps loudly in response to your remark.",
	"You burp at yourself.",
	"$n burps at $mself.  What a sick sight."
    },

    {
	"cackle",
	"You throw back your head and cackle with insane glee!",
	"$n throws back $s head and cackles with insane glee!",
	"You cackle gleefully at $N",
	"$n cackles gleefully at $N.",
	"$n cackles gleefully at you.  Better keep your distance from $m.",
	"You cackle at yourself.  Now, THAT'S strange!",
	"$n is really crazy now!  $e cackles at $mself."
    },

    {
	"chuckle",
	"You chuckle politely.",
	"$n chuckles politely.",
	"You chuckle at $S joke.",
	"$n chuckles at $N's joke.",
	"$n chuckles at your joke.",
	"You chuckle at your own joke, since no one else would.",
	"$n chuckles at $s own joke, since none of you would."
    },

    {
	"clap",
	"You clap your hands together.",
	"$n shows $s approval by clapping $s hands together.",
	"You clap at $S performance.",
	"$n claps at $N's performance.",
	"$n claps at your performance.",
	"You clap at your own performance.",
	"$n claps at $s own performance."
    },

    {
	"comb",
	"You comb your hair - perfect.",
	"$n combs $s hair, how dashing!",
	"You patiently untangle $N's hair - what a mess!",
	"$n tries patiently to untangle $N's hair.",
	"$n pulls your hair in an attempt to comb it.",
	"You pull your hair, but it refused to be combed.",
	"$n tries to comb $s tangled hair."
    },

    {
	"comfort",
	"Do you feel uncomfortable?",
	"$n needs comforting.",
	"You comfort $M.",
	"$n comforts $N.",
	"$n comforts you.",
	"You make a vain attempt to comfort yourself.",
	"$n has no one to comfort $m but $mself."
    },

    {
	"cringe",
	"You cringe in terror.",
	"$n cringes in terror!",
	"You cringe away from $M.",
	"$n cringes away from $N in mortal terror.",
	"$n cringes away from you.",
	"I beg your pardon?",
	NULL
    },

    {
	"cry",
	"Waaaaah ...",
	"$n bursts into tears.",
	"You cry on $S shoulder.",
	"$n cries on $N's shoulder.",
	"$n cries on your shoulder.",
	"You cry to yourself.",
	"$n sobs quietly to $mself."
    },

    {
	"cuddle",
	"Whom do you feel like cuddling today?",
	NULL,
	"You cuddle $M.",
	"$n cuddles $N.",
	"$n cuddles you.",
	"You must feel very cuddly indeed ... :)",
	"$n cuddles up to $s shadow.  What a sorry sight."
    },

    {
	"curse",
	"You swear loudly for a long time.",
	"$n swears: @*&^%@*&!",
	"You swear at $M.",
	"$n swears at $N.",
	"$n swears at you!  Where are $s manners?",
	"You swear at your own mistakes.",
	"$n starts swearing at $mself.  Why don't you help?"
    },

    {
	"curtsey",
	"You curtsey to your audience.",
	"$n curtseys gracefully.",
	"You curtsey to $M.",
	"$n curtseys gracefully to $N.",
	"$n curtseys gracefully for you.",
	"You curtsey to your audience (yourself).",
	"$n curtseys to $mself, since no one is paying attention to $m."
    },

    {
	"dance",
	"Feels silly, doesn't it?",
	"$n does the Toxic Waltz.",
	"You sweep $M into a romantic waltz.",
	"$n sweeps $N into a romantic waltz.",
	"$n sweeps you into a romantic waltz.",
	"You skip and dance around by yourself.",
	"$n dances a pas-de-une."
    },

    /*
     * This one's for Baka, Penn, and Onethumb!
     */
    {
	"drool",
	"You drool on yourself.",
	"$n drools on $mself.",
	"You drool all over $N.",
	"$n drools all over $N.",
	"$n drools all over you.",
	"You drool on yourself.",
	"$n drools on $mself."
    },

    {
	"fart",
	"Where are your manners?",
	"$n lets off a real rip-roarer ... a greenish cloud envelops $n!",
	"You fart at $M.  Boy, you are sick.",
	"$n farts in $N's direction.  Better flee before $e turns to you!",
	"$n farts in your direction.  You gasp for air.",
	"You fart at yourself.  You deserve it.",
	"$n farts at $mself.  Better $m than you."
    },

    {
	"flip",
	"You flip head over heels.",
	"$n flips head over heels.",
	"You flip $M over your shoulder.",
	"$n flips $N over $s shoulder.",
	"$n flips you over $s shoulder.  Hmmmm.",
	"You tumble all over the room.",
	"$n does some nice tumbling and gymnastics."
    },

    {
	"fondle",
	"Who needs to be fondled?",
	NULL,
	"You fondly fondle $M.",
	"$n fondly fondles $N.",
	"$n fondly fondles you.",
	"You fondly fondle yourself, feels funny doesn't it ?",
	"$n fondly fondles $mself - this is going too far !!"
    },

    {
	"french",
	"Kiss whom?",
	NULL,
	"You give $N a long and passionate kiss.",
	"$n kisses $N passionately.",
	"$n gives you a long and passionate kiss.",
	"You gather yourself in your arms and try to kiss yourself.",
	"$n makes an attempt at kissing $mself."
    },

    {
	"frown",
	"What's bothering you ?",
	"$n frowns.",
	"You frown at what $E did.",
	"$n frowns at what $E did.",
	"$n frowns at what you did.",
	"You frown at yourself.  Poor baby.",
	"$n frowns at $mself.  Poor baby."
    },

    {
	"fume",
	"You grit your teeth and fume with rage.",
	"$n grits $s teeth and fumes with rage.",
	"You stare at $M, fuming.",
	"$n stares at $N, fuming with rage.",
	"$n stares at you, fuming with rage!",
	"That's right - hate yourself!",
	"$n clenches $s fists and stomps $s feet, fuming with anger."
    },

    {
	"gasp",
	"You gasp in astonishment.",
	"$n gasps in astonishment.",
	"You gasp as you realize what $e did.",
	"$n gasps as $e realizes what $N did.",
	"$n gasps as $e realizes what you did.",
	"You look at yourself and gasp!",
	"$n takes one look at $mself and gasps in astonisment!"
    },

    {
	"giggle",
	"You giggle.",
	"$n giggles.",
	"You giggle in $S presence.",
	"$n giggles at $N's actions.",
	"$n giggles at you.  Hope it's not contagious!",
	"You giggle at yourself.  You must be nervous or something.",
	"$n giggles at $mself.  $e must be nervous or something."
    },

    {
	"glare",
	"You glare at nothing in particular.",
	"$n glares around $m.",
	"You glare icily at $M.",
	"$n glares at $N.",
	"$n glares icily at you, you feel cold to your bones.",
	"You glare icily at your feet, they are suddenly very cold.",
	"$n glares at $s feet, what is bothering $m?"
    },

    {
	"grin",
	"You grin evilly.",
	"$n grins evilly.",
	"You grin evilly at $M.",
	"$n grins evilly at $N.",
	"$n grins evilly at you.  Hmmm.  Better keep your distance.",
	"You grin at yourself.  You must be getting very bad thoughts.",
	"$n grins at $mself.  You must wonder what's in $s mind."
    },

    {
	"groan",
	"You groan loudly.",
	"$n groans loudly.",
	"You groan at the sight of $M.",
	"$n groans at the sight of $N.",
	"$n groans at the sight of you.",
	"You groan as you realize what you have done.",
	"$n groans as $e realizes what $e has done."
    },

    {
	"grope",
	"Whom do you wish to grope?",
	NULL,
	"Well, what sort of noise do you expect here?",
	"$n gropes $N.",
	"$n gropes you.",
	"You grope yourself - YUCK.",
	"$n gropes $mself - YUCK."
    },

    {
	"grovel",
	"You grovel in the dirt.",
	"$n grovels in the dirt.",
	"You grovel before $M.",
	"$n grovels in the dirt before $N.",
	"$n grovels in the dirt before you.",
	"That seems a little silly to me.",
	NULL
    },

    {
	"growl",
	"Grrrrrrrrrr ...",
	"$n growls.",
	"Grrrrrrrrrr ... take that, $N!",
	"$n growls at $N.  Better leave the room before the fighting starts.",
	"$n growls at you.  Hey, two can play it that way!",
	"You growl at yourself.  Boy, do you feel bitter!",
	"$n growls at $mself.  This could get interesting..."
    },

    {
	"grumble",
	"You grumble.",
	"$n grumbles.",
	"You grumble to $M.",
	"$n grumbles to $N.",
	"$n grumbles to you.",
	"You grumble under your breath.",
	"$n grumbles under $s breath."
    },

    {
	"grunt",
	"GRNNNHTTTT.",
	"$n grunts like a pig.",
	"GRNNNHTTTT.",
	"$n grunts to $N.  What a pig!",
	"$n grunts to you.  What a pig!",
	"GRNNNHTTTT.",
	"$n grunts to nobody in particular.  What a pig!"
    },

    {
	"hand",
	"Kiss whose hand?",
	NULL,
	"You kiss $S hand.",
	"$n kisses $N's hand.  How continental!",
	"$n kisses your hand.  How continental!",
	"You kiss your own hand.",
	"$n kisses $s own hand."
    },

    {
	"hop",
	"You hop around like a little kid.",
	"$n hops around like a little kid.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"hug",
	"Hug whom?",
	NULL,
	"You hug $M.",
	"$n hugs $N.",
	"$n hugs you.",
	"You hug yourself.",
	"$n hugs $mself in a vain attempt to get friendship."
    },

    {
	"kiss",
	"Isn't there someone you want to kiss?",
	NULL,
	"You kiss $M.",
	"$n kisses $N.",
	"$n kisses you.",
	"All the lonely people :(",
	"$n kisses $mself, $e must be lonely :("
    },

    {
	"laugh",
	"You laugh.",
	"$n laughs.",
	"You laugh at $N mercilessly.",
	"$n laughs at $N mercilessly.",
	"$n laughs at you mercilessly.  Hmmmmph.",
	"You laugh at yourself.  I would, too.",
	"$n laughs at $mself.  Let's all join in!!!"
    },

    {
	"think",
	"You close your eyes and think very hard.",
	"$n closes $s eyes and thinks very very hard.",
	"You close your eyes and think very hard about $N.",
	"$n closes $s eyes and thinks very hard about $N.",
	"$n closes $s eyes and thinks very hard about you.",
	"You start to wonder about yourself, what am I doing here?",
	"$n starts to wonder about $mself, where did $e come from?"
    },

    {
	"roll",
	"You roll eyeballs, look nice back there?",
	"$n rolls $s eyeballs.",
	"You roll your eyeballs at $N.",
	"$n rolls $s eyeballs at $N.",
	"$n rolls $s eyeballs at you, hmph.",
	"You mess around with your eyeballs, and they pop out!",
	"$n messes around with $s eyeballs, and they pop out! Gross!"
    },
    
    {
	"raise",
	"You raise an eyebrow.",
	"$n raises an eyebrow.",
	"You raise an eyebrow inquisitively at $N.",
	"$n raises an eyebrow at $N.",
	"$n raises an eyebrow inquisitively at you.",
	"You fiddle around with your eyebrows, making them dance.",
	"$n fiddles around with $s eyebrows, making them dance..How weird."
    },
 
    {
	"rofl",
	"You roll around on the floor laughing.",
	"$n rolls around on the floor laughing.",
	"You roll around on the floor laughing at $N.",
	"$n rolls around on the floor laughing at $N.",
	"$n rolls around on the floor laughing at you.",
	"You roll around on the floor laughing at yourself, goober!",
	"$n rolls around on the floor laughing at $mself, what a goof."
    },

    {
	"rock",
	"You rock the MUD!",
	"$n rocks the MUD!",
	"You think $N rocks the MUD!",
	"$n thinks $N rocks the MUD!",
	"$n thinks you rock the MUD! Yea!",
	"You proudly state you think you rock the mud, yea right.",
	"$n proudly states $e thinks $e rocks the mud, yea right."
    },

    {
	"blah",
	"You go 'BLAH!'",
	"$n goes 'BLAH!'",
	"You go 'BLAH!' at $N.",
	"$n goes 'BLAH!' at $N.",
	"$n goes 'BLAH!' at you.",
	"You scream 'BLAH! BLAH!' at yourself.",
	"$n screams 'BLAH! BLAH!' at $mself, they must be frustrated."
    },
   
    {
	"bop",
	"Who do you want to bop?",
	NULL,
	"You bop $N on the head.",
	"$n bops $N on the head.",
	"$n bops you on the head, hey!",
	"You bop yourself on the head, ouch.",
	"$n bops $mself on the head."
    },
 
    {
	"goof",
	"You goof around.",
	"$n goofs around.",
	"You goof around with $N.",
	"$n goofs around with $N.",
	"$n goofs around with you.",
	"You goof around with yourself, uh oh....",
	"$n goofs around with $mself, uhhh...."
    },

    {
	"muah",
	"You turn and scream 'MUAHAHAHAHAHA!'",
	"$n turns and screams 'MUAHAHAHAHAHA!'",
	"You turn to $N and scream 'MUAHAHAHAHA!'",
	"$n turns to $N and screams 'MUAHAHAHAHA!'",
	"$n turns to you and screams 'MUAHAHAHAHA!'",
	"You scream at yourself 'MUAHAHAHAHAHAHAHAHA!!!!'",
	"$n turns to $mself and screams 'MUAHAHAHAHAHAHAHAHA!!!!'.. psycho!"
    },

    {
	"pounce",
	"Who do you want to pounce?",
	NULL,
	"You pounce on $N and introduce yourself.",
	"$n pounces on $N and introduces $mself, 'Hi my name is $n'",
	"$n pounces on you and introduces $mself, 'Hi my name is $n'",
	"You pounce on yourself!",
	"$n pounces on $mself!"
    },

    {
	"afk",
	"You are going away from keyboard for a few moments.",
	"$n is going away from keyboard for a few moments.",
	"You let $N know your going away from keyboard for a few moments.",
	"$n lets $N know they are going away from keyboard for a few moments.",
	"$n lets you know hes going away from keyboard for a few moments.",
	"You let yourself know your going away from keyboard, okay, got it!",
	"$n lets $mself know $e will be going away from keyboard, okay, $es got it!"
    },

    {
	"brb",
	"You will be right back.",
	"$n will be right back.",
	"You let $N know your going to be right back.",
	"$n lets $N know $e is going to be right back.",
	"$n lets you know $e will be right back.",
	"You let yourself know you will be right back, wait up!",
	"$n lets $mself know $e will be right back, sure, uhh, yea."
    },
   
    {
	"twirl",
	"You twirl around.",
	"$n twirls around.",
	"You twirl around with $N.",
	"$n twirls around with $N.",
	"$n twirls around with you.",
	"You twirl around with yourself, getting a little dizzy!",
	"$n twirls around with $mself, getting dizzy."
    },
	
    {                             
	"hmm",
	"You go 'Hmmmm'",
	"$n goes 'Hmmmm'",
	"You go 'Hmmmm' at $N.",
	"$n goes 'Hmmmm' at $N.",
	"$n goes 'Hmmmm' at you.",
	"You go 'Hmmmm' to yourself.",
	"$n goes 'Hmmmm' to $mself."
    },

    {
	"tap",
	"You tap your foot impatiently.",
	"$n taps $s foot impatiently, hurry up!",
	"You tap your foot at $N.",
	"$n taps $s foot at $N.",
	"$n taps $s foot at you, whats taking so long??",
	"You start to tap dance, whee.",
	"$n tap dances around the room, look at $m go!"
    },  
    {
	"snuke",
	"You tried to smile but your hands were on the wrong keys!",
	"$n tried to smile but $s hands were on the wrong keys!",
	"You try to smile at $N but your hands were on the wrong keys!",
	"$n tried to smile at $N but $s hands were on the wrong keys!",
	"$n tried to smile at you but $s hands were on the wrong keys!",
	"You try to smile at yourself but your hands were on the wrong keys!",
	"$n tries to smile at $mself but types it in horribly! What a freak!"
    },

    {
	"slap",
	"Who do you want to slap?",
	NULL,
	"You slap $N!",
	"$n slaps $N!",
	"$n slaps you!",
	"You slap yourself, bad flap!",
	"$n slaps $mself, what a bad flap!"
    },

    {
	"smack",
	"Who do you want to smack?",
	NULL,
	"You smack $N upside the head!",
	"$n smacks $N upside the head!",
	"$n smacks you upside the head! Ouch!",
	"You smack yourself upside the head, DUH!",
	"$n smacks $mself upside the head, DUH!"
    },

    {
	"boggle",
	"You boggle at the concept.",
	"$n boggles at the concept.",
	"You boggle at $N.",
	"$n boggles at $N.",
	"$n boggles at you.",
	"You boggle at your own existence.",
	"$n boggles at $s own existence."
    },

    {
	"taunt",
	"You taunt the gods.",
	"$n taunts the gods.",
	"You taunt $N sadistically.",
	"$n taunts $N sadistically.",
	"$n taunts you sadistically.",
	"You taunt yourself, are you okay?",
	"$n taunts $mself schizophrenically"
    },

    {
	"cheer",
	"You cheer wildly.",
	"$n cheers wildly!",
	"You cheer at $N wildly, WOO WOO!",
	"$n cheers at $N wildly, WOO WOO!",
	"$n cheers at you wildly, WOO WOO!",
	"You cheer wildly at yourself! Nice ego, flap-$y.",
	"$n cheers wildly for $mself, what an ego!"
    },

    {
	"ack",
	"ACK!",
	"$n ACKS!",
	"You ack at $N, look at that!",
	"$n acks at $N.",
	"$n acks at you, what did you do now?",
	"You ack at yourself, what have you done now?!",
	"$n acks at $mself, what has $e done now?!"
    },

    {
	"whine",
	"WHINE! I cant believe $e did that! What a cheater!",
	"$n WHINES! You cheated! You didnt miss a thing!",
	"You whine at $N.",
	"$n whines at $N.",
	"$n whines at you!",
	"You whine at yourself...?",
	"$n whines at $mself, $e must be upset."
    },

    {
	"spit",
	"You spit over your left shoulder.",
	"$n spits over $s left shoulder.",
	"You spit at $N.",
	"$n spits at $N.",
	"$n spits at you.",
	"You spit on your hands and smooth your hair.",
	"$n spits at $s hands and smooths $m hair."
    },

    {
	"spike",
	"You spike a volleyball in the ground.",
	"$n spikes a volleyball in the ground.",
	"You spike a volleyball in $N's face!",
	"$n spikes a volleyball in $N's face!",
	"$n spikes a volleyball in your face!",
	NULL,
	NULL
    },

    {
	"play",
	"You play with your toys.",
	"$n plays with $s toys.",
	"You share your toys with $N.",
	"$n shares $s toys with $N.",
	"$n shares $s toys with you.",
	"You play with yourself.. bored?",
	"$n plays with $mself.. and you get to watch!"
    },

    {
	"clean",
	"You clean up the room, how nice.",
	"$n cleans up the room, all spic and span.",
	"You clean $N with your tongue.",
	"$n cleans $N with $s tongue.",
	"$n cleans you with $s tongue!",
	"You clean yourself with your tongue, yummy.",
	"$n cleans $mself with $s tongue."
    },
	
    {
	"mosh",
	"You mosh violently around the room!",
	"$n moshes violently around the room!",
	"You slam into $N as you mash madly around the room!",
	"$n smashes into $N as $e moshes madly around the room!",
	"$n smashes into you as $e moshes madly around the room!",
	"You go into wild convulsions and mosh like a maniac!",
	"$n goes into wild convulsions and moshes around like a maniac! Run!"
    },

    {
	"lick",
	"You lick your lips and smile.",
	"$n licks $s lips and smiles.",
	"You lick $M.",
	"$n licks $N.",
	"$n licks you.",
	"You lick yourself.",
	"$n licks $mself - YUCK."
    },

    {
	"love",
	"You love the whole world.",
	"$n loves everybody in the world.",
	"You tell your true feelings to $N.",
	"$n whispers softly to $N.",
	"$n whispers to you sweet words of love.",
	"Well, we already know you love yourself (lucky someone does!)",
	"$n loves $mself, can you believe it?"
    },

    {
	"massage",
	"Massage what?  Thin air?",
	NULL,
	"You gently massage $N's shoulders.",
	"$n massages $N's shoulders.",
	"$n gently massages your shoulders.  Ahhhhhhhhhh ...",
	"You practice yoga as you try to massage yourself.",
	"$n gives a show on yoga positions, trying to massage $mself."
    },

    {
	"moan",
	"You start to moan.",
	"$n starts moaning.",
	"You close your eyes and moan while thinking about $N.",
	"$n moans at the thought of $N.",
	"$n closes $s eyes and moans while thinking about you.  Hmmmm.",
	"You moan at yourself.",
	"$n makes $mself moan."
    },

    {
	"nibble",
	"You nibble anxiously on your fingernails",
	"$n nibbles on $s fingernails anxiously",
	"You nibble on $N's ear.",
	"$n nibbles on $N's ear.",
	"$n nibbles on your ear.",
	"You nibble on your OWN ear.",
	"$n nibbles on $s OWN ear."
    },

    {
	"nod",
	"You nod solemnly.",
	"$n nods solemnly.",
	"You nod in recognition to $M.",
	"$n nods in recognition to $N.",
	"$n nods in recognition to you.",
	"You nod at yourself.",
	"$n nods at $mself."
    },

    {
	"nudge",
	"Nudge whom?",
	NULL,
	"You nudge $M.",
	"$n nudges $N.",
	"$n nudges you.",
	"You nudge yourself, for some strange reason.",
	"$n nudges $mself, to keep $mself awake."
    },

    {
	"nuzzle",
	"Nuzzle whom?",
	NULL,
	"You nuzzle $S neck softly.",
	"$n softly nuzzles $N's neck.",
	"$n softly nuzzles your neck.",
	"I'm sorry, friend, but that's impossible.",
	NULL
    },

    {
	"pat",
	"Pat whom?",
	NULL,
	"You pat $N on $S head.",
	"$n pats $N on $S head.",
	"$n pats you on your head.",
	NULL,
	NULL
    },

/* added, dunno where else i need to put it */
    {
	"peer",
	"You peer all around.",
	"$n peers around the room.",
	"You peer at $M intently.",
	"$n peers at $N intently.",
	"$n peers at you intently.",
	NULL,
	NULL
    },

    {
	"point",
	"Point at whom?",
	NULL,
	"You point at $M accusingly.",
	"$n points at $N accusingly.",
	"$n points at you accusingly.",
	"You point proudly at yourself.",
	"$n points proudly at $mself."
    },

    {
	"poke",
	"Poke whom?",
	NULL,
	"You poke $M in the ribs.",
	"$n pokes $N in the ribs.",
	"$n pokes you in the ribs.",
	"You poke yourself in the ribs, feeling very silly.",
	"$n pokes $mself in the ribs, looking very sheepish."
    },

    {
	"ponder",
	"You ponder the question.",
	"$n sits down and thinks deeply.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"pout",
	"Ah, don't take it so hard.",
	"$n pouts.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"pray",
	"You feel righteous, and maybe a little foolish.",
	"$n begs and grovels to the powers that be.",
	"You crawl in the dust before $M.",
	"$n falls down and grovels in the dirt before $N.",
	"$n kisses the dirt at your feet.",
	"Talk about narcissism ...",
	"$n mumbles a prayer to $mself."
    },

    {
	"puke",
	"You puke ... chunks everywhere!",
	"$n pukes.",
	"You puke on $M.",
	"$n pukes on $N.",
	"$n pukes and spews vomit all over your clothing!",
	"You puke on yourself.",
	"$n pukes on $s clothes."
    },

    {
	"punch",
	"You drink some punch.",
	"$n drinks some punch.",
	"You punch $M playfully.",
	"$n punches $N playfully.",
	"$n punches you playfully.  OUCH!",
	"You punch yourself.  You deserve it.",
	"$n punches $mself.  Why don't you join in?"
    },

    {
	"purr",
	"MMMMEEEEEEEEOOOOOOOOOWWWWWWWWWWWW.",
	"$n purrs contentedly.",
	"You purr contentedly in $S lap.",
	"$n purrs contentedly in $N's lap.",
	"$n purrs contentedly in your lap.",
	"You purr at yourself.",
	"$n purrs at $mself.  Must be a cat thing."
    },

    {
	"ruffle",
	"You've got to ruffle SOMEONE.",
	NULL,
	"You ruffle $N's hair playfully.",
	"$n ruffles $N's hair playfully.",
	"$n ruffles your hair playfully.",
	"You ruffle your hair.",
	"$n ruffles $s hair."
    },

    {
	"scream",
	"ARRRRRRRRRRGH!!!!!",
	"$n screams loudly!",
	"ARRRRRRRRRRGH!!!!!  Yes, it MUST have been $S fault!!!",
	"$n screams loudly at $N.  Better leave before $n blames you, too!!!",
	"$n screams at you!  That's not nice!  *sniff*",
	"You scream at yourself.  Yes, that's ONE way of relieving tension!",
	"$n screams loudly at $mself!  Is there a full moon up?"
    },

    {
	"shake",
	"You shake your head.",
	"$n shakes $s head.",
	"You shake $S hand.",
	"$n shakes $N's hand.",
	"$n shakes your hand.",
	"You are shaken by yourself.",
	"$n shakes and quivers like a bowl full of jelly."
    },

    {
	"shiver",
	"Brrrrrrrrr.",
	"$n shivers uncomfortably.",
	"You shiver at the thought of fighting $M.",
	"$n shivers at the thought of fighting $N.",
	"$n shivers at the suicidal thought of fighting you.",
	"You shiver to yourself?",
	"$n scares $mself to shivers."
    },

    {
	"shrug",
	"You shrug.",
	"$n shrugs helplessly.",
	"You shrug in response to $s question.",
	"$n shrugs in response to $N's question.",
	"$n shrugs in respopnse to your question.",
	"You shrug to yourself.",
	"$n shrugs to $mself.  What a strange person."
    },

    {
	"sigh",
	"You sigh.",
	"$n sighs loudly.",
	"You sigh as you think of $M.",
	"$n sighs at the sight of $N.",
	"$n sighs as $e thinks of you.  Touching, huh?",
	"You sigh at yourself.  You MUST be lonely.",
	"$n sighs at $mself.  What a sorry sight."
    },

    {
	"sing",
	"You raise your clear voice towards the sky.",
	"PANIC ! $n has begun to sing.",
	"You sing a ballad to $M.",
	"$n sings a ballad to $N.",
	"$n sings a ballad to you!  How sweet!",
	"You sing a little ditty to yourself.",
	"$n sings a little ditty to $mself."
    },

    {
	"smile",
	"You smile happily.",
	"$n smiles happily.",
	"You smile at $M.",
	"$n beams a smile at $N.",
	"$n smiles at you.",
	"You smile at yourself.",
	"$n smiles at $mself."
    },

    {
	"smirk",
	"You smirk.",
	"$n smirks.",
	"You smirk at $S saying.",
	"$n smirks at $N's saying.",
	"$n smirks at your saying.",
	"You smirk at yourself.  Okay ...",
	"$n smirks at $s own 'wisdom'."
    },

    {
	"snap",
	"You snap your fingers.. DAMN!",
	"$n snaps $s fingers... DAMN!",
	"You snap back at $M.",
	"$n snaps back at $N.",
	"$n snaps back at you!",
	"You snap yourself to attention.",
	"$n snaps $mself to attention."
    },

    {
	"snarl",
	"You grizzle your teeth and look mean.",
	"$n snarls angrily.",
	"You snarl at $M.",
	"$n snarls at $N.",
	"$n snarls at you, for some reason.",
	"You snarl at yourself.",
	"$n snarls at $mself."
    },

    {
	"sneeze",
	"Gesundheit!",
	"$n sneezes.",
	"You sneeze all over $N.",
	"$n sneezes on $N.",
	"$n sneezes all over you.. ick!",
	NULL,
	NULL
    },

    {
	"snicker",
	"You snicker softly.",
	"$n snickers softly.",
	"You snicker with $M about your shared secret.",
	"$n snickers with $N about their shared secret.",
	"$n snickers with you about your shared secret.",
	"You snicker at your own evil thoughts.",
	"$n snickers at $s own evil thoughts."
    },

    {
	"sniff",
	"You sniff sadly. *SNIFF*",
	"$n sniffs sadly.",
	"You sniff sadly at the way $E is treating you.",
	"$n sniffs sadly at the way $N is treating $m.",
	"$n sniffs sadly at the way you are treating $m.",
	"You sniff sadly at your lost opportunities.",
	"$n sniffs sadly at $mself.  Something MUST be bothering $m."
    },

    {
	"snore",
	"Zzzzzzzzzzzzzzzzz.",
	"$n snores loudly.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },
				   
    {
	"snowball",
	"Whom do you want to throw a snowball at?",
	NULL,
	"You throw a snowball in $N's face.",
	"$n throws a snowball at $N.",
	"$n throws a snowball at you.",
	"You throw a snowball at yourself.",
	"$n throws a snowball at $mself."
    },

    {
	"snuggle",
	"Who?",
	NULL,
	"you snuggle $M.",
	"$n snuggles up to $N.",
	"$n snuggles up to you.",
	"You snuggle up, getting ready to sleep.",
	"$n snuggles up, getting ready to sleep."
    },

    {
	"spank",
	"Spank whom?",
	NULL,
	"You spank $M playfully.",
	"$n spanks $N playfully.",
	"$n spanks you playfully.  OUCH!",
	"You spank yourself.  Kinky!",
	"$n spanks $mself.  Kinky!"
    },

    {
	"squeeze",
	"Where, what, how, whom?",
	NULL,
	"You squeeze $M fondly.",
	"$n squeezes $N fondly.",
	"$n squeezes you fondly.",
	"You squeeze yourself - try to relax a little!",
	"$n squeezes $mself."
    },

    {
	"stare",
	"You stare at the sky.",
	"$n stares at the sky.",
	"You stare dreamily at $N, completely lost in $S eyes..",
	"$n stares dreamily at $N.",
	"$n stares dreamily at you, completely lost in your eyes.",
	"You stare dreamily at yourself - enough narcissism for now.",
	"$n stares dreamily at $mself - NARCISSIST!"
    },

    {
	"strut",
	"Strut your stuff.",
	"$n struts proudly.",
	"You strut to get $S attention.",
	"$n struts, hoping to get $N's attention.",
	"$n struts, hoping to get your attention.",
	"You strut to yourself, lost in your own world.",
	"$n struts to $mself, lost in $s own world."
    },

    {
	"sulk",
	"You sulk.",
	"$n sulks in the corner.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"thank",
	"Thank you too.",
	NULL,
	"You thank $N heartily.",
	"$n thanks $N heartily.",
	"$n thanks you heartily.",
	"You thank yourself since nobody else wants to !",
	"$n thanks $mself since you won't."
    },

    {
	"tickle",
	"Whom do you want to tickle?",
	NULL,
	"You tickle $N.",
	"$n tickles $N.",
	"$n tickles you - hee hee hee.",
	"You tickle yourself, how funny!",
	"$n tickles $mself."
    },

    {
	"twiddle",
	"You patiently twiddle your thumbs.",
	"$n patiently twiddles $s thumbs.",
	"You twiddle $S ears.",
	"$n twiddles $N's ears.",
	"$n twiddles your ears.",
	"You twiddle your ears like Dumbo.",
	"$n twiddles $s own ears like Dumbo."
    },

    {
	"wave",
	"You wave.",
	"$n waves happily.",
	"You wave goodbye to $N.",
	"$n waves goodbye to $N.",
	"$n waves goodbye to you.  Have a good journey.",
	"Are you going on adventures as well?",
	"$n waves goodbye to $mself."
    },

    {
	"whistle",
	"You whistle appreciatively.",
	"$n whistles appreciatively.",
	"You whistle at the sight of $M.",
	"$n whistles at the sight of $N.",
	"$n whistles at the sight of you.",
	"You whistle a little tune to yourself.",
	"$n whistles a little tune to $mself."
    },

    {
	"wiggle",
	"Your wiggle your bottom.",
	"$n wiggles $s bottom.",
	"You wiggle your bottom toward $M.",
	"$n wiggles $s bottom toward $N.",
	"$n wiggles $s bottom toward you.",
	"You wiggle about like a fish.",
	"$n wiggles about like a fish."
    },

    {
	"wince",
	"You wince.  Ouch!",
	"$n winces.  Ouch!",
	"You wince at $M.",
	"$n winces at $N.",
	"$n winces at you.",
	"You wince at yourself.  Ouch!",
	"$n winces at $mself.  Ouch!"
    },

    {
	"wink",
	"You wink suggestively.",
	"$n winks suggestively.",
	"You wink suggestively at $N.",
	"$n winks at $N.",
	"$n winks suggestively at you.",
	"You wink at yourself ?? - what are you up to ?",
	"$n winks at $mself - something strange is going on..."
    },

    {
	"yawn",
	"You must be tired.",
	"$n yawns.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"high5",
	"You put up your hand to high-five everyone, but no one responds.",
	"$n puts $s hand up to get high-fives, but no one responds.. what a loser!",
	"You high-five $N.. woo hoo!",
	"$n high-fives $N.",
	"$n high-fives you.. woo hoo!", 
	"You high-five yourself.. um.. woo hoo?",
	"$n high-fives $mself.. guess $e has no friends."
    },

    {
	"scold",
	"You scold everyone.. tsk! tsk!",
	"$n scolds everyone.. tsk! tsk!",
	"You scold $N sternly for what $E did.",
	"$n scolds $M.",
	"$n scolds you sternly for what you did.",
	"You scold yourself.. lighten up, it's just a game!",
	"$n scolds $mself.. wonder if $e'll get grounded."
     },

    {
	"gouge",
	"Pick a victim to gouge, you knob!",
	NULL,
	"You gouge out $N's eyes with a rusty spoon.",
	"$n gouges out $N's eyes.",
	"$n gouges out your eyes with a rusty spoon.. ouch!",
	"You gouge your own eyes out.. a little harsh, don't you think?",
	"$n gouges out $s own eyes.. Yeesh.. how gross."
    },

    {
	"stomp",
	"You stomp around angrily.",
	"$n stomps around angrily.",
	"You stomp on $S foot.",
	"$n stomps on $N's foot.",
	"$n stomps on your foot.",
	NULL,
	NULL

    },

    {
	"shudder",
	"You shudder.",
	"$n shudders.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"blink",
	"You blink your eyes innocently.",
	"$n blinks $s eyes innocently.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"hum",
	"You hum innocently.",
	"$n hums innocently.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"threaten",
	"You threaten everyone in the room with certain death.",
	"$n threatens everyone with certain death.",
	"You threaten $N.. $E must be shaking in fear!",
	"$n threatens $N.",
	"$n threatens you.. run away! run away!",
	NULL,
	NULL
    },

    {
	"gulp",
	"You gulp.",
	"$n gulps.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"stroke",
	"who?",
	NULL,
	"You stroke $S thigh with feathery touches.",
	NULL,
	"$n lightly strokes your thigh with feathery touches.",
	NULL,
	NULL
    },

    {
	"embrace",
	"You try to engulf everyone in a great big hug!",
	"$n tries to hug everyone.. touching, isn't it?",
	"You embrace $M warmly.",
	"$n embraces $N",
	"$n embraces you warmly.",
	"You embrace yourself.. why?",
	"$n embraces $mself.. hmm... must be lonely"
    },

    {
	"blow",
	"In who's ear??",
	NULL,
	"You blow softly in $S ear.",
	"$n blows in $N's ear.",
	"$n blows softly in your ear.",
	NULL,
	NULL
    },

    {
	"sob",
	"You sob.",
	"$n covers $s head with $s hands and sobs.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"faint",
	"You faint.",
	"$n faints.",
	"You faint into $N's arms.",
	"$n faints into $N's arms.",
	"$n faints into your arms.",
	"You realize what you did and faint into a crumpled heap.",
	"$n realizes what $e did and faints into a crumpled heal."
    },

    {
	"lust",
	"A gleam of lust fills your eyes.",
	"A gleam of lust fills $n's eyes.",
	"Your eyes fill with lust as you look at $N.",
	"$n lusts over $N",
	"$n's eyes fill with lust as $e looks at you.",
	"You lust over yourself.. somebody might as well do it.",
	"$n lusts over $mself.. guess $e sees something $e likes."
    },

    {   "flex",
	"You flex your mighty muscles... OOooooo!",
	"$n flexes what $e tries to pass off as muscles.",
	"You flex your muscles at $N, trying to impress $M.",
	"$n flexes $s muscles for $N.",
	"$n flexes $s muscles in an attempt to impress you.",
	"You show how flexible you are by putting your foot behind your head.",
	"$n shows how flexible $e is by putting $s foot behind $s head."
    },

    {   "caress",
	"Caress who?",
	NULL,
	"You caress $M with soft, loving touches.",
	"$n caresses $N lovingly.",
	"$n caresses you with soft, loving touches.",
	"You must have really soft skin.",
	"$n caresses $mself with soft, loving touches."
    },

    {
	"shy",
	"Awww.. people aren't that scary, are they?",
	"$n bites $s lip bashfully",
	"You suddenly feel shy and ackward around $M.",
	"$n is shy around $N.",
	"For some reason, $n feels shy around you.",
	NULL,
	NULL
    },

    {
	"undress",
	"You start to slowly remove your clothing.",
	"$n starts to remove $s clothing.",
	"You undress $M with your eyes.",
	"$n undresses $N with $s eyes.",
	"$n stares intently at you and begins undressing you with $s eyes.",
	"Feeling a bit warm?",
	NULL
    },

    {
	"stick",
	"You stick your tongue out.",
	"$n sticks $s tongue out.",
	"You stick your tongue out at $N.",
	"$n sticks $s tongue out at $N.",
	"$n sticks $s tongue out at you.",
	"You stick your tongue out and touch the tip of your nose with it.",
	"$n sticks $s tongue out and touches the tip of $s nose with it."
    },

    {                                                                         
        "nuzslam",                                                              
        "on who??",                                                   
        NULL,                                                                
        "You slam $N into the wall and nuzzle $M neck.",                      
        NULL,                                         
        "$n slams you against the wall and nuzzles your neck.",         
        "You act like a freak.",                        
        "$n acts like a freak."                      
    },                                                                    
      
    {
	"tackle",
	"Going fishing??",
	NULL,
	"You run towards $N and tackle $M to the ground.",
	"$n tackles $N to the ground.",
	"$n runs quickly towards you and tackles you to the ground.",
	"You tackle yourself and fall to the ground.",
	"$n tackles $mself and falls to the ground."
    },

    {
	"stone",
	"Dude.. the colors!",
	"$n pulls out a funny looking cigarette and smokes up.",
	"You throw stones at $N, trying to drive $M from your village.",
	"$n throws stones at $N.",
	"$n throws stones at you.  It seems you're not wanted here.",
	"You throw stones at yourself in disgust.",
	"$n throws stones at $mself in disgust."
    },

    {
	"beckon",
	"You beckon everyone to listen.",
	"$n beckons everyone to pay attention.",
	"You beckon $N to join you.",
	"$n beckons $N.",
	"$n beckons you to join $m.",
	NULL,
	NULL
    },

    {
	"tug",
	"Tug on whose sleeve?",
	"$n is getting impatient.",
	"You tug on $N's sleeve.",
	"$n tugs on $N's sleeve.",
	"$n tugs on your sleeve.. what does $e what??",
	"You tug on your ear.",
	"$n tugs on $s ear."
    },

    {
	"rootbeer",
	"You down a cold, frothy rootbeer.",
	"$n downs a cold, frothy rootbeer.",
	"You draw a cold, frothy rootbeer for $N.",
	"$n draws a cold, frothy rootbeer for $N.",
	"$n draws a cold, frothy rootbeer for you.",
	"You draw a cold, frothy rootbeer for yourself.",
	"$n draws a cold, frothy rootbeer for $mself."
    },

    {
	"juggle",
	"You juggle.",
	"$n juggles. (Just be glad $e's not a mime, ok?)",
	"Just what do you think you're doing with $M??",
	"$n tosses $N into the air and tries to juggle with $M.",
	"$n tosses you in the air and juggles you with your equipment.",
	"You juggle your equipment in the air.. fly, demon blade, fly!",
	"$n juggles $s equipment in the air.. look at that helmet fly!"
    },

    {
	"laces",
	"Who do you want to torment?",
	"$n is looking for a victim to torment.",
	"You tie $N's shoelaces together when $e isn't looking.. (snicker).",
	"$n ties $N's shoelaces together.",
	"$n ties your shoelaces together while you aren't looking.. doh.",
	NULL,
	NULL
    },

    {
	"finish",
	"Done so soon?",
	NULL,
	"You tear $S arms off $S body and let the blood drip onto $S face.",
	"$n finishes $N by tearing $S arms off.",
	"$n finishes you off by tearing your arms off and showing them too you.",
	"Suicidal tendencies?",
	NULL
    },

    {
	"schwing",
	"SCHWING!!!",
	"$n seems to appreciate the attention $e is receiving.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },

    {
	"fist",
	"You shake your fist angrily.",
	"$n shakes $s fist angrily.",
	"You shake your fist angrily at $M.",
	"$n shakes $s fists angrily at $N.",
	"$n shakes $s fists angrily at you.. what did you do to upset $m?",
	"You shake your fists in frustration over your stupidity.",
	"$n shakes $s fists in frustration over $s stupidity."
    },

    {
	"innocent",
	"No one saw you.  They can't prove anything.",
	"$n tries to look innocent.",
	"You try to look innocent to $M.",
	"$n tries to look innocent to $N.",
	"$n tries to look innocent for you.. yeah right!.",
	NULL,
	NULL
    },

    {
	"skip",
	"You skip along happily.. isn't life grand? :)",
	"$n skips happily, recalling the happier days of $s youth.",
	"You grab $S hand and skip with $M down the road.",
	"$n grabs $N and skips merrily down the road.",
	"$n takes your hand and takes you skipping down the road.",
	"You take out your skipping rope and start getting into shape",
	"$n takes out $s skipping rope and starts getting into shape." 
     },

     {
	"scratch",
	"You scratch you head.. hunh??",
	"$n scratches $s head in confusion.",
	"You scratch your nails down $S back.",
	"$n scratches $s nails down $N's back.",
	"$n scratches $s nails down your back.. AHHHHHHH!",
	"You scratch your itch, hopefully no one noticed.",
	"$n has an itch in a really embarassing spot."
     },

     {
	"eye",
	"You give everyone the evil eye.",
	"$n gives everyone the evil eye.",
	"You give $N the evil eye.",
	"$n gives $N the evil eye.",
	"$n gives you the evil eye.. oooo.. scary.",
	"You check yourself, making sure all you are all in one piece.",
	"$n checks $mself, making sure $e is all still in one piece."
     },

     {
	"sneer",
	"You sneer.",
	"$n sneers.",
	"You sneer at $N.",
	"$n sneers at $N.",
	"$n sneers at you.",
	"You sneer at yourself.",
	"$n sneers at $mself.. odd."
     },

     {
	"howl",
	"You toss your head back and let loose a blood-curdling howl.",
	"$n tosses $s head back and lets loose a blood-curdling howl.",
	"You toss your head back and howl at $N.",
	"$n tosses $s head back and howls at $N.",
	"$n tosses $s head back and howls at you.",
	"You howl in agony over what you just did.",
	"$n howls in agony over what $e just did."
     },

     {
	"cower",
	"You cowers in fear.",
	"$n cowers in fear like the wimp $e is.",
	"You cower in fear from $M.",
	"$n cowers in fear from $N.",
	"$n cowers in fear from you... heh heh.. you're one bad dude!.",
	NULL,
	NULL
     },

     {  
	"fear",
	"You wet your pants in fear.",
	"$n wets $s pants in fear.",
	"You wet your pants in fear of $M.",
	"$n wets $s pants in fear of $N.",
	"$n wets $s pants in fear of you.",
	"You scare yourself to death!.",
	"$n scares $mself to death!."
     },

     {
	"carve",
	"You pull out your carving knife and make a lovely decoy duck.",
	"$n pulls out $s carving knife and makes a lovely decoy duck.",
	"You carve you initials in $S flesh.",
	"$n carves $s initials into $N's flesh.",
	"$n carves $s initials into your flesh.  *ouch* ",
	"You carve your initials into a nearby tree.",
	"$n carves $s initials into a nearby tree so no one will forget $m."
     },

     {
	"strip",
	"You do a slow, sensual strip tease in the corner.",
	"$n does a slow, sensual strip tease in the corner.",
	"You begin a slow, sensual strip tease to get $S attention.",
	"$n begins to strip for $N.",
	"$n begins a slow, sensual strip tease to get your attention.",
	"Overheated are we?",
	NULL
     },

     {
	"freak",
	"You freak out.",
	"$n freaks out.",
	"You freak out at $N.",
	"$n freaks out at $N.",
	"$n freaks out at you.",
	"You freak out at yourself.",
	"$n freaks out at $mself."
     },

     {
	"flutter",
	"You flutter your eyelashes coquettishly.",
	"$n flutters $s eyelashes coquettishly.",
	"You flutter your eyelashes at $M.",
	"$n flutters $s eyelashes at $N.",
	"$n flutters $s eyelashes at you.",
	"You flutter around imitating a butterfly.",
	"$n flutters around the room, trying to look like a butterfly."
     },

     {
	"goose",
	"Who do you want to goose?",
	NULL,
	"You goose $N!",
	"$n gooses $N.",
	"$n gooses you!",
	"Where do you think your hand is going?",
	"$n gooses $mself!?!"
     },

     {
	"lambada",
	"Lambada alone? Don't be shy.. pick a partner.",
	NULL,
	"You press your body close to $S and dance a sultry lambada.",
	"$n dances the lambada with $N.",
	"$n presses $s body close to yours and dances a sultry lambada with you.",
	NULL,
	NULL
     },

     {
	"hiccup",
	"HIC!",
	"$n hiccups.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
     },

     {
	"stagger",
	"You stagger like a drunken fool.",
	"$n staggers like a drunken fool.",
	"You stagger and sway, lurching towards $N.",
	"$n staggers and sways, lurching towards $N.",
	"$n staggers and sways, lurching towards you.",
	NULL,
	NULL
     },

     {
	"snort",
	"You snort derisively.",
	"$n snorts derisively.",
	"You snort derisively at $N.",
	"$n snorts derisively at $N.",
	"$n snorts derisively at you.",
	"You close your eyes and snort, sucking up a huge mass of phlegm.",
	"$n closes $s eyes and snorts, sucking up a huge mass of phlegm."
     },

     {
	"knee",
	"You get down on one knee.",
	"$n gets down on one knee.",
	"You knee $N in the stomach, making $M double over in pain.", 
	"$n knees $N in the stomach.",
	"$n knees you in the stomach.. OOF!",
	"Your knees knock together in fear.",
	"$n's knees knock together in fear."
     },

     {
	"flirt",
	"You start flirting with everyone.",
	"$n starts flirting with everyone in the room.",
	"You flirt with $M.",
	"$n flirts with $N.",
	"$n flirts with you.",
	"You flirt with yourself.. why though?",
	"$n flirts with $mself.. hrm..."
     },

     {
	"swoon",
	"You swoon.",
	"$n swoons.",
	"You swoon over $N.",
	"$n swoons over $N.",
	"$n swoons over you.",
	NULL,
	NULL
     },

     {
	"lightbulb",
	"By George, I think you've got it!",
	"$n finally clues in.",
	"You screw a lightbulb into $N's head, hoping it'll give $M a clue.",
	"$n screws a lightbulb into $N's head to give $M a clue.",
	"$n screws a lightbulb into your head, trying to give you a clue.",
	"You open your skull to see if your lightbulb has burnt out.",
	"$n opens $s skull to see if $s lightbulb has burnt out."
     },

     {
	"tag",
	"Tag whom?",
	NULL,
	"You tag $N.. $S's it!",
	"$n tags $N.. care to join in on the game?",
	"$n tags you.. you're it!",
	"You put a tag on your toe and prepare for your death.",
	"$n puts a tag on $s toe and prepares for $s death."
     },

     {
	"bkiss",
	"You blow a kiss in the air.",
	"$n blows a kiss in the air.",
	"You blow a kiss towards $M as $E leaves.",
	"$n blows a kiss at $N.",
	"$n blows a kiss towards you as you leave.",
	NULL,
	NULL
     },

     {
	"insane",
	"You begin to lose your grip on reality",
	"$n feels $s grip on reality begin to loosen.",
	"You laugh in an insane fury at $N.",
	"$n laughs in an insane fury at $N.",
	"$n laughs in an insane fury at you.",
	"You laugh insanely at yourself.",
	"$n laughs insanely at $mself."
     },

     {
	"peck",
	"Isn't there anyone you want to give a quick kiss to?",
	NULL,
	"You peck $M on the cheek.",
	"$n pecks $N on the cheek.",
	"$n pecks you on the cheek.",
	NULL,
	NULL
     },

     {
	"mutter",
	"You mutter quietly to yourself.",
	"$n mutters quietly to $mself.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
     },

     {
	"flinch",
	"You flinch.",
	"$n flinches.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
     },

     {
	"grimace",
	"You grimace in obvious pain.",
	"$n grimaces in obvious pain.",
	"You grimace in pain at $N.",
	"$n grimaces in pain at $N.",
	"$n grimaces in pain at you.",
	NULL,
	NULL
     },

     {
	"apologize",
	"You apologize until you are blue in the face.",
	"$n apologizes until $e is blue in the face.",
	"You apologize to $N.",
	"$n apologizes to $N.",
	"$n apologizes to you.",
	"You beg yourself for forgiveness.  Don't give in.",
	"$n apologizes to $mself for being such a dolt."
     },

     {
	"cough",
	"Yuck!  Cover your mouth next time.",
	"$n coughs.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
     },

     {
	"wait",
	"You wait.",
	"$n waits.",
	"You wait for $N.",
	"$n waits for $N.",
	"$n is waiting for you.",
	NULL,
	NULL
     },

     {
	"cover",
	"You cover your eyes. You don't want to see this!.",
	"$n covers $s eyes.. $e doesn't want to see this!.",
	"You cover $S eyes with your hands so $E doesn't see this.",
	"$n covers $N's eyes so $E doesn't see this.",
	"$n covers your eyes with $s hands.. what doesn't $e want you to see?",
	"You cover your face, embarassed by what you have done.",
	"$n covers $s face, embarassed by what $e has done."
     },

     {
	"plead",
	"You beg and plead to anyone who will listen.",
	"$n begs and pleads with anyone who will listen.",
	"You plead with $N, trying to get your way.",
	"$n pleads with $N.",
	"$n pleads with you, trying to get $s way.",  
	"You plead with yourself.. do it right this time!.",
	"$n pleads with $mself to do it right this time."
     },

     {
	"squeal",
	"You squeal with delight",
	"$n squeals with delight.",
	"You squeal with delight at $N.",
	"$n squeals with delight at $N.",
	"$n squeals with delight at you.",
	"You squeal with delight at yourself.",
	"$n squeals with delight at $mself."
     },

     {
	"footsie",
	"Who do you want to play footsie with?",
	NULL,
	"You take off your shoe and slide your toes up $N's leg.",
	NULL,
	"You feel $n's toes slide up your leg.. I think $e likes you!",
	NULL,
	NULL
     },

     {
	"tango",
	"Are you looking for a dance partner?",
	"$n seems to be looking for that special someone.",
	"You place a rose between your lips and grab $N for a tango!",
	"$n tangos across the dancefloor with $N.",
	"$n places a rose between $s lips and invites you for a tango.",
	NULL,
	NULL
     },

     {
	"eskimo",
	"Rub noses with whom?",
	NULL,
	"You wraps your arms around $S shoulders and rub noses with $M.",
	"$n rubs noses with $N.",
	"$n wraps $s arms around you and rubs noses with you.",
	"You can't do that.",
	NULL
     },

     {
	"salute",
	"You salute.",
	"$n salutes.",
	"You salute $M.",
	"$n salutes $N.",
	"$n salutes you.",
	"You salute yourself.. aye aye cap'n!",
	"$n salutes $mself. Hrm."
     },

     {
	"catnap",
	"You curl up in the corner and fall asleep.",
	"$n curls up in the corner and falls asleep.",
	"You curl up in $S lap and fall asleep.",
	"$n curls up in $N's lap and falls asleep.",
	"$n curls up in your lap and falls asleep.",
	NULL,
	NULL
     },
     {	"bleed",
	"You bleed all over the floor.",
	"$n bleeds all over the floor.",
	"You bleed all over $N's lap.",
	"$n bleeds all over $N.",
	"$n bleeds all over you.",
	"You bleed all over yourself.",
	"$n bleeds all over $mself."
     
     },
     {
	"",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
};



/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;
    bool social;
    bool docommand;
    OBJ_DATA *obj_check;
    OBJ_DATA *obj_next;


    if ( !IS_NPC( ch ) )
      REMOVE_BIT( ch->act, PLR_CMD_SUCCESS );

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * No hiding.
     */
    REMOVE_BIT( ch->affected_by, AFF_HIDE );

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace(*argument) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	&&   !str_prefix( command, cmd_table[cmd].name )
	&&   cmd_table[cmd].level <= trust )
	{
	    found = TRUE;
	    break;
	}
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   cmd_table[cmd].log == LOG_ALWAYS )
    { 
	sprintf( log_buf, "Log %s (%d): %s", ch->name, ch->in_room->vnum, logline );
	log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
	char buf[MAX_STRING_LENGTH];
	struct snoop_data *snoop;
	
	sprintf(buf,"%%(%s)%s\n\r", ch == NULL ? "Null" : ch->name, logline );
	for ( snoop = ch->desc->snoop_by; snoop != NULL; snoop = snoop->next )
	    send_to_char( buf, snoop->d->character );
/*	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 ); */
    }

/*
    if ( !found )
    {
	*
	 * Look for command in socials table.
	 *
	if ( (social = !check_social( ch, command, argument) ) )
	{
	    send_to_char( "?\n\r", ch );
	    return;
	}
    }
*/
    social = FALSE;
    if ( !found )
	social = check_social(ch, command, argument );

    /*
     * Character not in position for command?
     */
   if (!social && found)
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r", ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r", ch);
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r", ch);
	    break;
	}
	return;
    }

    docommand = TRUE;
    /* check for interactive mobile special procedures */
    /* make sure it doesn't crash if the command is quit */
/* adding object checks, will check obj, then mob, then room */
  if (str_prefix( command, "quit" ) )
  {
    for ( obj_check = ch->carrying ; obj_check != NULL ; obj_check = obj_next )
    {
	obj_next = obj_check->next_content;

	if (obj_check->special != 0)
		docommand = (*obj_check->special) ( obj_check, ch, command, argument );

	if (!docommand)
		break;
    }
	/* check room for room proc */
    if (docommand)
	if (ch->in_room->special != 0)
		docommand = (*ch->in_room->special) ( ch, command, argument );
  }


    /*
     * Dispatch the command.
     */
   if (docommand)
   {
     if (found)
       (*cmd_table[cmd].do_fun) ( ch, argument );
     else if (!social)
       send_to_char("No.\n\r", ch );
    }


    tail_chain( );
    return;
}



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    extern int MAX_SOCIAL;
    bool found, found2;

    found  = FALSE;
    found2 = FALSE;

    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	&&   !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if( found == FALSE ) {

    for ( cmd = 0; cmd <= MAX_SOCIAL; cmd++ )
    {
	if( !str_prefix( command, socials[cmd].name ) )
	{
	    found2 = TRUE;
	    break;
	}
    }
    }

    if ( !found && !found2 )
	return FALSE;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r", ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too bad for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r", ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;

    if( found2 == TRUE ) {

	if( arg[0] == '\0' ) {

	    if( str_cmp( socials[cmd].roomnv, "NULL" ) )
		act( socials[cmd].roomnv, ch, NULL, victim, TO_ROOM, TRUE );
	    if( str_cmp( socials[cmd].charnv, "NULL" ) )
		act( socials[cmd].charnv, ch, NULL, victim, TO_CHAR, TRUE );

	}
	else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
	}
	else if ( victim == ch )
	{
		send_to_char( "What are you thinking?!\n\r", ch );
	}
	else if ( victim != NULL && victim != ch )
	{
	
	    if( str_cmp( socials[cmd].room, "NULL" ) )
		act( socials[cmd].room , ch, NULL, victim, TO_NOTVICT, TRUE );
	    if( str_cmp( socials[cmd].charc, "NULL" ) )
		act( socials[cmd].charc, ch, NULL, victim, TO_CHAR, TRUE );
	    if( str_cmp( socials[cmd].vict, "NULL" ) )
		act( socials[cmd].vict , ch, NULL, victim, TO_VICT, TRUE );
	}
    return TRUE;
    }

    if ( arg[0] == '\0' )
    {
	act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM, TRUE    );
	act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR, TRUE    );
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
	act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM, TRUE    );
	act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR, TRUE    );
    }
    else
    {

	act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT, TRUE );
	act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR, TRUE    );
	act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT, TRUE    );

	if ( !IS_NPC(ch) && IS_NPC(victim)
	&&   !IS_AFFECTED(victim, AFF_CHARM)
	&&   IS_AWAKE(victim) )
	{
	    switch ( number_bits( 4 ) )
	    {
	    case 0:
/*
		multi_hit( victim, ch, TYPE_UNDEFINED );
*/
		break;

	    case 1: case 2: case 3: case 4:
	    case 5: case 6: case 7: case 8:
		act( social_table[cmd].others_found,
		    victim, NULL, ch, TO_NOTVICT, TRUE );
		act( social_table[cmd].char_found,
		    victim, NULL, ch, TO_CHAR, TRUE    );
		act( social_table[cmd].vict_found,
		    victim, NULL, ch, TO_VICT, TRUE    );
		break;

	    case 9: case 10: case 11: case 12:
		act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT, TRUE );
		act( "You slap $N.",  victim, NULL, ch, TO_CHAR, TRUE    );
		act( "$n slaps you.", victim, NULL, ch, TO_VICT, TRUE    );
		break;
	    }
	}
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
    if ( *arg == '\0' )
	return FALSE;

    if ( *arg == '-' && strlen( arg ) > 1 )
      arg++;

    for ( ; *arg != '\0'; arg++ )
    {
	if ( !isdigit(*arg) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;
    
    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

