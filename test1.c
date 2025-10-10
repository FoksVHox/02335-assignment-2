
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "aq.h"

#include "aux.h"

/** 
 * Test with two concurrent producers. Alarm '2' waits for Alarm '1' to be read before thread proceeds.
 * Make test target using the following command:
 * make test TEST_FILE=test1.c 
 */

static AlarmQueue q;

void * producer1 (void * arg) {
  put_alarm(q, 1);
  put_normal(q, 2);
  return 0;
}

void * producer2 (void * arg) {
  msleep(100);
  put_alarm(q, 3);
  put_normal(q, 4);
  return 0;
}

void * consumer(void * arg) {
  msleep(500);
  get(q);
  get(q);
  get(q);
  get(q);
  return 0;
}

int main(int argc, char ** argv) {
    int ret;

  q = aq_create();

  if (q == NULL) {
    printf("Alarm queue could not be created\n");
    exit(1);
  }
  
  pthread_t t1;
  pthread_t t2;
  pthread_t t3;

  void * res1;
  void * res2;
  void * res3;

  printf("----------------\n");

  /* Fork threads */
  pthread_create(&t1, NULL, producer1, NULL);
  pthread_create(&t2, NULL, producer2, NULL);
  pthread_create(&t3, NULL, consumer, NULL);

  /* Join with all threads */
  pthread_join(t1, &res1);
  pthread_join(t2, &res2);
  pthread_join(t3, &res3);

  printf("----------------\n");
  printf("Threads terminated with %ld, %ld, %ld\n", (uintptr_t) res1, (uintptr_t) res2, (uintptr_t) res3);

  print_sizes(q);
  
  return 0;
}

