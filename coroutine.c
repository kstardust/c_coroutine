#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <stdint.h>

#include "coroutine.h"

struct coroutine {
  coroutine_func func;
  ucontext_t old_ctx;
  ucontext_t ctx;
  void *ret;
  void *args;
  int done;
  stack_t stack;
  struct co_loop *loop;
};

struct co_loop {
  coroutine_t *current;
  coroutine_t *next;
  int running;
  ucontext_t dispatcher;
  coroutine_t cos[MAX_COS];
};

static void
_co_call(coroutine_t *c)
{
  void *ret = c->func(c, c->args);
  c->done = 1;
  co_yield(c, ret);
  
  // never returns  
  assert(0);
}

int
co_finished(coroutine_t *c)
{
  return c->done;
}

coroutine_t*
create_co(co_loop_t *loop, coroutine_func func, void *args)
{
  coroutine_t *co = NULL;
  for (int i = 0; i < MAX_COS; i++) {
    if (loop->cos[i].func == NULL) {
      co = loop->cos + i;
      break;
    }
  }

  if (co != NULL) {
    getcontext(&co->ctx);
    co->ctx.uc_stack.ss_sp = co->stack.ss_sp;
    co->ctx.uc_stack.ss_size = co->stack.ss_size;
    co->func = func;
    co->ret = NULL;
    co->args = args;
    co->done = 0;
    makecontext(&co->ctx, (void (*)())_co_call, 1, (void*)co);
  }
  
  return co;
}

void
del_co(coroutine_t *c)
{
  c->func = NULL;
  c->ret = NULL;
}

void
co_dispatcher(co_loop_t *loop)
{}

int
run_forever(co_loop_t *loop)
{
  if (loop->running) {
    printf("error: loop is already running.");
    return -1;
  }
  loop->running = 1;
 
  while (loop->running) {
    co_dispatcher(loop);
  }
  return 0;
}

co_loop_t*
co_create_loop()
{
  co_loop_t *l = (co_loop_t*)malloc(sizeof(co_loop_t));
  l->running = 0;
  l->current = NULL;
  
  for (int i = 0; i < MAX_COS; i++) {
    l->cos[i].func = NULL;
    l->cos[i].loop = l;
    l->cos[i].stack.ss_sp = malloc(SIGSTKSZ);
    l->cos[i].stack.ss_size = SIGSTKSZ;
  }
  
  return l;
}

void
co_yield(coroutine_t *c, void *ret)
{
  // printf("co_yield %p\n", c);
  c->ret = ret;
  
  // printf("co_yield %p before yield\n", c);
  
  int n = swapcontext(&c->ctx, &c->old_ctx);
  if (n != 0) {
    perror("co_yield error");
    assert(0);
  }
  
  // printf("co_yield %p yield return\n", c);  
}

void*
co_resume(coroutine_t *c)
{
  // printf("co_resume %p\n", c);
  
  assert(c->func != NULL);
  if (c->done) return NULL;
  
  c->ret = NULL;
  
  // printf("co_resume %p before swap\n", c);
  
  int n = swapcontext(&c->old_ctx, &c->ctx);
  if (n != 0) {
    perror("co_resume error");
    assert(0);
  }
  
  // printf("co_resume %p swap return\n", c);
  
  return c->ret;
}
