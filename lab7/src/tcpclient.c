#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <getopt.h>

#define BUFSIZE 100
#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)

int main(int argc, char *argv[])
{
  int bufsize = -1;

  while (true)
  {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"bufsize", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c)
    {
    case 0:
    {
      switch (option_index)
      {
      case 0:
        bufsize = atoi(optarg);
        // TODO: your code here
        if (bufsize <= 0)
        {
          printf("bufsize must be positive");
          return 1;
        }
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    }
    break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (bufsize == -1)
  {
    fprintf(stderr, "Using: %s --bufsize 100 address port\n",
            argv[0]);
    return 1;
  }

  int fd;
  int nread;
  char buf[bufsize];
  struct sockaddr_in servaddr;
  if (argc < 3)
  {
    printf("Too few arguments \n");
    exit(1);
  }

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, argv[optind], &servaddr.sin_addr) <= 0)
  {
    perror("bad address");
    exit(1);
  }

  servaddr.sin_port = htons(atoi(argv[optind + 1]));

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0)
  {
    perror("connect");
    exit(1);
  }

  write(1, "Input message to send\n", 22);
  while ((nread = read(0, buf, bufsize)) > 0)
  {
    if (write(fd, buf, nread) < 0)
    {
      perror("write");
      exit(1);
    }
  }

  close(fd);
  exit(0);
}
