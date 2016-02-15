
/*
 *  many2one mapping v1.0: 
 *  the library provides enqueue_context, yield and dequeue_context 
*/

#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>

 typedef struct context_queue Queue;

//declare the functions provided by the thread library 
Queue* create_context(int (*func)());
Queue* dequeue_context();
void enqueue_context(Queue *context);
void yield();

/**** Application code ****/



struct context_queue{
  ucontext_t *context;
  Queue *next;
};
Queue *head=NULL, *tail=NULL;

int func1()
{
  int i;
  printf("Sum of First 200 numbers by Thread 1 \n");
  int sum = 0;
  int yield_1=0,yield_2=0;
  for(i=1;i<=200;i++){

    sum = sum + i;
    if(sum >= 100 && yield_1 == 0){
      sleep(1);
      yield_1 = 1;
      printf("Sum by Thread 1 reached 100 - Yielding for Next Thread\n");
      yield();
    }
    if(sum >= 200 && yield_2 == 0){
      sleep(1);
      yield_2 = 1;
      printf("Sum by Thread 1 reached 200 - Yielding for Next Thread\n");
      yield();
    }
  }
  printf("Sum of First 200 Numbers = %d\n",sum);
  dequeue_context();
  if(head != NULL){
    setcontext(head->context);
  }
}

int func2()
{
  int i;
  long product = 1;
  printf("Product of first 20 numbers by Thread 2\n");
  int yield_1=0,yield_2=0;
  for(i=1;i<=20;i++){
    product = product * i;
    if(product > 100 && yield_1 == 0){
      sleep(1);
      yield_1 = 1;
      printf("Product by Thread 2 has reached 100 - Yielding for Next Thread\n");
      yield();
    }
    if(product > 500 && yield_2 == 0){
      sleep(1);
      yield_2 = 1;
      printf("Product by Thread 2 has reached 500 - Yielding for Next Thread\n");
      yield();
    }
  }
  printf("Product of First 20 Numbers = %ld\n",product);
  dequeue_context();
  if(head != NULL){
    setcontext(head->context);
  }
}

int func3()
{
  int i;
  int even_sum = 0;
  printf("Sum of First 100 Even Numbers by Thread 3\n");
  for(i=1;i<=100;i=i+2){
    even_sum = even_sum + i;
    if(even_sum == 100){
      sleep(1);
      printf("Even Sum by Thread 3 Reached 100. Sum upto %d is %d\n",i,even_sum);
      yield();
    }
  }
  printf("Sum of First 100 Even Numbers = %d\n",even_sum);
  dequeue_context();
  if(head != NULL){
    setcontext(head->context);
  }
  
}

int main()
{
  //create_context(func1);
  enqueue_context(create_context(func1));

  //create_context(func2);
  enqueue_context(create_context(func2));

  //create_context(func3);
  enqueue_context(create_context(func3));
  
  
  setcontext(head->context);
}


/**** Thread library code ****/

//define a queue of user thread records



Queue* create_context(int (*func)())
{
  //construct a thread record
  Queue *new_context;
  new_context=(Queue *)malloc(sizeof(Queue));
  new_context->context=(ucontext_t *)malloc(sizeof(ucontext_t));
  getcontext(new_context->context); //initialize the context structure
  new_context->context->uc_stack.ss_sp=(void *)malloc(16384);
  new_context->context->uc_stack.ss_size=16384;
  makecontext(new_context->context, func, 0); //make the context for a thread running func
  new_context->next=NULL;

  return new_context;
}

Queue* dequeue_context()
{
  if(head==NULL) return NULL;
  
  Queue *temp;
  temp = head;

  head = head->next;


  return temp;

}

void enqueue_context(Queue *context){
  if(head == NULL){
    head = context;
    return;
  }else{
    Queue *temp = head;

    while(temp->next != NULL){
      temp = temp->next;
    }

    temp->next = context;
  }

  return;
  
}

void yield()
{
  if(head==NULL) return;

  Queue *context = dequeue_context();


  Queue *th; 
  th=(Queue *)malloc(sizeof(Queue)); 
  th->context=(ucontext_t *)malloc(sizeof(ucontext_t));


  Queue *temp_head = head;

  enqueue_context(th);
  swapcontext(th->context, temp_head->context);

}