#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
// #include <limits.h>
#include <stdbool.h>
#include <string.h>
// #include <unistd.h>
#include <getopt.h>
#include <pthread.h>

struct Section {
	int mod;
	int begin;
	int end;
};

int result = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void PartialCalcMod(const struct Section *arg_pointer) {
	int local_result = 1;
	for (int i=(*arg_pointer).begin; i < (*arg_pointer).end; i++) {
		local_result *= i;
		local_result %= (*arg_pointer).mod;
	}
	pthread_mutex_lock(&mut);
	result *= local_result;
	result %= (*arg_pointer).mod;
	// printf("Result: %d\n", result);
	pthread_mutex_unlock(&mut);
}

int main(int argc, char **argv) {
  uint32_t k = 0;
  uint32_t pnum = 0;
  uint32_t mod = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
            if (k <= 0){
              printf("k must be positive");
              return 1;
            }
            break;
          case 1:
            pnum = atoi(optarg);
            if (pnum <= 0 || pnum > k || k % pnum != 0){
              printf("pnum must be positive, less than k and k \% pnum must be zero");
              return 1;
            }
            break;
          case 2:
            mod = atoi(optarg);
            if (mod <= 0){
              printf("mod must be positive");
              return 1;
            }
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (k == 0 || pnum == 0 || mod == 0) {
    printf("Usage: %s -k \"num\" --pnum \"num\" --mod \"num\" \n",
           argv[0]);
    return 1;
  }

  pthread_t threads[pnum];
  int sect_length = k / pnum;
  struct Section args[pnum];
  for (uint32_t i = 0; i < pnum; i++) {
	// printf("1\n");
	args[i].mod = mod;
	args[i].begin = i * sect_length + 1;
    args[i].end = args[i].begin + sect_length;
    if (pthread_create(&threads[i], NULL, PartialCalcMod, (void *)&(args[i]))) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }
  for (uint32_t i = 0; i < pnum; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("FinResult: %d\n", result);
  return 0;
}
