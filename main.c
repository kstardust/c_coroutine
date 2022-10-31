#include <stdio.h>
#include <stdint.h>
#include "coroutine.h"

void*
counter(coroutine_t *c, void* arg) {
  uintptr_t n = (uintptr_t)arg;
  while (n-- > 1) {
    co_yield(c, (void*)n);
  }
  return (void*)n;
}

int main() {
    
  co_loop_t *loop = co_create_loop();
  coroutine_t *counter_co = create_co(loop, counter, (void*)10);

  while (!co_finished(counter_co)) {
    printf("%lu\n", (uintptr_t)co_resume(counter_co));
  }
  
  del_co(counter_co);

  counter_co = create_co(loop, counter, (void*)20);
  
  while (!co_finished(counter_co)) {
    printf("%lu\n", (uintptr_t)co_resume(counter_co));
  }

  return 0;
}
