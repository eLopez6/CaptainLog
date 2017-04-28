#ifndef Project4
#define Project4

#define TRUE 1
#define FALSE 0
#define ERROR 0
#define PROTOCOL "tcp"
#define BUFLEN 1024
#define MAX_CLOG_SIZE 365
#define PORT_POS 1 // for server
#define HOST_POS 1
#define PORT_POS_C 2
#define REQUIRED_ARGC 2
#define REQUIRED_ARGC_C 3
#define QLEN 1
#define COMMAND_POS 0
#define ARG_OFFSET 2
#define MAX_DIGITS 4 // increased to allow \0
#define DIGIT_OFFSET 4
#define MAX_ENTRY_LEN 512
#define MAX_INDEX 364
#define DELAY_SEC 1
#define DECIMAL_RADIX 10
#define PERMS 0777
#define MAX_CHAR_DATE 20
#define FILENAME_LEN 21
#define FILENAME "./CaptainLogArchive "
#define FILE_EXT_LEN 5

#define WRITE_LOG '1'
#define READ_LOG '2'
#define CLEAR_LOG '3'
#define NUM_LOG '4'
#define LAST_MADE '5'
#define ARCHIVE_LOG '6'
#define QUIT '7'



int errexit (const char *format, const char *arg);
int usage (char *progname);
void setUpSocketsServer(char *port);
void setUpSockets(char *hostname, char *port);
void startacceptingComms(char *port);
void *zmalloc(unsigned int size);
int isnumber(char *num);
void writeLog(char *message);
void readAndPrintLog(int sd, char buffer[], int bytes);
int convertIndexStr(char *index);
void readFromSocket(int sd, char buffer[], int bytes);
void clearTheLog();
void entriesInLog();
void sendLog();
void writeLogEntry(char buffer[]);
void safeWrite(int sd, char *arg, int bytes);
void safeRead(int sd, char *dest, int bytes);
void archiveLog();


typedef struct clog {
  char *logs[MAX_CLOG_SIZE];
  unsigned short numLogs;
  char *mostRecentLog;
} CaptainLog;



#endif
