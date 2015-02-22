#include <stdint.h>
#include <sys/time.h>
#include <list.h>


struct timerStruct structList[10];

#define TIMER_CYCLES
#define TIMER_USEC

struct timerStruct{
  struct timeval begin, end;
  double elapsed_time;
  int mode, numUsed;
  long int min, max, sum;
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
  long int temp;
  temp = (end.tv_sec - begin.tv_sec)*1000000L + end.tv_usec - begin.tv_usec;
  if(temp > structList[timer_id].max){ structList[timer_id].max = temp; }
  if(temp < structList[timer_id].min){ structList[timer_id].min = temp; }
  structList[timer_id].sum = structList[timer_id].sum + temp;
  structList[timer_id].numUsed++;
  return temp;
}

uint64_t timer_min(int timer_id){
  return structList[timer_id].min;
}

uint64_t timer_max(int timer_id){
  return structList[timer_id].max;
}

uint64_t timer_avg(int timer_id){
  return (structList[timer_id].sum / structList[timer_id].numUsed);
}

uint64_t timer_end(int timer_id, struct histogram* hist ){
  //Not applicable to the current implementation.
}





