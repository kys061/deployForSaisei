#include "include/lcd_ctrl.h"

#ifdef EZIO_G500_CONFIG
uint	giBmpWidth, giBmpHeigh;
uint	giBmpWidthInByte;
uint	giBmpTotalDataInByte;
uint	giBmpPageWidthInByte;
uint	giBmpLineOfOneBlockInByte;
#endif

long BaudrateLong[]={B115200, B57600, B38400, B19200, B9600, B4800, B2400};
char *BaudrateString[]={"1152", "0576", "0384", "0192", "0096", "0048", "0024"};

/********************************************************
 * Function: GetBaudrateIndex				*
 * Description : Get the index of defined baudrate	*
 *******************************************************/
int GetBaudrateIndex(long Baud_Rate){
	int index;
	
	switch (Baud_Rate)
	{
		case 115200:
			index = 0;
			break;
		case 57600:
			index = 1;
			break;
		case 38400:
			index = 2;
			break;
		case 19200:
			index  = 3;
			break;
		case 9600:
			index  = 4;
			break;
		case 4800:
			index  = 5;
			break;
		case 2400:
			index  = 6;
			break;
		default:
			index = 0;
			break;
	}  //end of switch baud_rate
	
	return index;
}

/********************************************************
 * Function: CloseAdrPort				*
 * Description : Close serial port			*
 *******************************************************/
void CloseAdrPort(int fd)
{
	if (fd > 0)
		close(fd);
}

/********************************************************
 * Function: OpenAdrPort				*
 * Description : Open serial port 			*
 *******************************************************/
int OpenAdrPort(char* device, long baudrate)
{	  	
	int fd;
	int index;
	char deviceName[64]={0x00};
	sprintf(deviceName, "%s", device);

	// make sure port is closed
	CloseAdrPort(fd);
	fd = open(deviceName, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0)
		return -1;
	else
	{
		struct termios my_termios;
		/**/
		index=GetBaudrateIndex(baudrate);
		fcntl(fd,F_SETFL,0);
		memset (&my_termios, 0x00, sizeof(my_termios));
		tcgetattr(fd, &my_termios); /*get the current options and change them*/
		/**/	
		cfsetispeed(&my_termios, BaudrateLong[index]);
		cfsetospeed(&my_termios, BaudrateLong[index]);
		
		my_termios.c_cflag |=  (CLOCAL | CREAD); 
		my_termios.c_cflag &= ~PARENB; // clear parity enable */
		my_termios.c_cflag &= ~CSTOPB; 
		my_termios.c_cflag &= ~CSIZE;  
		my_termios.c_cflag |=  CS8;     
		my_termios.c_cflag &= ~CRTSCTS;//disable Hardware flow control
		/**/
		my_termios.c_iflag &= ~INPCK; // Enable parity checking
		my_termios.c_iflag &= ~(ICRNL | INLCR);
		my_termios.c_iflag &= ~(IXON | IXOFF | IXANY);//disable software flow control
		/**/
		my_termios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // choosing raw input
		/**/ 
		my_termios.c_oflag &= ~OPOST;  
		my_termios.c_oflag &= ~(ONLCR | OCRNL);
		/**/
		my_termios.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
		my_termios.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
		my_termios.c_cc[VERASE]   = 0;     /* del */
		my_termios.c_cc[VKILL]    = 0;     /* @ */
		my_termios.c_cc[VEOF]     = 0;     /* Ctrl-d */
		my_termios.c_cc[VTIME]    = 0;     /* inter-character timer unused */
		my_termios.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
		my_termios.c_cc[VSWTC]    = 0;     /* '\0' */
		my_termios.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
		my_termios.c_cc[VSTOP]    = 0;     /* Ctrl-s */
		my_termios.c_cc[VSUSP]    = 0;     /* Ctrl-z */
		my_termios.c_cc[VEOL]     = 0;     /* '\0' */
		my_termios.c_cc[VREPRINT] = 0;     /* Ctrl-r */
		my_termios.c_cc[VDISCARD] = 0;     /* Ctrl-u */
		my_termios.c_cc[VWERASE]  = 0;     /* Ctrl-w */
		my_termios.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
		my_termios.c_cc[VEOL2]    = 0;     /* '\0' */
		tcsetattr(fd, TCSANOW, &my_termios);
	}
	return fd;
}

/********************************************************
 * Function: SendCommand				*
 * Description : Send command to LCD			*
 *******************************************************/
void SendCommand(int fd, uchar *cmd, int length){
	write(fd, cmd, length);
//sleep(1000);
}

/********************************************************
 * Function: SendString					*
 * Description : Send string to LCD			*
 *******************************************************/
void SendString(int fd, char *str, int length){
	write(fd, str, length);
//sleep(1000);
}

/********************************************************
 * Function: Init					*
 * Description : Initialize LCD				*
 *******************************************************/
void Init(int fd){
	uchar cmd[INIT_LEN]=INIT;
	SendCommand(fd,cmd,INIT_LEN);
}

/********************************************************
 * Function: Cls					*
 * Description : Clear screen				*
 *******************************************************/
void Cls(int fd){
	uchar cmd[CLS_LEN]=CLS;
	SendCommand(fd,cmd,CLS_LEN);
}

/********************************************************
 * Function: Home					*
 * Description : Move cursor to Home			*
 *******************************************************/
void Home(int fd){
	uchar cmd[HOME_LEN]=HOME;
	SendCommand(fd, cmd, HOME_LEN);
}

/********************************************************
 * Function: Hide					*
 * Description : Hide cursor				*
 *******************************************************/
void  Hide (int fd) {
	uchar cmd[HIDE_LEN]=HIDE; 
	SendCommand(fd, cmd, HIDE_LEN);
}

/********************************************************
 * Function: Show					*
 * Description : Show cursor				*
 *******************************************************/
void  Show (int fd) {
	uchar cmd[SHOW_LEN]=SHOW; 
	SendCommand(fd, cmd, SHOW_LEN);
}

/********************************************************
 * Function: MoveCurPosition				*
 * Description : Move cursor to (x,y)			*
 *******************************************************/
void MoveCurPosition(int fd, uchar x, uchar y){
#ifdef EZIO_G500_CONFIG
	uchar cmd[MCP_LEN]={0x1B, 0x6C, x, y};
#else
	uchar pos;
	if (x == 1)
		pos = 0x80 + (y - 1);
	else if (x == 2)
		pos = 0xC0 + (y - 1);

	uchar cmd[MCP_LEN]={0xFE, pos};
#endif
	SendCommand(fd, cmd, MCP_LEN);
}

/********************************************************
 * Function: MoveL					*
 * Description : Move cursor 1 character left		*
 *******************************************************/
void  MoveL (int fd) {
	uchar cmd[MOVEL_LEN]=MOVEL; 
	SendCommand(fd, cmd, MOVEL_LEN);
}

/********************************************************
 * Function: MoveR					*
 * Description : Move cursor 1 character right		*
 *******************************************************/
void  MoveR (int fd) {
	uchar cmd[MOVER_LEN]=MOVER; 
	SendCommand(fd, cmd, MOVER_LEN);
}

/********************************************************
 * Function: CharShow					*
 * Description : Display 1 character to (x,y)		*
 *******************************************************/
void CharShow(int fd, char c, int x, int y)
{
	MoveCurPosition(fd, x, y);
	SendString(fd, &c, 1);
}
/********************************************************
+ * Function: PatternShow                                       *
+ * Description : Display customized pattern to (x,y)   *
+ *******************************************************/
void PatternShow(int fd, char c, int x, int y)
{
	MoveCurPosition(fd, x, y);
	char str_p[2];
	str_p[0] = c;
	SendString(fd, str_p, 1);
}

#ifdef EZIO_G500_CONFIG
/********************************************************
 * Function: GetBmpModel				*
 * Description : Get bmp model				*
 *******************************************************/
static void	GetBmpModel(int	iModel)
{
	int		iPageNumber;

	switch (iModel) {
	case 0:
		giBmpWidth = 192;
		giBmpHeigh = 64;
		iPageNumber = 3;
		break;
	case 1:
		giBmpWidth = 128;
		giBmpHeigh = 64;
		iPageNumber = 2;
		break;
	default:
		giBmpWidth = 128;
		giBmpHeigh = 32;
		iPageNumber = 2;
	}
	giBmpWidthInByte = giBmpWidth / 8;
	giBmpTotalDataInByte = giBmpWidthInByte * giBmpHeigh;
	giBmpPageWidthInByte = giBmpWidthInByte / iPageNumber;
	giBmpLineOfOneBlockInByte = giBmpWidthInByte * 8;
}

/********************************************************
 * Function: CheckFileFormat				*
 * Description : Check bmp format			*
 *******************************************************/
static int CheckFileFormat(FILE *fRStream, BmpFileHeader *stBmpFileHeader, BmpInfoHeader *stBmpInfoHeader)
{
	if (fread(stBmpFileHeader, 2, 1, fRStream) < 1) {
		printf("Read File Header Data error\n");
		return 1;
	}
	if (fread(&(stBmpFileHeader->size), 12, 1, fRStream) < 1) {
		printf("Read File Header Data error\n");
		return 1;
	}
	if (stBmpFileHeader->type != ('M' * 256 + 'B')) {
		printf("Not BMP File\n");
		return 1;
	}
	 if (fread(stBmpInfoHeader, 40, 1, fRStream) < 1) {
		printf("Read Information Header Data error\n");
		return 1;
	}
	if ((stBmpInfoHeader->width != giBmpWidth) || (stBmpInfoHeader->height != giBmpHeigh)) {
		printf("Picture Size not 192 * 64 :%d * %d\n",stBmpInfoHeader->width,stBmpInfoHeader->height);
		return 1;
	}
	if (stBmpInfoHeader->bits != 1) {
		printf("Only support Block/White");
		return 1;
	}
	if (stBmpInfoHeader->compression != 0) {
		printf("Can not support Compressed mode\n");
		return 1;
	}
	printf("Picture Size %d * %d=>%d=>%d\n",stBmpInfoHeader->width,stBmpInfoHeader->height,stBmpInfoHeader->imagesize,stBmpFileHeader->offset);
	return 0;
}

/********************************************************
 * Function: TurnUpsideDown				*
 * Description : Turn up side down			*
 *******************************************************/
static void TurnUpsideDown(uchar *cReadBuffer)
{
	uint    i, k;
	uchar   *ucTmp;
	uchar   *ucUpBuffer;
	uchar   *ucDownBuffer;
	
	ucTmp = (uchar *)malloc(giBmpWidthInByte);
	if (ucTmp == NULL) {
		printf("Not enough memory to allocate buffer");
		return;
	}
	for (i = 0; i < (giBmpHeigh / 2); i++) {
		ucUpBuffer = &cReadBuffer[i * giBmpWidthInByte];
		ucDownBuffer = &cReadBuffer[(giBmpHeigh - i - 1) * giBmpWidthInByte];
		for (k = 0; k < giBmpWidthInByte; k++)
			ucTmp[k] = ucUpBuffer[k] ^ 0xFF;
		for (k = 0; k < giBmpWidthInByte; k++)
			ucUpBuffer[k] = ucDownBuffer[k] ^ 0xFF;
		for (k = 0; k < giBmpWidthInByte; k++)
			ucDownBuffer[k] = ucTmp[k];
	}
	free(ucTmp);
}

/********************************************************
 * Function: ConvertMap					*
 * Description : Convert map				*
 *******************************************************/
static void ConvertMap(uchar* ucBuffer)
{
	char            cDestBuffer[8];
	int             i, k;
	unsigned char   cMask, cData;
	
	cMask = 0x80;
	
	for (k = 0; k < 8; k++) {
		cData = 0;
		for (i = 0; i < 8; i++) {
			if (ucBuffer[i] & cMask)
			        cData |= 0x80;
			if (i < 7)
			        cData >>= 1;
		}
		cDestBuffer[k] = cData;
		cMask >>= 1;
	}
	
	for (i = 0; i < 8; i++)
		ucBuffer[i] = cDestBuffer[i];

}

/********************************************************
 * Function: ConvertBmp					*
 * Description : Convert bmp				*
 *******************************************************/
static void ConvertBmp(uchar *cReadBuffer, uchar *cWriteBuffer)
{
	uint    i, k;
	uchar   *ucStart;
	uint    uiPage, uiLine, j;
	uchar   ucBuffer[8];
	uint    iGetData;
	
	iGetData = 0;
	for (uiPage = 0; uiPage < giBmpWidthInByte; uiPage += giBmpPageWidthInByte) {
		for (uiLine = 0; uiLine < giBmpTotalDataInByte; uiLine += giBmpLineOfOneBlockInByte) {
			for (j = 0; j < giBmpPageWidthInByte; j++) {
				ucStart = &(cReadBuffer[uiPage + uiLine + j]);
				
				k = 0;
				for (i = 0; i < giBmpLineOfOneBlockInByte; i += giBmpWidthInByte) {
					ucBuffer[k++] = ucStart[i];
				}
				ConvertMap(ucBuffer);
				
				for (k = 0; k < 8; k++)
					cWriteBuffer[iGetData++] = ucBuffer[k];
			}
		}
	}
}

/********************************************************
 * Function: SendPic					*
 * Description : Send picture to LCD			*
 *******************************************************/
void SendPic(int fd, int mode, char *pic_path){
	FILE *fRStream;
	BmpFileHeader   stBmpFileHeader;
	BmpInfoHeader   stBmpInfoHeader;
	char cData[2];
	uchar *cReadBuffer, *cWriteBuffer;
	unsigned int i;
	
	if ((fRStream = fopen(pic_path, "rb")) == NULL) {
		printf("Open %s Error\n",pic_path);
		return;
	}
	GetBmpModel(mode);

	if (CheckFileFormat(fRStream, &stBmpFileHeader, &stBmpInfoHeader)) {
		return;
	}
	fseek(fRStream, (long)stBmpFileHeader.offset, SEEK_SET);
	if ((cReadBuffer = (uchar *) malloc(giBmpTotalDataInByte)) == NULL)
	{
		printf("Not enough memory to allocate buffer\n");
		return;  /* terminate program if out of memory */
	}
	i = fread(cReadBuffer, 1, giBmpTotalDataInByte, fRStream);
	if (i != giBmpTotalDataInByte) {
 		printf("Read BMP Data error\n");
		return;
	}
	TurnUpsideDown(cReadBuffer);
	if ((cWriteBuffer = (uchar *) malloc(giBmpTotalDataInByte)) == NULL)
	{
		printf("Not enough memory to allocate buffer\n");
		return;  /* terminate program if out of memory */
	}
	ConvertBmp(cReadBuffer, cWriteBuffer);

	cData[0] = 0x1B;
	cData[1] = 'G';
	write(fd, cData, 2);
	for (i = 0; i < giBmpTotalDataInByte; i += D_SEND_BLOCK) {
		write(fd, &(cWriteBuffer[i]), D_SEND_BLOCK);
	}
	fclose(fRStream);
	free(cReadBuffer);
	free(cWriteBuffer);
	
}

/********************************************************
 * Function: SavePic                                    *
 * Description : Save pic in layer n.                   *
 *******************************************************/
void SavePic(int fd, uchar val)
{
	uchar cmd[SPIC_LEN] = SPIC;
	cmd[2] = val;
	SendCommand(fd, cmd, SPIC_LEN);
}

/********************************************************
 * Function: LoadPic                                    *
 * Description : Load pic in layer n.                   *
 *******************************************************/
void LoadPic(int fd, uchar val)
{
	uchar cmd[LPIC_LEN] = LPIC;
	cmd[2] = val;
	SendCommand(fd, cmd, LPIC_LEN);
}

/********************************************************
 * Function: ChangeBaudrate				*
 * Description : Change baudrate			*
 *******************************************************/
void ChangeBaudrate(int fd, long baudrate){
	char cData[5];
	int index;

	index=GetBaudrateIndex(baudrate);
	cData[0] = 0x1B;
	cData[1] = 'R';
	int i;
	for (i = 0; i < 4; i++)
		cData[i + 2] = BaudrateString[index][i];
		
	write(fd,cData,6);
}

/********************************************************
 * Function: CAN					*
 * Description : Clear current line			*
 *******************************************************/
void Can(int fd){
	uchar cmd[CAN_LEN]=CAN;
	SendCommand(fd, cmd, CAN_LEN);
}

/********************************************************
 * Function: SetBackLight				*
 * Description : Set back light	 			*
 *******************************************************/
void SetBackLight(int fd, uchar light){
	uchar cmd[SBL_LEN]=SBL;
	cmd[2]=light;
	SendCommand(fd, cmd, SBL_LEN);
}

/********************************************************
 * Function: SetLED                                     *
 * Description : Set LED on/off.                        *
 *******************************************************/
void SetLED(int fd, uchar val)
{
	uchar cmd[SLED_LEN] = SLED;
	cmd[2] = val;
	SendCommand(fd, cmd, SLED_LEN);
}

/********************************************************
 * Function: MoveRMost					*
 * Description : Move cursor to right-most		*
 *******************************************************/
void MoveRMost(int fd){
	uchar cmd[MCRM_LEN]=MCRM;
	SendCommand(fd, cmd, MCRM_LEN);
}

/********************************************************
 * Function: MoveLMost					*
 * Description : Move cursor to left-most		*
 *******************************************************/
void MoveLMost(int fd){
	uchar cmd[MCLM_LEN]=MCLM;
	SendCommand(fd, cmd, MCLM_LEN);
}

/********************************************************
 * Function: MoveU					*
 * Description : Move cursor up				*
 *******************************************************/
void MoveU(int fd){
	uchar cmd[MOVEU_LEN]=MOVEU;
	SendCommand(fd, cmd, MOVEU_LEN);
}

/********************************************************
 * Function: MoveD					*
 * Description : Move cursor down			*
 *******************************************************/
void MoveD(int fd){
	uchar cmd[MOVED_LEN]=MOVED;
	SendCommand(fd, cmd, MOVED_LEN);
}
#endif


#if defined(EZIO_300_CONFIG) || defined(EZIO_340_CONFIG) || defined(EZIO_390_CONFIG)
/********************************************************
 * Function: StopSend					*
 * Description : Stop send message			*
 *******************************************************/
void  StopSend (int fd) {
#if defined(ENABLE_STOP)
	uchar cmd[STOPEND_LEN]=STOPEND; 
	SendCommand(fd, cmd, STOPEND_LEN);
#endif /* ENABLE_STOP */
}

/********************************************************
 * Function: ReadKey					*
 * Description : Read keypad				*
 *******************************************************/
void  ReadKey (int fd) {
	uchar cmd[READKEY_LEN]=READKEY;
	SendCommand(fd, cmd, READKEY_LEN);
	usleep(100000);
}

/********************************************************
 * Function: Blank					*
 * Description : Blank display				*
 *******************************************************/
void  Blank (int fd) {
	uchar cmd[BLANK_LEN]=BLANK; 
	SendCommand(fd, cmd, BLANK_LEN);
	usleep(100000);
}

/********************************************************
 * Function: TurnOn					*
 * Description : Turn on (blinking block cursor)	*
 *******************************************************/
void  TurnOn (int fd) {
	uchar cmd[TURN_LEN]=TURN; 
	SendCommand(fd, cmd, TURN_LEN);
}

/********************************************************
 * Function: ScrollL					*
 * Description : Scroll screen left			*
 *******************************************************/
void  ScrollL(int fd){
	uchar cmd[SCL_LEN]=SCL; 
	SendCommand(fd, cmd, SCL_LEN);
}

/********************************************************
 * Function: ScrollR					*
 * Description : Scroll screen right			*
 *******************************************************/
void  ScrollR(int fd){
	uchar cmd[SCR_LEN]=SCR; 
	SendCommand(fd, cmd, SCR_LEN);
}

/********************************************************
 * Function: SetDisollR					*
 * Description : Set character-generator address	*
 *******************************************************/
void  SetDis(int fd){
	uchar cmd[SETDIS_LEN]=SETDIS; 
	SendCommand(fd, cmd, SETDIS_LEN);
	usleep(100000);
}

/********************************************************
 * Function: Paint                                      *
 * Description : Create customized pattern              *
 *******************************************************/

void  Paint(int fd,char* str){
	uchar cmd[PAINT_LEN]={0xFE};
	uchar memsitetab[SITERANGE]=SITE;

	uchar len=strlen(str);

	if (len==INFOLEN)
	{
		uchar sum=0;
		uchar cnt=0;
		uchar ch=0;
		uchar digit=0;
		for (cnt = 0; cnt < 2; cnt++)
		{
			ch=str[cnt];
			digit=chtohex(ch);
			sum=(sum << 4) + digit;
		}


		if (sum>=SITERANGE)
			printf("The address is given from 00 to %02d!!\tpair-1\n", SITERANGE);
		else
		{
			cmd[1]=memsitetab[sum];
			for (cnt=2;cnt<10;cnt++)
			{
				uchar i=(cnt-1)*2;
				uchar value = 0, digit=0;
				int j=0;
				for (j = 0; j < 2; j++)
				{
					ch=str[i+j];
					if (!((ch>='0' && ch<='9') || (ch>='a' && ch<='f') || (ch>='A' && ch<='F'))){
						printf ("The information of -p is false!!\tpair-%d\n",cnt);
						break;
					}
					else{
						digit=chtohex(ch);
						value=(value << 4) + digit;
					}
				}
				cmd[cnt]=value;
			}
			SendCommand(fd, cmd,PAINT_LEN);
		}
	}
	else
		printf("The Information length of -p is false!!\n");
}

uchar chtohex(uchar ch)
{
	uchar digit=0;
	if (ch >= '0' && ch <= '9')
		return (uchar)(ch - '0');
	else if (ch >= 'a' && ch <= 'f')
		return (uchar)(ch - 'a' + 10);
	else if (ch >= 'A' && ch <= 'F')
		return (uchar)(ch - 'A' + 10);
	else
		return 0;
}


/********************************************************
 * Function: ShowMessage				*
 * Description : Display message to LCD line 1 & 2	*
 *******************************************************/
void  ShowMessage (int fd, char *str1 , char *str2){
	uchar cmd1[SL1_LEN]=SL1; 
	uchar cmd2[SL2_LEN]=SL2;
	SendCommand(fd, cmd1, SL1_LEN);
	usleep(1000);
	SendString(fd, str1, strlen(str1));
	usleep(1000);
	SendCommand(fd, cmd2, SL1_LEN);
	usleep(1000);
	SendString(fd, str2, strlen(str2));
	usleep(1000);
}

/********************************************************
 * Function: ShowMessage_1				*
 * Description : Display message to LCD line 1  	*
 *******************************************************/
void  ShowMessage_1 (int fd, char *str1){
	uchar cmd1[SL1_LEN]=SL1;
	SendCommand(fd, cmd1, SL1_LEN);
	usleep(1000);
	SendString(fd, str1, strlen(str1));
	usleep(1000);
}

/********************************************************
 * Function: ShowMessage_2				*
 * Description : Display message to LCD line  2		*
 *******************************************************/
void  ShowMessage_2 (int fd, char *str1){
	uchar cmd1[SL2_LEN]=SL2;
	write(fd, cmd1, 2);
	SendCommand(fd, cmd1, SL2_LEN);
	usleep(1000);
	SendString(fd, str1, strlen(str1));
	usleep(1000);
}

/********************************************************
 * Function: SendNoiseInit				*
 * Description : Send noise before command.		*
 *******************************************************/
void SendNoiseInit (int fd){
	uchar cmd[NOISE_LEN]=NOISE;
	SendCommand(fd, cmd, NOISE_LEN);
	Init(fd);
	usleep(1000);
}
/********************************************************
 * Function: ShowPattern                               *
 * Description : Display customized pattern  to LCD            *
 *******************************************************/
void ShowPattern (int fd, char cmd, int x, int y){
	PatternShow(fd,cmd,x,y);
	usleep(1000);
}
#endif


#if defined(EZIO_G500_CONFIG)
/********************************************************
 * Function: ShowMessage                                *
 * Description : Display message to LCD line 1 & 2      *
 *******************************************************/
void  ShowMessage (int fd, char *str1 , char *str2)
{
	uchar cmd1[SL1_LEN] = SL1; 
	uchar cmd2[SL2_LEN] = SL2;
	SendCommand(fd, cmd1, SL1_LEN);
	usleep(1000);
	SendString(fd, str1, strlen(str1));
	usleep(1000);
	SendCommand(fd, cmd2, SL1_LEN);
	usleep(1000);
	SendString(fd, str2, strlen(str2));
	usleep(1000);
}


/********************************************************
 * Function: ShowMessage_2                              *
 * Description : Display message to LCD line 2          *
 *******************************************************/
void  ShowMessage_2 (int fd, char *str1)
{
	uchar cmd1[SL2_LEN] = SL2;
	//write(fd, cmd1, 2);
	SendCommand(fd, cmd1, SL2_LEN);
	usleep(1000);
	SendString(fd, str1, strlen(str1));
	usleep(1000);
}
#endif
