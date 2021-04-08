#include "include/lcd_ctrl.h"
#ifdef EZIO_300_CONFIG
void print_help(char *progname)
{
	printf("\n");

	printf("%s - The Caswell EZIO sample API\n", progname);
	printf("Usage: %s -d [device] -[command]\n", progname);
	printf("[device]\n");
	printf("\texample: /dev/ttyS0\n");
	printf("[command]\n");
#ifndef EZIO_320_CONFIG
	printf("\t -g                    Read key\n");
#endif /* not EZIO_320_CONFIG */
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
	printf("\t -P [Addr] [Row] [Col] Display customized pattern in specific position\n");
	printf("\t [Line]:    (1|2) Display in the (1|2) lines\n");
	printf("\t [Row]:     (1|2)\n");
	printf("\t [Col]:     (1|16)\n");
	printf("\t [Message]: display string, string length limit of 16 \n");
	printf("\t [Information]: get save address and row types,\n");
	printf("\t                need 18 characters, more in README\n");
	printf("\n");
}

int get_key(unsigned int fd){
	int res;
	unsigned char buf[3]={0x00},old_status=0x00;
	SetDis(fd);
	while (1)
	{
		ReadKey(fd);
		memset(buf,0x00,sizeof(buf));
		res = read(fd,buf,2);			/* read response from EZIO   */
		if (res == 1) {
			res = read(fd,&buf[1],1);
		}

		switch(buf[1]) {			/* Switch the Read command   */
		case KEY_CODE_UP:				/* Up Botton was received    */
			if(old_status == KEY_CODE_UP)
				break;
			ShowMessage(fd, MSG1, MSG3);	/* display "CASwell EZIO"   */
			sleep(1);			/* display "Up is selected   */
			old_status = KEY_CODE_UP;
			break;
		case KEY_CODE_DOWN:				/* Down Botton was received  */
			if(old_status == KEY_CODE_DOWN)
				break;
			ShowMessage(fd, MSG1, MSG4);	/* display "CASwell EZIO"   */
			sleep(1);			/* display "Down is selected */
			old_status = KEY_CODE_DOWN;
			break;
		case KEY_CODE_ENTER:				/* Enter Botton was received */
			if(old_status == KEY_CODE_ENTER)
				break;
			ShowMessage(fd, MSG1, MSG5);	/* display "CASwell EZIO"   */
			sleep(1);			/* display "Enter is selected*/
			old_status = KEY_CODE_ENTER;
			break;			
		case KEY_CODE_ESC:				/* ESC   Botton was received */
			if(old_status == KEY_CODE_ESC)
				break;
			ShowMessage(fd, MSG1, MSG6);	/* display "CASwell EZIO"   */
			sleep(1);			/* display "Esc is selected  */
			old_status = KEY_CODE_ESC;
			break;
		default:
			ShowMessage(fd, MSG1, MSG2); 	/* display "CASwell EZIO"   */
			old_status = 0xfb;
		}
	}
	return 0;
}

void show_message(unsigned int fd,char* LINE1,char* LINE2)
{
	int CLINE1=strlen(LINE1);
	int CLINE2=strlen(LINE2);
	if((CLINE1!=0)&&(CLINE2==0))
	{
		ShowMessage_1(fd,_NULL);
		ShowMessage_1(fd,LINE1);
	}
	else if ((CLINE1==0)&&(CLINE2!=0))
	{
		ShowMessage_2(fd,_NULL);
		ShowMessage_2(fd,LINE2);
	}
	else
	{
		ShowMessage(fd,_NULL,_NULL);
		ShowMessage(fd,LINE1,LINE2);
	}
}

int main (int argc,char *argv[]) {
	int fd=0,c=0;

	if(argc == 3)      //Send noise before init command.
	{
		if (argv[1][1] == 'n')
		{
			fd=OpenAdrPort(argv[2],2400);
			SendNoiseInit(fd);
			return 0;
		}
	}

	if(argc<4)
	{
		print_help(argv[0]);
		return 0;
	}
	fd=OpenAdrPort(argv[2],2400);
	Init(fd);
	Hide(fd);
LOOP:
#ifdef EZIO_320_CONFIG
	while ((c = getopt (argc, argv, "d:s:C:SThctioLRlrpP")) != -1)
#else
	while ((c = getopt (argc, argv, "d:s:C:STghctioLRlrpP")) != -1)
#endif /* EZIO_320_CONFIG */
	switch (c)
	{
#ifndef EZIO_320_CONFIG
		case 'g':
			get_key(fd);				//Read key
			break;
#endif /* not EZIO_320_CONFIG */
		case 's':
			if (argc==4)
			{
				printf("Please input string\n");
				break;
			}else if(atoi(optarg)==1)
			{
				ShowMessage_1(fd,_NULL);
				ShowMessage_1(fd,argv[5]);
			}else if(atoi(optarg)==2)
                        {
                                ShowMessage_2(fd,_NULL);
                                ShowMessage_2(fd,argv[5]);
			}
			break;
		case 'C':
			if  (argc < 7) {
				print_help(argv[0]);
			} else {
				CharShow(fd, argv[4][0], atoi(argv[5]), atoi(argv[6]));
			}
			break;
		case 'c':
			Cls(fd);				//Clear screen
			break;
		case 'o':
			Home(fd);				//Home cursor
			break;
		case 'i':
			Hide(fd);				//Hide cursor & display blanked characters
			break;
		case 't':
			TurnOn(fd);				//Turn on (blinking block cursor)
			break;
		case 'L':
			MoveL(fd);				//Move cursor 1 character left
			break;
		case 'R':
			MoveR(fd);				//Move cursor 1 character right
			break;
		case 'l':
			ScrollL(fd);				//Scroll 1 character left
			break;
		case 'r':
			ScrollR(fd);				//Scroll 1 character right
			break;
		case 'S':
			Show(fd);				//Show underline cursor
			break;
		case 'T':
			ReadKey(fd);				//test mode(Read_KEY)
			break;
		case 'h':
			print_help(argv[0]);
			StopSend(fd);
			return 0;
		case 'd':						
			goto LOOP;
		case 'b':						
			goto LOOP;			
		case 'p':
			Paint(fd,argv[4]);
			break;
		case 'P':
                       if (argc!= 7)
                        {
                               printf("Wrong Format Input\n");
                        }
                        ShowPattern(fd,atoi(argv[4]),atoi(argv[5]),atoi(argv[6]));
                        break;
		default:
			print_help(argv[0]);
			StopSend(fd);
			return 0;
	}

	StopSend(fd);
	CloseAdrPort(fd);					
	return 0;
 
}
#else
int main (int argc,char *argv[]) {
	return 0;
}
#endif 
