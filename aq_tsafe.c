/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include "stdlib.h"
#include <pthread.h>

typedef struct Node {
  void *msg;
  char MsgKind;
  struct Node * next;
} Node;

typedef struct AlarmQueueimplementation {
  Node * head;
  Node * tail;
  int size;
  int alarmcount;

  pthread_mutex_t mtx;
  pthread_cond_t not_empty;
  pthread_cond_t alarm_gone;
} AlarmQueueimplementation;


AlarmQueue aq_create( ) {
  AlarmQueueimplementation *aq = malloc(sizeof(AlarmQueueimplementation));
  if (!aq) return NULL;
  aq->head = NULL;
  aq->tail = NULL;
  aq->size = 0;
  aq->alarmcount = 0;
  pthread_mutex_init(&aq->mtx, NULL);
  pthread_cond_init(&aq->not_empty, NULL);
  pthread_cond_init(&aq->alarm_gone, NULL);
  return (AlarmQueue) aq;
}

int aq_send( AlarmQueue aq, void * msg, MsgKind k){
  AlarmQueueimplementation *queue = (AlarmQueueimplementation *)aq;

  pthread_mutex_lock(&queue->mtx);

  while (queue->alarmcount > 0 && k == AQ_ALARM) {
    pthread_cond_wait(&queue->alarm_gone, &queue->mtx);
  }
  Node *n = malloc(sizeof(Node));
  if (!n) {
    pthread_mutex_unlock(&queue->mtx);
    return AQ_NULL_MSG;
  }
  n->msg = msg;
  n->MsgKind = k;
  n->next = NULL;
  if (k == AQ_ALARM) {
      n->next = queue->head;
      queue->head = n;
      if (queue->tail == NULL) {
          queue->tail = n;
      }
      queue->alarmcount++;
    }else {
        if (queue->tail == NULL) {
        queue->head = queue->tail = n;
    } else {
      queue->tail->next = n;
      queue->tail = n;
    } 
  }
  queue->size++;
  pthread_cond_signal(&queue->not_empty); // wake one receiver
  pthread_mutex_unlock(&queue->mtx);
  return 0;
}

int aq_recv(AlarmQueue aq, void **msg) {
    if (!aq) return AQ_UNINIT;
    AlarmQueueimplementation *queue = (AlarmQueueimplementation *)aq;
    pthread_mutex_lock(&queue->mtx);
    while (queue->size == 0){
      pthread_cond_wait(&queue->not_empty, &queue->mtx);
    }
    Node *n = queue->head;
    queue->head = n->next;
    if (queue->tail == n) queue->tail = NULL;
    queue->size--;
    if (n->MsgKind == AQ_ALARM){
      queue->alarmcount--;
      pthread_cond_broadcast(&queue->alarm_gone); // wake all alarm senders
    }
    *msg = n->msg;
    int kind = n->MsgKind;
    free(n);
    pthread_mutex_unlock(&queue->mtx);
    return kind;
}

int aq_size( AlarmQueue aq) {
  AlarmQueueimplementation *queue = (AlarmQueueimplementation *)aq;
  pthread_mutex_lock(&queue->mtx);
  int s = queue->size;
  pthread_mutex_unlock(&queue->mtx);
  return s;
}

int aq_alarms( AlarmQueue aq) {
  AlarmQueueimplementation *queue = (AlarmQueueimplementation *)aq;
  pthread_mutex_lock(&queue->mtx);
  int a = queue->alarmcount;
  pthread_mutex_unlock(&queue->mtx);
  return a;
}



