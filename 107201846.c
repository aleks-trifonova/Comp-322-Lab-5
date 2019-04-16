#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "pid.h"

#define NUM_THREADS    100
#define ITERATIONS    3
#define SLEEP        5
#define PID_MIN    300
#define PID_MAX    350
#define PID_RANGE    50

pthread_mutex_t test_mutex;

int failure = 0;

// mutex lock for accessing pid_map
pthread_mutex_t mutex;
int map[PID_RANGE];

int allocate_map(void);

int allocate_pid(void);

void release_pid(int pid);

/**
 * @brief Allocates the pid map.
 * Goes through the array and assigns an index to the values in the array 
 * Then once the thread is called it will go into the array and lock and unlock the mutex's as it seems necessary. 
 */
int allocate_map(void) {
    pthread_mutex_lock(&test_mutex);
    int i;
    for (i = 0; i < PID_RANGE; i++) {
        map[i] = 0;
    }
    pthread_mutex_unlock(&test_mutex);
    return 1;
}

/**
 * @brief Allocates a pid
 * We lock the mutex, then we loop through the array and check if the pid value is true if its true then it is being used and we move on and check the 
 * next one if it is false then we assign that pid to that thread. 
 * Then we unlock the mutex.
 * 
 */
int allocate_pid(void) {
    pthread_mutex_lock(&test_mutex);
    int i;
    for (i = 0; i < PID_RANGE; i++) {
        if (map[i] == 0) {
            map[i] = 1;
            pthread_mutex_unlock(&test_mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&test_mutex);
    return -1;
}

/**
 * @brief Releases a pid
 * A thread locks the mutex and then it goes directly to its assigned pid to 
 * release it which it does then the thread unlocks the mutex as necessary when it is finished. 
 * @param pid 
 */
void release_pid(int pid) {
    pthread_mutex_lock(&test_mutex);
    map[pid] = 0;
    pthread_mutex_unlock(&test_mutex);
}

/**
 * @brief Allows threads to come into the array and loop through and check for available pids to be assigned. 
 * @param param
 * @return
 */
void *allocator(void *param) {

    for (int i = 0; i < ITERATIONS; i++) {

        int pid = allocate_pid();

        if (pid == -1) {
            printf("No pid available\n");
            failure++;
            sleep((int) (rand() % SLEEP));
        } else {
            printf("PID %d allocated\n", PID_MIN + pid);
            sleep((int) (rand() % SLEEP));
            release_pid(pid);

            printf("PID %d released\n", PID_MIN + pid);
        }
    }
}

int main(void) {
    pthread_t tids[NUM_THREADS];

    // Initialize mutex lock
    pthread_mutex_init(&mutex, NULL);

    /* allocate the pid map */
    if (allocate_map() == -1) {
        return -1;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&tids[i], NULL, &allocator, NULL);
    }
    for (int j = 0; j < NUM_THREADS; j++) {
        pthread_join(tids[j], NULL);
    }

    printf("%d  processes can not obtain a PID\n", failure);
    printf("***DONE***\n");

    return 0;
}






