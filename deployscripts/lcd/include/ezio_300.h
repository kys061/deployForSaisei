typedef unsigned int            uint;
typedef unsigned short int	usint;
typedef unsigned char           uchar;
//LCD Command Define		
#define POLL		1 			/* 1 :for polling 0 :for interrupt mode */
/* Add or Change Show Message here */
#define CLS 		{0xFE, 0X01}		/* Clear screen */
#if	POLL
#define INIT 		{0xFE, 0X28, 0xFE, 0x28}/* Start Of HEX */
#else
#define INIT		{0xFE, 0x28}
#endif
#define NOISE		{0xFE}			/* Add noise before command */
#if POLL
#define STOPEND 	{0xFE, 0X37, 0xFE, 0x37}		/* End Of HEX */
#else
#define STOPEND		{0xFE, 0x37}
#endif
#define HOME		{0xFE, 0X02}		/* Move cursor to home  */
#define READKEY 	{0xFE, 0X06}		/* Read key */
#define BLANK		{0xFE, 0X08}		/* Blank display (retaining data) */
#define HIDE		{0xFE, 0X0C}		/* Hide cursor & display blanked characters */
#define TURN		{0xFE, 0X0D}    	/* Turn on (blinking block cursor) */
#define SHOW		{0xFE, 0X0E}		/* Show underline cursor */
#define MOVEL 		{0xFE, 0X10}		/* Move cursor 1 character left */
#define MOVER		{0xFE, 0X14}		/* Move cursor 1 character right */
#define SCL		{0xFE, 0X18}		/* Scroll cursor 1 character left */
#define SCR		{0xFE, 0x1C}		/* Scroll cursor 1 character right */
#define SETDIS		{0xFE, 0x40}		/* Set character-generator address */
#define SL1  		{0xFE, 0x80}		/* Move cursor to Line 1 */
#define SL2  		{0xFE, 0xC0}		/* Move cursor to Line 2 */
#define LINE_1		0x80
#define LINE_2		0xC0

//Command length
#define CLS_LEN		2
#if	POLL
#define INIT_LEN	4
#else
#define INIT_LEN	2
#endif
#define NOISE_LEN	1
#if POLL
#define STOPEND_LEN	4
#else
#define STOPEND_LEN 2
#endif
#define HOME_LEN	2
#define READKEY_LEN	2
#define BLANK_LEN	2
#define SHOW_LEN	2
#define HIDE_LEN	2
#define TURN_LEN	2
#define MOVEL_LEN	2
#define MOVER_LEN	2
#define SCL_LEN		2
#define SCR_LEN		2
#define SETDIS_LEN	2
#define SL1_LEN		2
#define SL2_LEN		2
#define MCP_LEN		2			/* Move cursor to (x,y) */ 
/**/
#define PAINT_LEN	10
#define SITERANGE	8
#define INFOLEN		18
#define	SITE	{0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78}

/* Key code */
#define KEY_CODE_UP     0xBE
#define KEY_CODE_DOWN   0xBD
#define KEY_CODE_ENTER  0xBB
#define KEY_CODE_ESC    0xB7

#define MSG1	"Caswell EZIO    "
#define MSG2	"****************"
#define MSG3	"Up is select    "
#define MSG4	"Down is select  "
#define MSG5	"Enter is select "
#define MSG6	"Esc is select   "
#define MSG7	"Right is select "
#define _NULL	"                "

#define LCR_BRATE_B2400		48
#define LCR_WDATA_8BITS_1STOP	0x03
#define TTY_GETDATA		0x01
#define TTY_BRATE		0x02
#define MEB392X			2
#define MEB393X			1

int GetBaudrateIndex(long Baud_Rate);
void CloseAdrPort(int fd);
int OpenAdrPort(char* device,long baudrate);
void SendCommand(int fd, uchar *cmd, int length);
void SendString(int fd, char *str, int length);
void Init (int fd);
void Cls (int fd);
void Home (int fd);
void Hide (int fd);
void Show (int fd);
void MoveCurPosition(int fd, uchar x, uchar y);
void MoveL (int fd);
void MoveR (int fd);
void CharShow(int fd, char c, int x, int y);
void StopSend (int fd);
void ReadKey (int fd);
void Blank (int fd);
void TurnOn (int fd);
void ScrollL(int fd);
void ScrollR(int fd);
void SetDis(int fd);
void ShowMessage (int fd,char *str1 , char *str2);
void ShowMessage_1 (int fd,char *str1 );
void ShowMessage_2 (int fd,char *str1 );
void SendNoiseInit(int fd);
void ShowPattern (int fd, char cmd, int x, int y);

void Paint(int fd,char* str);
uchar chtohex(uchar ch);
