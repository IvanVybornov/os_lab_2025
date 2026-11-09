#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <math.h>

#include <pthread.h>
#include "mulmod.h"

uint64_t end_result = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

struct ArgsForThread
{
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
  int sck;
};

struct Server
{
  char ip[255];
  int port;
};

bool ConvertStringToUI64(const char *str, uint64_t *val)
{
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE)
  {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

void work_with_server(const struct ArgsForThread *arg_pointer)
{
  uint64_t begin = arg_pointer->begin;
  uint64_t end = arg_pointer->end;
  uint64_t mod = arg_pointer->mod;
  int sck = arg_pointer->sck;

  char task[sizeof(uint64_t) * 3];
  memcpy(task, &begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0)
  {
    fprintf(stderr, "Send failed\n");
    exit(1);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0)
  {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }

  // TODO: from one server
  // unite results
  uint64_t answer = 0;
  memcpy(&answer, response, sizeof(uint64_t));
  // printf("answer: %llu\n", answer);
  pthread_mutex_lock(&mut);
  end_result = MultModulo(end_result, answer, mod);
  pthread_mutex_unlock(&mut);
}

int main(int argc, char **argv)
{
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true)
  {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
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
        ConvertStringToUI64(optarg, &k);
        // TODO: your code here
        if (k <= 0)
        {
          printf("k must be positive");
          return 1;
        }
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        // TODO: your code here
        if (mod <= 0)
        {
          printf("mod must be positive");
          return 1;
        }
        break;
      case 2:
        // TODO: your code here
        memcpy(servers, optarg, strlen(optarg));
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

  if (k == -1 || mod == -1 || !strlen(servers))
  {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  // // TODO: for one server here, rewrite with servers from file
  // unsigned int servers_num = 1;
  // struct Server *to = malloc(sizeof(struct Server) * servers_num);
  // // TODO: delete this and parallel work between servers
  // to[0].port = 20001;
  // memcpy(to[0].ip, "127.0.0.1", sizeof("127.0.0.1"));

  struct Server *to = NULL;
  char buffer[255];
  unsigned int servers_num = 0;
  FILE *servers_file = fopen(servers, "r");
  while (fgets(buffer, sizeof(buffer), servers_file))
  {
    servers_num++;                                // считали сервер, круто, увеличиваем счётчик
    char *server_ip = strtok(buffer, ":\n");      // получаем первую лексемму (то, что до двоеточия) - IP адрес
    char *server_port = strtok(NULL, ":\n");      // продолжаем читать buffer дальше, то есть port
    to = realloc(to, servers_num);                // реаллоцируем память
    to[servers_num - 1].port = atoi(server_port); // назначаем порт в последнем элементе массива
    // printf("PORT: %s\n", to[servers_num - 1].port);
    memcpy(to[servers_num - 1].ip, server_ip, strlen(server_ip) * sizeof(char)); // теперь IP
    printf("IP: %s\n", server_ip);
    printf("IP: %s, PORT: %d\n", to[servers_num - 1].ip, to[servers_num - 1].port);
    // printf("IP: %s, PORT: %s\n", server_ip, server_port);
  }

  int sck_arr[servers_num];
  pthread_t threads[servers_num];
  struct ArgsForThread args[servers_num];
  uint64_t sect_length = ceil((double)k / (double)servers_num);

  // TODO: work continiously, rewrite to make parallel
  for (int i = 0; i < servers_num; i++)
  {
    struct hostent *hostname = gethostbyname(to[i].ip);
    if (hostname == NULL)
    {
      fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
      exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(to[i].port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

    sck_arr[i] = socket(AF_INET, SOCK_STREAM, 0);
    if (sck_arr[i] < 0)
    {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    if (connect(sck_arr[i], (struct sockaddr *)&server, sizeof(server)) < 0)
    {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }

    // TODO: for one server
    // parallel between servers
    args[i].begin = i * sect_length + 1;
    args[i].end = (args[i].begin + sect_length > k) ? k + 1 : args[i].begin + sect_length;
    args[i].mod = mod;
    args[i].sck = sck_arr[i];

    if (pthread_create(&threads[i], NULL, work_with_server, (void *)&(args[i])))
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  for (int i = 0; i < servers_num; i++)
  {
    pthread_join(threads[i], NULL);
    close(sck_arr[i]);
  }

  printf("End Result: %d\n", end_result);

  free(to);

  return 0;
}
