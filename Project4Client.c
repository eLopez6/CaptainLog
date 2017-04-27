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

  // if (connect (sd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  //     errexit ("cannot connect", NULL);

  while (TRUE)
  {
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
      strncpy(commandArgs, buffer + ARG_OFFSET, BUFLEN); // copies the argument
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
      if (write(sd, buffer, BUFLEN - 1) < 0)
        errexit("error in writing to socket: %s", buffer);
    }
    else
    {
      printf("quitting application\n");
      free(commandArgs);
      close(sd);
      exit(1);
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
