/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include "stdlib.h"

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
} AlarmQueueimplementation;


AlarmQueue aq_create( ) {
  AlarmQueueimplementation *aq = malloc(sizeof(AlarmQueueimplementation));
  if (!aq) return NULL;
  aq->head = NULL;
  aq->tail = NULL;
  aq->size = 0;
  aq->alarmcount = 0;
  return (AlarmQueue) aq;
}

int aq_send( AlarmQueue aq, void * msg, MsgKind k){
  AlarmQueueimplementation *queue = (AlarmQueueimplementation *)aq;
  if (queue->alarmcount > 0 && k == AQ_ALARM) {
    return AQ_NO_ROOM;
  }
  Node *n = malloc(sizeof(Node));
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
  return 0;
}

int aq_recv(AlarmQueue aq, void **msg) {
    if (!aq) return AQ_UNINIT;
    AlarmQueueimplementation *queue = (AlarmQueueimplementation *)aq;
    if (queue->size == 0 || !queue->head) return AQ_NO_MSG;
    Node *n = queue->head;
    queue->head = n->next;
    if (queue->tail == n) queue->tail = NULL;
    queue->size--;
    if (n->MsgKind == AQ_ALARM) queue->alarmcount--;
    *msg = n->msg;
    int kind = n->MsgKind;
    free(n);
    return kind;
}

int aq_size( AlarmQueue aq) {
  AlarmQueueimplementation *queue = (AlarmQueueimplementation *)aq;
  return queue->size;
}

int aq_alarms( AlarmQueue aq) {
  AlarmQueueimplementation *queue = (AlarmQueueimplementation *)aq;
  return queue->alarmcount;
}



