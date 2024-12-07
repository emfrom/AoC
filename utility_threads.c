#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

//#include "xmalloc.h"

typedef struct {
    void *(*operation)(void *);
    void *(*get_input)(void *);
    void *(*collate)(void *);
    void *user_data;
    pthread_mutex_t *get_input_mutex;
    pthread_mutex_t *collate_mutex;
} thread_args_t;

void *thread_worker(void *arg)
{
    thread_args_t *args = (thread_args_t *) arg;


    for (;;) {
        void *input;
        pthread_mutex_lock(args->get_input_mutex);
        input = args->get_input(args->user_data);
        pthread_mutex_unlock(args->get_input_mutex);

        if (NULL == input)
            return NULL;

        //Get it done
        void *result = args->operation(input);
        if (NULL == result)
            return NULL;

        if (args->collate) {
            pthread_mutex_lock(args->collate_mutex);
            args->collate(result);
            pthread_mutex_unlock(args->collate_mutex);
        }
    }

    return NULL;
}


// Runs a bunch of threads
// get_input is called on user_data to get input for the operation(both mutex
// protected)
// operation() is called on the result of get_input
// If non-NULL, collate is called on the result of operation()
//  , protected by a different mutex
void threadrun(int n_threads,
               void *(*operation)(void *),
               void * (*collate)(void *),
               void * (*get_input)(void *), void *user_data)
{
    pthread_t *threads = xmalloc(sizeof(pthread_t) * n_threads);
    thread_args_t args;
    pthread_mutex_t get_input_mutex, collate_mutex;

    args.operation = operation;
    args.get_input = get_input;
    args.user_data = user_data;
    args.collate = collate;
    args.get_input_mutex = &get_input_mutex;
    args.collate_mutex = &collate_mutex;


    pthread_mutex_init(&get_input_mutex, NULL);
    pthread_mutex_init(&collate_mutex, NULL);


    for (int i = 0; i < n_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_worker, &args) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }


    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&get_input_mutex);
    pthread_mutex_destroy(&collate_mutex);

    xfree(threads);
}



#ifdef THREADRUN_TEST

void *operation(void *arg)
{
    int *num = (int *) arg;
    int *result = malloc(sizeof(int));
    *result = (*num) * 2;       // Example: double the input
    return result;
}

void *collate_function(void *arg)
{
    int *result = (int *) arg;
    printf("Result: %d\n", *result);
    free(result);
    return NULL;
}

void *get_input(void *arg)
{
    arg = arg;

    static int counter = 0;

    if (counter > 10)
        return NULL;

    int *input = malloc(sizeof(int));
    *input = counter++;         // Example: Incremental inputs


    return input;
}

int main()
{
    printf("Should be: 0, 2, 4, .. , 20 (in some order)\n\n");
    threadrun(8, operation, collate_function, get_input, NULL);
    return 0;
}

#endif
