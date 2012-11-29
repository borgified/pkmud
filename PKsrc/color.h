/*
 * color.c
 *
 * (C) 1999 by Rip  (Robert L. Peckham - peckham@robpeckham.net)
 */


#define MAX_COLOR 20


struct color_info
{
  char	*name;
  char	*sequence;
};

extern const struct color_info color_data[MAX_COLOR];

#define C_BLACK		"\033[0;30m"
#define C_RED		"\033[0;31m"
#define C_GREEN		"\033[0;32m"
#define C_YELLOW	"\033[0;33m"
#define C_BLUE		"\033[0;34m"
#define C_MAGENTA	"\033[0;35m"
#define C_CYAN		"\033[0;36m"
#define C_WHITE		"\033[0;37m"
#define C_B_BLACK	"\033[1;30m"
#define C_B_RED		"\033[1;31m"
#define C_B_GREEN	"\033[1;32m"
#define C_B_YELLOW	"\033[1;33m"
#define C_B_BLUE	"\033[1;34m"
#define C_B_MAGENTA	"\033[1;35m"
#define C_B_CYAN	"\033[1;36m"
#define C_B_WHITE	"\033[1;37m"
#define ANSI_OFF	"\033[0m"
#define ANSI_BOLD	"\033[0;1m"
#define ANSI_UNDERSCORE	"\033[0;4m"
#define ANSI_BLINK	"\033[0;5m"
#define ANSI_REVERSE    "\033[0;7m"
#define ANSI_CONCEALED  "\033[0;8m"
#define ANSI_BEEP	"\007"

