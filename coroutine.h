struct co_loop;
struct coroutine;

typedef struct co_loop co_loop_t;
typedef struct coroutine coroutine_t;

typedef void *(*coroutine_func)(coroutine_t *, void *);

#define MAX_COS 1024
#define CO_STACK_SIZE SIGSTKSZ

co_loop_t* co_create_loop();
coroutine_t* create_co(co_loop_t *loop, coroutine_func func, void *args);

void co_yield(coroutine_t *c, void *ret);
void* co_resume(coroutine_t *c);
int co_finished(coroutine_t *c);
void del_co(coroutine_t *c);

