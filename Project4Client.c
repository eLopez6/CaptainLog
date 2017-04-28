// Emilio Lopez
// eil11
// Project4Client.c
// Date Created: 4/13/2017
// Code for the entire Project 4 Client

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Project4.h"

static struct sockaddr_in sin;
static struct hostent *hinfo;
static struct protoent *protoinfo;
static int sd; //ret;

int main(int argc, char *argv[])
{
  char buffer [BUFLEN];

  // Command must at least have exec name, port, command number
  if (argc != REQUIRED_ARGC_C)
    errexit("invalid number of arguments", NULL);

  setUpSockets(argv[HOST_POS], argv[PORT_POS_C]);


  while (TRUE)
  {
    int test_i;
    char commandNo;
    char *commandArgs;

    memset(buffer,0x0,BUFLEN);
    if (fgets(buffer, BUFLEN - 1, stdin) == NULL)
      errexit("error in reading standard input", NULL);

    commandNo = buffer[COMMAND_POS];

    if (!commandNo)
    {
      printf("no command entered, please try again\n");
      continue;
    }

    if (!isdigit(commandNo))
    {
      printf("invalid command entered, must be a digit\n");
      continue;
    }

    if (!(buffer[ARG_OFFSET])) // checks that there is a character for an arguments
      commandArgs = NULL;
    else
    {
      commandArgs = (char *)zmalloc(BUFLEN);
      // commandArgs = strtok(buffer, "\n");  // removes \n from argumetns
      strncpy(commandArgs, strtok(buffer + ARG_OFFSET, "\n"), BUFLEN); // copies the argument
    }

    //ensures only valid commands get processed (and quit)
    if (commandNo <= LAST_MADE && commandNo >= WRITE_LOG)
    {
      // checks if the command is write or read, which require arguments
      if (commandNo == WRITE_LOG || commandNo == READ_LOG)
      {
        if (!commandArgs)
        {
          printf("error in write or reading, no arguments\n");
          continue;
        }
      }
    }
    else
    {
      printf("quitting application\n");
      free(commandArgs);
      close(sd);
      exit(1);
    }

    //Writes command char to server so it can process next read correctly
    if (write(sd, &commandNo, 1) < 0)
      errexit("error in writing to socket: %s", buffer);

    switch (commandNo)
    {
      case WRITE_LOG:
        write(sd, commandArgs, strlen(commandArgs));
        break;

      case READ_LOG:
        if (isnumber(commandArgs))
        {
          test_i = convertIndexStr(commandArgs);
          if (test_i <= MAX_INDEX)
            write(sd, commandArgs, strlen(commandArgs));
        }
        memset(buffer, 0x0, BUFLEN);

        sleep(DELAY_SEC);

        if (read(sd, buffer, MAX_ENTRY_LEN) < 0)
          errexit("error reading sd for read log", NULL);

        printf("%s\n", buffer);
    }


    free(commandArgs);
  }
  printf("exiting");
  return TRUE;
}

void setUpSockets(char *hostname, char *port)
{
  hinfo = gethostbyname (hostname);
  if (hinfo == NULL)
      errexit ("cannot find name: %s", port);

  /* set endpoint information */
  memset ((char *)&sin, 0x0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(atoi(port));
  memcpy ((char *)&sin.sin_addr,hinfo->h_addr,hinfo->h_length);

  if ((protoinfo = getprotobyname (PROTOCOL)) == NULL)
      errexit ("cannot find protocol information for %s", PROTOCOL);

  /* allocate a socket */
  /*   would be SOCK_DGRAM for UDP */
  sd = socket(PF_INET, SOCK_STREAM, protoinfo->p_proto);
  if (sd < 0)
      errexit("cannot create socket",NULL);

  /* connect the socket */
  if (connect (sd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
      errexit ("cannot connect", NULL);
}

int isnumber(char *num)
{
  unsigned int i;

  for (i = 0; i < strlen(num); i++)
    if (!(isdigit(num[i])))
      return FALSE;

  return TRUE;
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

void readAndPrintLog(int sd, char buffer[], int bytes)
{
  int r;
  if ((r = read(sd, buffer, bytes)) < 0)
    errexit("error in reading bytes, exiting", NULL);

  printf("%s\n", buffer);
}


int convertIndexStr(char *index)
{
  int i;
  char *indexStr = (char *)zmalloc(MAX_DIGITS);
  strncpy(indexStr, index + ARG_OFFSET, DIGIT_OFFSET);
  i = atoi(indexStr);
  free(indexStr);
  return i;
}
