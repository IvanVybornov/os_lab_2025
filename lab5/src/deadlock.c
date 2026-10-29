#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_func(void* arg) {
    pthread_mutex_lock(&mutex1);
    printf("Поток 1: заблокировал mutex1\n");

    sleep(1); // необходимо ожидание, иначе поток1 может завершить выполнение ф-ции ещё до того, как поток2 заблокирует мутекс2 => дедлока не будет

    pthread_mutex_lock(&mutex2);
    printf("Поток 1: заблокировал mutex2\n");

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    return NULL;
}

void* thread2_func(void* arg) {
    pthread_mutex_lock(&mutex2);
    printf("Поток 2: заблокировал mutex2\n");

    sleep(1);

    pthread_mutex_lock(&mutex1);
    printf("Поток 2: заблокировал mutex1\n");

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}