// Emilio Lopez
// eil11
// Project4Server.c
// Date Created: 4/13/2017
// Code for the entire Project 4 Server

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Project4.h"

static int sd, sd2;
static struct sockaddr_in sin;
static struct sockaddr addr;
static unsigned int addrlen;

static CaptainLog c_log;

int main(int argc, char *argv [])
{
  char buffer[BUFLEN];
  const char *errorMess;

  if (argc != REQUIRED_ARGC)
    errexit("invalid number of arguments", NULL);

  setUpSocketsServer(argv[PORT_POS]);

  c_log.numLogs = 0;
  c_log.mostRecentLog = NULL;


  // Accepts connections
  while(TRUE)
  {
    startacceptingComms(argv[PORT_POS]);
    memset(buffer, 0x0, BUFLEN);

    //Reads from socket
    while (read(sd2, buffer, 1) > 0)
    {
      char commandNo;

      // Reads into command struct the contents of the read
      commandNo = buffer[COMMAND_POS];

      switch (commandNo)
      {
        case WRITE_LOG:
          writeLogEntry(buffer);
          break;

        case READ_LOG:
          sendLog();
          break;

        case CLEAR_LOG:
          clearTheLog();
          break;

        case NUM_LOG:
          entriesInLog();
          break;

        case LAST_MADE:
          write(sd2, c_log.mostRecentLog, strlen(c_log.mostRecentLog));
          break;

        default:
          errorMess = "invalid command\n";
          if (write (sd2, errorMess, strlen(errorMess)) < 0)
            errexit ("invalid command", NULL);
          break;
      }

    }
    close(sd2);
  }

  return TRUE;
}

int isnumber(char *num)
{
  unsigned int i;

  for (i = 0; i < strlen(num); i++)
    if (!(isdigit(num[i])))
      return FALSE;

  return TRUE;
}

// int safeWrite(int sd, char *arg, int bytes)
// {
//
// }

void setUpSocketsServer(char *port)
{
  struct protoent *protoinfo;

  if (isnumber(port) < 1)
    errexit("invalid port entered, must be a number: %s", port);

  if ((protoinfo = getprotobyname (PROTOCOL)) == NULL)
    errexit ("cannot find protocol information for %s", PROTOCOL);

  memset ((char *)&sin,0x0,sizeof (sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons ((u_short) atoi(port));

  /* allocate a socket */
  sd = socket(PF_INET, SOCK_STREAM, protoinfo->p_proto);
  if (sd < 0)
    errexit("cannot create socket", NULL);

  if (bind (sd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    errexit ("cannot bind to port %s", port);

  /* listen for incoming connections */
  if (listen (sd, QLEN) < 0)
    errexit ("cannot listen on port %s\n", port);
}

void startacceptingComms(char *port)
{
  sd2 = accept(sd,&addr,&addrlen);
  if (sd2 < 0)
    errexit ("error accepting connection", NULL);
}

void writeLogEntry(char buffer[])
{
  char *commandArgs = (char *)zmalloc(BUFLEN);
  memset(buffer, 0x0, BUFLEN);

  if (read(sd2, buffer, BUFLEN - 1) > 0)
    strncpy(commandArgs, buffer, BUFLEN - 1);  // copies message and adds \0
  else
    errexit("error reading WRITE_LOG args", NULL);

  if (strlen(commandArgs) > 0)
    writeLog(commandArgs);

  free(commandArgs);
}

void sendLog()
{
  int read_index;
  char *commandArgs = (char *)zmalloc(MAX_DIGITS);

  if (read(sd2, commandArgs, MAX_DIGITS) < 0)
    errexit("error reading for READ_LOG", NULL);
  read_index = atoi(commandArgs);

  if ((c_log.logs[read_index] != NULL))
    write(sd2, c_log.logs[read_index], strlen(c_log.logs[read_index]));
  else
    write(sd2, "no log for that entry", MAX_ENTRY_LEN);

  free(commandArgs);
}



/* K & R Reverse function found on pg. 62 */
void reverse(char s[])
{
  int c, i, j;

  for (i = 0, j = strlen(s) - 1; i < j; i++)
  {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

/* K & R itoa function found on pg 64 */
void itoa(int n, char s[])
{
  int i;

  i = 0;
  do {
    s[i++] = n % DECIMAL_RADIX + '0';
  } while ((n /= DECIMAL_RADIX) > 0);
  s[i] = '\0';
  reverse(s);
}

void entriesInLog()
{
  char *numString = (char *)zmalloc(MAX_DIGITS);
  itoa(c_log.numLogs, numString);
  if (write(sd2, numString, strlen(numString)) < 0)
    errexit("error writing number of entries in the log\n", NULL);
}

void clearTheLog()
{
  int i;
  printf("Clearing the Captain's Log\n");

  for (i = 0; i < MAX_CLOG_SIZE; i++)
    if (c_log.logs[i] != NULL)
      c_log.logs[i] = NULL;
  c_log.numLogs = 0;
  c_log.mostRecentLog = NULL;
}

void writeLog(char *message)
{
  if (c_log.numLogs >= MAX_CLOG_SIZE)
  {
    perror("Captain's Log is full");
    exit(EXIT_FAILURE);
  }

  c_log.logs[c_log.numLogs] = (char *)zmalloc(MAX_ENTRY_LEN);
  strncpy(c_log.logs[c_log.numLogs], message, MAX_ENTRY_LEN);
  c_log.mostRecentLog = c_log.logs[c_log.numLogs];
  c_log.numLogs++;

  printf("Number of logs in the log after writing: %u\n", c_log.numLogs);
}

int usage (char *progname)
{
    fprintf (stderr,"usage: %s port msg\n", progname);
    exit (ERROR);
}

int errexit (const char *format, const char *arg)
{
    fprintf (stderr,format,arg);
    fprintf (stderr,"\n");
    exit (ERROR);
}

void *zmalloc(unsigned int size)
{
  void *p;

  if ((p = (void *)malloc(size)) == NULL)
  {
    perror("Memory allocation failed, exiting program");
    exit(EXIT_FAILURE);
  }

  memset(p, 0x0, size);
  return p;
}
