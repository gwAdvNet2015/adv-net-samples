#include <stdint.h>
#include <sys/time.h>
#include <list.h>


struct timerStruct structList[10];

#define TIMER_CYCLES
#define TIMER_USEC

struct timerStruct{
  struct timeval begin, end;
  double elapsed_time;
  int mode;
  struct double_ll* timeList;
};

void timer_set_mode(int timer_id, int mode_flag);
void timer_start(int timer_id);
uint64_t timer_end(int timer_id);
uint64_t timer_min(int timer_id);
uint64_t timer_max(int timer_id);
uint64_t timer_avg(int timer_id);
uint64_t timer_end(int timer_id, struct list);

void timer_set_mode(int timer_id, int mode_flag){
  structList[timer_id].mode = mode_flag;
}

void timer_start(int timer_id){
  gettimeofday(structList[timer_id].begin, NULL);
}

uint64_t timer_end(int timer_id){
  gettimeofday(structList[timer_id].end, NULL);
}

uint64_t timer_min(int timer_id){
  double temp = 1000000000;
  struct Node* move = structList[timer_id].timeList->head;
  while(move != NULL){
    if(move->value < temp){ temp = move->value; }
  }
  return temp;
}

uint64_t timer_max(int timer_id){
  double temp = 0;
  struct Node* move = structList[timer_id].timeList->head;
  while(move != NULL){
    if(move->value > temp){ temp = move->value; }
  }
  return temp;
}

uint64_t timer_avg(int timer_id){
  double temp = 0;
  int length = 0;
  struct Node* move = structList[timer_id].timeList->head;
  while(move != NULL){ 
    temp = temp + move->value;
    length++;
  }
  temp = temp / length;
  return temp;
}

uint64_t timer_end(int timer_id, struct timeList){
  //Figure this out.
}





