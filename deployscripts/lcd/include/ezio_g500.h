typedef unsigned int            uint;
typedef unsigned short int	usint;
typedef unsigned char           uchar;

#define MSG1    "Received Key Hex is 0x41"
#define MSG2    "Received Key Hex is 0x42"
#define MSG3    "Received Key Hex is 0x43"
#define MSG4    "Received Key Hex is 0x44"
#define MSG5    "Received Key Hex is 0x45"
#define MSG6    "Received Key Hex is 0x46"
#define MSG7    "Received Key Hex is 0x47"
#define _NULL   "                    "

//LCD Command Define
#define CLS	{0x0C}				/* Clear screen */
#define CAN 	{0x18}				/* Clear current line */
#define INIT	{0x1B, 0x40}			/* LCM initialization */
#define HOME	{0x0B}				/* Move cursor to home */
#define SHOW	{0x1B, 0x5F, 0x01}		/* Show cursor */
#define HIDE	{0x1B, 0x5F, 0x00}		/* Hide cursor */
#define MOVEL	{0x1B, 0x5B, 0x44}		/* Move cursor 1 character left */
#define MOVER	{0x1B, 0x5B, 0x43}		/* Move cursor 1 character right */
#define MCRM	{0x1B, 0x5B, 0x52}		/* Move cursor to right-most */
#define MCLM	{0x1B, 0x5B, 0x4C}		/* Move cursor to left-most */
#define MOVEU	{0x1B, 0x5B, 0x41}		/* Move cursor up */
#define MOVED	{0x1B, 0x5B, 0x42}		/* Move cursor down */
#define SL1	{0x1B, 0x6C, 0x00, 0x00}	/* Move cursor to Line 1 */
#define SL2	{0x1B, 0x6C, 0x00, 0x01}	/* Move cursor to Line 2 */
#define SBL	{0x1B, 0x42, 0x00}		/* Set back light */
#define SLED	{0x1B, 0x4C, 0x00}		/* Set LED */
#define SPIC	{0x1B, 0x53, 0x00}		/* Save pic */
#define LPIC	{0x1B, 0x50, 0x00}		/* Load pic */

//Command Length
#define CLS_LEN		1
#define CAN_LEN 	1
#define INIT_LEN	2
#define HOME_LEN	1
#define SHOW_LEN	3
#define HIDE_LEN	3
#define MOVEL_LEN	3
#define MOVER_LEN	3
#define MCRM_LEN	3
#define MCLM_LEN	3
#define MOVEU_LEN	3
#define MOVED_LEN	3
#define SL1_LEN	4
#define SL2_LEN	4
#define MCP_LEN		4
#define SBL_LEN		3
#define SLED_LEN	3
#define SPIC_LEN	3
#define LPIC_LEN	3

typedef struct {
   unsigned short int type;			// Magic identifier
   unsigned int size;				// File size in bytes
   unsigned short int reserved1, reserved2;
   unsigned int offset;				// Offset to image data
} BmpFileHeader;

typedef struct {
   unsigned int size;				/* Header size in bytes      */
   unsigned int width,height;			/* Width and height of image */
   unsigned short int planes;       		/* Number of colour planes   */
   unsigned short int bits;         		/* Bits per pixel            */
   unsigned int compression;			/* Compression type          */
   unsigned int imagesize;			/* Image size in bytes       */
   unsigned int xresolution,yresolution;	/* Pixels per meter          */
   unsigned int ncolours;			/* Number of colours         */
   unsigned int importantcolours;		/* Important colours         */
} BmpInfoHeader;

typedef struct {                   		// if BmpInfoHeader.ncolours > 0
   unsigned char r,g,b,junk;       		// This field will be added
} BmpColourIndex;

/*
#define D_PIC_W                 192
#define D_PIC_H                 64
#define D_PIC_W_BYTE_LEN        (192 / 8)
#define D_PIC_DATA_LEN          (D_PIC_H * D_PIC_W_BYTE_LEN)
#define D_PIC_PAGE_W_BYTE_LEN   (192 / 8 / 3)
#define D_PIC_TOTAL_BYTE_LEN    (D_PIC_W_BYTE_LEN * D_PIC_H)
#define D_PIC_ONE_LINE_BYTE_LEN (D_PIC_W_BYTE_LEN * 8)
*/
#define D_SEND_BLOCK            64

static void	GetBmpModel(int	iModel);
static int CheckFileFormat(FILE *fRStream, BmpFileHeader *stBmpFileHeader, BmpInfoHeader *stBmpInfoHeader);
static void TurnUpsideDown(uchar *cReadBuffer);
static void ConvertMap(uchar* ucBuffer);
static void ConvertBmp(uchar *cReadBuffer, uchar *cWriteBuffer);
void SendPic(int fd, int mode, char *pic_path);
void ChangeBaudrate(int fd, long baudrate);
void SetBackLight(int fd, uchar light);
void Can(int fd);
void MoveRMost(int fd);
void MoveLMost(int fd);
void MoveU(int fd);
void MoveD(int fd);int GetBaudrateIndex(long Baud_Rate);
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
void ShowMessage(int fd, char *str1, char *str2);
void ShowMessage_2(int fd, char *str1);
void SavePic(int fd, uchar val);
void LoadPic(int fd, uchar val);
void SetLED(int fd, uchar val);
