#include "include/lcd_ctrl.h"

#ifdef EZIO_340_CONFIG
void print_help(void)
{
	printf("\n");

	printf("ezio_340_api - The CASwell EZIO sample API\n");
	printf("Usage: ezio_340_api -d [device] -[command]\n");
	printf("[device]\n");
	printf("\texample: /dev/ttyS0\n");
	printf("[command]\n");
	printf("\t -g                    Read key\n");
	printf("\t -s [Line] [Message]   Display message\n");
	printf("\t -C [Char] [Row] [Col] Display a character in specific position\n");
	printf("\t -c                    Clear screen\n");
	printf("\t -o                    Home cursor\n");
	printf("\t -L                    Move cursor 1 character left\n");
	printf("\t -R                    Move cursor 1 character right\n");
	printf("\t -l                    Scroll 1 character left\n");
	printf("\t -r                    Scroll 1 character right\n");
	printf("\t -i                    Hide cursor & display blanked characters\n");
	printf("\t -t                    Turn on (blinking block cursor)\n");
	printf("\t -S                    Show underline cursor\n");
	printf("\t -n                    Send noise message\n");
	printf("\t -h                    Show help message\n\n");
	printf("\t -p [Information]      Create customized pattern\n");
	printf("\t [Line]:    (1|2) Display in the (1|2) lines\n");
	printf("\t [Row]:     (1|2)\n");
	printf("\t [Col]:     (1|16)\n");
	printf("\t [Message]: display string, string length limit of 16 \n");
	printf("\t [Information]: get save address and row types,\n");
	printf("\t                need 18 characters, more in README\n");
	printf("\n");
}

int get_key(unsigned int fd)
{
	int				res;
	unsigned char	buf[3] = {0x00};
	unsigned char	old_status = 0x00;

	SetDis(fd);
	while (1) {
		ReadKey(fd);
		memset(buf, 0x00, sizeof(buf));
		/* Read response from EZIO */
		res = read(fd, buf, 2);
		/* Switch the Read command */
		switch (buf[1]) {
		case 0x3a:	/* Up button was received */
			if (old_status != 0x3a) {
				ShowMessage(fd, MSG1, MSG3);
				sleep(1);
				old_status = 0x3a;
			}
			break;
		case 0x1e:	/* Down button was received */
			if (old_status != 0x1e) {
				ShowMessage(fd, MSG1, MSG4);
				sleep(1);
				old_status = 0x1e;
			}
			break;
		case 0x3c:	/* Enter buttonn was received */
			if (old_status != 0x3c) {
				ShowMessage(fd, MSG1, MSG5);
				sleep(1);
				old_status = 0x3c;
			}
			break;
		case 0x36:	/* Left button was received */
			if (old_status != 0x36) {
				ShowMessage(fd, MSG1, MSG6);
				sleep(1);
				old_status = 0x36;
			}
			break;
		case 0x2e:	/* Right button was received */
			if (old_status != 0x2e) {
				ShowMessage(fd, MSG1, MSG7);
				sleep(1);
				old_status = 0x2e;
			}
			break;
		default:
			ShowMessage(fd, MSG1, MSG2);
			old_status = 0x3e;
			break;
		}
	}
	return (0);
}

void show_message(unsigned int fd, char *LINE1, char *LINE2)
{
	int	CLINE1 = strlen(LINE1);
	int	CLINE2 = strlen(LINE2);

	if ((CLINE1 != 0) && (CLINE2 == 0)) {
		ShowMessage_1(fd, _NULL);
		ShowMessage_1(fd, LINE1);
	} else if ((CLINE1 == 0) && (CLINE2 != 0)) {
		ShowMessage_2(fd, _NULL);
		ShowMessage_2(fd, LINE2);
	} else {
		ShowMessage(fd, _NULL, _NULL);
		ShowMessage(fd, LINE1, LINE2);
	}
}

int main(int argc, char *argv[])
{
	int	fd = 0;
	int	c = 0;

    /* Send noise before init command */
	if (argc == 3) {
		if (argv[1][1] == 'n') {
			fd = OpenAdrPort(argv[2], 2400);
			SendNoiseInit(fd);
			return (0);
		}
	}

	if (argc < 4) {
		print_help();
		return (-1);
	}

	fd = OpenAdrPort(argv[2], 2400);
	Init(fd);
	Hide(fd);

LOOP:
	while ((c = getopt(argc, argv, "d:s:C:STghctioLRlrp")) != -1) {
		switch (c) {
		case 'g':
			get_key(fd);
			break;
		case 's':
			if (argc == 4) {
				printf("Please input string.\n");
				break;
			} else if (atoi(optarg) == 1) {
				ShowMessage_1(fd, _NULL);
				ShowMessage_1(fd, argv[5]);
			} else if (atoi(optarg) == 2) {
				ShowMessage_2(fd, _NULL);
				ShowMessage_2(fd, argv[5]);
			}
			break;
		case 'C':
			if (argc < 7) {
				print_help();
			} else {
				CharShow(fd, argv[4][0], atoi(argv[5]), atoi(argv[6]));
			}
			break;
		case 'c':
			Cls(fd);
			break;
		case 'o':
			Home(fd);
			break;
		case 'i':
			Hide(fd);
			break;
		case 't':
			TurnOn(fd);
			break;
		case 'L':
			MoveL(fd);
			break;
		case 'R':
			MoveR(fd);
			break;
		case 'l':
			ScrollL(fd);
			break;
		case 'r':
			ScrollR(fd);
			break;
		case 'S':
			Show(fd);
			break;
		case 'T':
			ReadKey(fd);
			break;
		case 'h':
			print_help();
			StopSend(fd);
			return (0);
		case 'd':						
			goto LOOP;
		case 'b':						
			goto LOOP;			
		case 'p':
			Paint(fd, argv[4]);
			break;
		default:
			print_help();
			StopSend(fd);
			return (-1);
		}
	}

	StopSend(fd);
	CloseAdrPort(fd);					
	return (0);
}
#else
int main(int argc, char *argv[])
{
	return (0);
}
#endif /* EZIO_340_CONFIG */
