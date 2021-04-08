/*******************************************************************************

  CASwell(R) Sample LCD Daemon
  Copyright(c) 2016 Gilbert Peng <gilbert.peng@cas-well.com>

  This sample daemon can provide the rolling menu in LCD to show the basic
  display and button functions of EZIO-G500/G400 module via provided EZIO API.

  CASwell, Inc. All rights reserved.

*******************************************************************************/

#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "include/lcd_ctrl.h"

#define	MENU_MAIN			0

#define	ACT_INIT			0
#define	ACT_HOME			1
#define	ACT_REBOOT			2
#define	ACT_SHUTDOWN		3

#ifdef EZIO_G400_CONFIG
#define	KEY_UP				0x44
#define	KEY_DOWN			0x45
#define	KEY_ENTER			0x46
#define	KEY_ESC				0x41
#else
#define	KEY_UP				0x44
#define	KEY_DOWN			0x46
#define	KEY_ENTER			0x45
#define	KEY_ESC				0x43
#endif

#define	MAX_ACT_NUM			10
#define	LINE_BLANK			"                "

#define	MSG_INIT			"Initializing... "
#define	MSG_HOME			"Service ONLINE  "
#define	MSG_REBOOT			"Reboot          "
#define	MSG_SHUTDOWN		"Shutdown        "

#define	MSG_CD_STOP			"Hold ESC to stop"
#define	MSG_FMT_CD_REBOOT	"Reboot in %2d s\0"
#define	MSG_FMT_CD_SHUTDOWN	"Shutdown in %2d s\0"
#define	MSG_REBOOTING		"Rebooting...    "
#define	MSG_SHUTTINGDOWN	"Shutting down..."

/* Global variables to control path */
unsigned int	prev_menu = MENU_MAIN;
unsigned int	curr_menu = MENU_MAIN;
unsigned int	prev_act = ACT_INIT;
unsigned int	curr_act = ACT_INIT;

/* Define functions */
int countdown(int lcm, char *fmt, int cd);
char * strdate(void);
void init_home(int lcm);
void act_reboot(int lcm);
void act_shutdown(int lcm);

struct act {
	char	*msg;
	int		next_menu;
	void	(*init_func) (int);
	void	(*act_func) (int);
	int		down_act;
	int		up_act;
};

struct menu {
	char		*name;
	struct act	acts[MAX_ACT_NUM];
};

static struct menu	menus[] = {
	/* Menu 0: Main Menu */
	{
		.name = "Main Menu",
		.acts = {
			{ MSG_INIT, -1, NULL, NULL, ACT_INIT, ACT_INIT },
			{ MSG_HOME, -1, init_home, NULL, ACT_REBOOT, ACT_SHUTDOWN },
			{ MSG_REBOOT, -1, NULL, act_reboot, ACT_SHUTDOWN, ACT_HOME },
			{ MSG_SHUTDOWN, -1, NULL, act_shutdown, ACT_HOME, ACT_REBOOT },
		}
	},
	{ NULL }
};

int countdown(int lcm, char *fmt, int cd)
{
	int				res;
	unsigned char	key[3] = {0x00};
	char			buf[17] = {0};
	fd_set rfds;
	struct timeval tv = {0};

	for (; cd; cd--) {
		snprintf(buf, sizeof(buf), fmt, cd);
		ShowMessage(lcm, buf, MSG_CD_STOP);

		
		FD_ZERO(&rfds);
		FD_SET(lcm, &rfds);
		
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		
		res = select(lcm + 1, &rfds, NULL, NULL, &tv);
		
		if(res < 0)
			return (cd);
		
		if(res > 0 && FD_ISSET(lcm, &rfds)) {
			res = read(lcm, &key[1], 1);
			
			switch (key[1]) {
			case KEY_ESC:
				ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);
				return (cd);
			default:
				break;
			}
		}
	}

	return (cd);
}

char * strdate(void)
{
	static char		str_date[17] = {0};
	time_t			t;
	struct tm		*tmp;

	t = time(NULL);
	tmp = localtime(&t);

	if (tmp != NULL) {
		strftime(str_date, sizeof(str_date), "%F %R", tmp);
	}

	return (str_date);
}

void init_home(int lcm)
{
	/* Clear first for blinking effect */
	ShowMessage_2(lcm, LINE_BLANK);
	ShowMessage_2(lcm, strdate());
}

void act_reboot(int lcm)
{
	if (countdown(lcm, MSG_FMT_CD_REBOOT, 5) == 0) {
		/* Start to reboot */
		ShowMessage(lcm, MSG_REBOOTING, LINE_BLANK);
		sleep(1);
		system("reboot");
	}
}

void act_shutdown(int lcm)
{
	if (countdown(lcm, MSG_FMT_CD_SHUTDOWN, 5) == 0) {
		/* Start to shutdown */
		ShowMessage(lcm, MSG_SHUTTINGDOWN, LINE_BLANK);
		sleep(1);
		system("shutdown -h now");
	}
}

int main(int argc, char **argv)
{
	int				lcm;
	int				res;
	unsigned int	tick = 0;
	unsigned char	key[3] = {0x00};
	
	
	if(argc < 2)
	{
		printf("\n");
		printf("Usage: lcdd [device]\n");
		printf("\n");
		return 0;
	}
	

	/* Open LCM control path */
	if ((lcm = OpenAdrPort(argv[1], 115200)) < 0) {
		fprintf(stderr, "Fail to open LCM control path!\n");
		exit(1);
	}

	/* Initialize the LCM */
	Init(lcm);
	Hide(lcm);
	ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);
	sleep(3);

	/* Main loop to scan keypad and respond */
	//SetDis(lcm);
	curr_menu = MENU_MAIN;
	curr_act = ACT_HOME;
	ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);

	while (1) {
		/* Read key pressed from LCM */
		memset(key, 0x0, sizeof(key));
		res = read(lcm, &key[1], 1);

		/* Take corresponding action by the read key */
		switch (key[1]) {
		case KEY_UP:
			curr_act = menus[curr_menu].acts[curr_act].up_act;
			ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);

			if (menus[curr_menu].acts[curr_act].init_func != NULL) {
				menus[curr_menu].acts[curr_act].init_func(lcm);
			}

			break;
		case KEY_DOWN:
			curr_act = menus[curr_menu].acts[curr_act].down_act;
			ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);

			if (menus[curr_menu].acts[curr_act].init_func != NULL) {
				menus[curr_menu].acts[curr_act].init_func(lcm);
			}

			break;
		case KEY_ENTER:
			if (menus[curr_menu].acts[curr_act].next_menu != -1) {
				prev_menu = curr_menu;
				curr_menu = menus[curr_menu].acts[curr_act].next_menu;
				prev_act = curr_act;
				curr_act = 0;
				ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);

				if (menus[curr_menu].acts[curr_act].init_func != NULL) {
					menus[curr_menu].acts[curr_act].init_func(lcm);
				}
			} else if (menus[curr_menu].acts[curr_act].act_func != NULL) {
				menus[curr_menu].acts[curr_act].act_func(lcm);
			}

			break;
		case KEY_ESC:
			if (prev_menu != curr_menu) {
				curr_menu = prev_menu;
				curr_act = prev_act;

				if (menus[curr_menu].acts[curr_act].init_func != NULL) {
					menus[curr_menu].acts[curr_act].init_func(lcm);
				}

				ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);
			}

			break;
		default:
			/* Update display date/time */
			if ((curr_menu == MENU_MAIN) \
					&& (curr_act == ACT_HOME) \
					&& (tick % 15 == 0)) {
				init_home(lcm);
			}

			break;
		}

		tick++;
	}

	/* Prepare to exit */
	//StopSend(lcm);
	CloseAdrPort(lcm);

	exit(0);
}
