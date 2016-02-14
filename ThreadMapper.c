/*
 *  many2one mapping v1.0: 
 *  the library provides uthread_create, uthread_yield and uthread_exit 
*/

#define _XOPEN_SOURCE 600
#define NUM_THREADS 3

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <pthread.h>

 struct thread_info{
  ucontext_t *ucp;
  struct thread_info *next;
};
struct thread_info *head=NULL, *tail=NULL;

//declare the functions provided by the thread library 
void uthread_create(void (*func)());
void uthread_exit();
void uthread_yield();

/**** Application code ****/

void th1()
{
  int i;
  sleep(1);
  for(i=0;i<3;i++){
    printf("This is thread 1.\n");
    //sleep(1);
    uthread_yield();
  }
  printf("Thread 1 exit.\n");
  uthread_exit();
}

void th2()
{
  int i;
  sleep(1);
  for(i=0;i<3;i++){
    printf("This is thread 2.\n");fflush(stdout);
    //sleep(1);
    uthread_yield();
  }
  printf("Thread 2 exit.\n");
  uthread_exit();
}

void th3()
{
  int i;
  sleep(1);
  for(i=0;i<3;i++){
    printf("This is thread 3.\n");fflush(stdout);
    //sleep(1);
    uthread_yield();
  }
  printf("Thread 3 exit.\n");
  uthread_exit();
}

int main()
{
  uthread_create(th1);
  uthread_create(th2);
  uthread_create(th3);

  pthread_t threads[NUM_THREADS];
  int rc, t=0;


  printf("Creating thread %d\n", 1);
  rc = pthread_create(&threads[0], NULL, th1, (void *) 0);

  printf("Creating thread %d\n", 2);
  rc = pthread_create(&threads[1], NULL, th2, (void *) 1);

  printf("Creating thread %d\n", 3);
  rc = pthread_create(&threads[2], NULL, th3, (void *) 2);

  //uthread_exit();
}

void uthread_create(void (*func)())
{
  //construct a thread record
  struct thread_info *th;
  th=(struct thread_info *)malloc(sizeof(struct thread_info));
  th->ucp=(ucontext_t *)malloc(sizeof(ucontext_t));
  getcontext(th->ucp); //initialize the context structure
  th->ucp->uc_stack.ss_sp=(void *)malloc(16384);
  th->ucp->uc_stack.ss_size=16384;
  makecontext(th->ucp, th1, 0); //make the context for a thread running func
  th->next=NULL;

  //add the thread record into the queue
  if(tail!=NULL) tail->next=th;
  else head=th;
  tail=th;
}

void uthread_exit()
{
  struct thread_info *th;

  if(head==NULL) exit(0); //all threads are finished

  //pick up the thread record at the front of the queue
  th=head;
  head=head->next;
  if(head==NULL) tail=NULL;

  //set the context of the picked thread as the concurrent thread (i.e., run it!)
  setcontext(th->ucp);
}

void uthread_yield()
{
  if(head==NULL) return;

  //construct a thread record th 
  struct thread_info *th;
  th=(struct thread_info *)malloc(sizeof(struct thread_info));
  th->ucp=(ucontext_t *)malloc(sizeof(ucontext_t));


  //apend the record to the queue
  if(tail!=NULL) tail->next=th;
  else head=th;
  tail=th;

  //fectch the front thread record th1
  struct thread_info *th1;
  th1=head;
  head=head->next;

  //swap th1 with th
  swapcontext(th->ucp, th1->ucp);
}