/*******************************************************************************

  CASwell(R) Sample LCD Daemon
  Copyright(c) 2015 Alan Yu <alan.yu@cas-well.com>

  This sample daemon can provide the rolling menu in LCD to show the basic
  display and button functions of EZIO-300 module via provided EZIO API.

  CASwell, Inc. All rights reserved.

*******************************************************************************/

#include <time.h>
#include "include/lcd_ctrl.h"

#define	MENU_MAIN			0

#define	ACT_INIT			0
#define	ACT_HOME			1
#define	ACT_REBOOT			2
#define	ACT_SHUTDOWN		3

#define	KEY_UP				0xBE
#define	KEY_DOWN			0xBD
#define	KEY_ENTER			0xBB
#define	KEY_ESC				0xB7

#define	MAX_ACT_NUM			10
#define	LINE_BLANK			"                "

#define	MSG_INIT			"                "
#define	MSG_HOME			"SAISEI          "
#define MSG_HOME2			"FLOWCOMMAND     "
#define	MSG_REBOOT			"Reboot          "
#define	MSG_SHUTDOWN		"Shutdown        "

#define	MSG_CD_STOP			" "
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
			{ MSG_HOME, -1, init_home, NULL, ACT_REBOOT, ACT_SHUTDOWN },	// 메시지, -1, 호출함수, 널, 다운버튼시 값, 업버튼시 값
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

	for (; cd; cd--) {
		snprintf(buf, sizeof(buf), fmt, cd);
		ShowMessage(lcm, buf, MSG_CD_STOP);

		sleep(1);

		ReadKey(lcm);
		memset(key, 0x0, sizeof(key));
		res = read(lcm, key, 2);
		if (res == 1) {
			res = read(lcm, &key[1], 1);
		}

		switch (key[1]) {
		case KEY_ESC:
			ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);
			return (cd);
		default:
			break;
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
	// ShowMessage_2(lcm, LINE_BLANK);
	//ShowMessage_2(lcm, strdate());
	ShowMessage_1(lcm, MSG_HOME);
	ShowMessage_2(lcm, MSG_HOME2);
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

	/* Open LCM control path */
	if ((lcm = OpenAdrPort(argv[1], 2400)) < 0) {
		fprintf(stderr, "Fail to open LCM control path!\n");
		exit(1);
	}

	/* Initialize the LCM */
	Init(lcm);
	Hide(lcm);
	ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, LINE_BLANK);
	sleep(3);

	/* Main loop to scan keypad and respond */
	SetDis(lcm);
	curr_menu = MENU_MAIN;
	curr_act = ACT_HOME;
	ShowMessage(lcm, menus[curr_menu].acts[curr_act].msg, MSG_HOME2);
	ShowMessage_1(lcm, MSG_HOME);
	ShowMessage_2(lcm, MSG_HOME2);


	while (1) {
		/* Read key pressed from LCM */
		ReadKey(lcm);
		memset(key, 0x0, sizeof(key));
		res = read(lcm, key, 2);
		if (res == 1) {
			res = read(lcm, &key[1], 1);
		}

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
			// if ((curr_menu == MENU_MAIN) \
			// 		&& (curr_act == ACT_HOME) \
			// 		&& (tick % 15 == 0)) {
			// 	init_home(lcm);
			// }
			// init_home(lcm);

			break;
		}

		tick++;
	}

	/* Prepare to exit */
	StopSend(lcm);
	CloseAdrPort(lcm);

	exit(0);
}
