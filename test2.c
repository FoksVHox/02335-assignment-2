
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <assert.h>

#include "aq.h"

#include "aux.h"

/** 
 * Test 2: normal messages are never blocked.
 * Make test target using the following command:
 * make test TEST_FILE=test2.c 
 */

static AlarmQueue q;

void * producer (void * arg) {
  put_alarm(q, 1);
  put_normal(q, 2);
  put_normal(q, 3);
  put_normal(q, 4);
  put_alarm(q, 5);
  put_normal(q, 6);
  put_normal(q, 7);
  return 0;
}

void * consumer(void * arg) {
  msleep(500);
  get(q);
  msleep(500);
  get(q);
  get(q);
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

  void * res1;
  void * res2;

  printf("----------------\n");

  /* Fork threads */
  pthread_create(&t1, NULL, producer, NULL);
  pthread_create(&t2, NULL, consumer, NULL);

  /* Join with all threads */
  pthread_join(t1, &res1);
  pthread_join(t2, &res2);

  printf("----------------\n");
  printf("Threads terminated with %ld, %ld\n", (uintptr_t) res1, (uintptr_t) res2);

  print_sizes(q);
  
  return 0;
}

