#ifndef Project4
#define Project4

#define TRUE 1
#define FALSE 0
#define ERROR 0
#define PROTOCOL "tcp"
#define BUFLEN 1024
#define MAX_LOG_LENGTH 500
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

#define WRITE_LOG '1'
#define READ_LOG '2'
#define CLEAR_LOG '3'
#define NUM_LOG '4'
#define LAST_MADE '5'
#define QUIT '6'



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


typedef struct clog {
  char *logs[MAX_CLOG_SIZE];
  unsigned short numLogs;
  char *mostRecentLog;
} CaptainLog;



#endif
