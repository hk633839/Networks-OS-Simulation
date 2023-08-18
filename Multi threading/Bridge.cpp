#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <bits/stdc++.h>

using namespace std;

// Total number of threads
#define THREADS 20 


// Mutex lock init
pthread_mutex_t bridge_lock = PTHREAD_MUTEX_INITIALIZER;

void *north_people(void* n)
{
    int sleep_time = rand() % 5;
    int thread_no = pthread_self();

    // Locking the bridge
    pthread_mutex_lock(&bridge_lock);
	printf("North traveller: %d is travelling on the bridge\n", thread_no);

    // Sleepiing for a random amount of time
   	sleep(sleep_time);
	printf("North traveller: %d completed travelling\n", thread_no);

    // Unlocking the bridge
	pthread_mutex_unlock(&bridge_lock);

    // Exiting the thread
	pthread_exit(NULL); 
}

void *south_people(void* n)
{
    int sleep_time = rand() % 5;
    int thread_no = pthread_self();

    // Locking the bridge
    pthread_mutex_lock(&bridge_lock);
	printf("South traveller: %d is travelling on the bridge\n", thread_no);

    // Sleepiing for a random amount of time
   	sleep(sleep_time);
	printf("South traveller: %d completed travelling\n", thread_no);

    // Unlocking the bridge
	pthread_mutex_unlock(&bridge_lock);

    // Exiting the thread
	pthread_exit(NULL); 
}


int main()
{
    srand(time(NULL));

    // Initializing the thread IDs for north and south people
    pthread_t north_threads[THREADS];
    pthread_t south_threads[THREADS];

    // Temporary variables for error checking
    int temp1, temp2;

    // Mutex Initialize
    pthread_mutex_init(&bridge_lock, NULL);

    // Creating the threads
    for(int i=0; i<THREADS; i++)
    {
        temp1 = pthread_create(&north_threads[i], NULL, north_people, NULL);
        temp2 = pthread_create(&south_threads[i], NULL, south_people, NULL);

        // Checking whether the creation is successful or not
        if(temp1 != 0 || temp2 != 0)
        {
            cout << "The thread creation is unsuccessful" << endl;
            return 1;
        }
    }

    // Waiting for the threads to join
    for(int i=0; i<THREADS; i++)
    {
        temp1 = pthread_join(north_threads[i], NULL);
        temp2 = pthread_join(south_threads[i], NULL);

        // Checking whether the joining is successful or not
        if(temp1 != 0 || temp2 != 0)
        {
            cout << "The thread joining is unsuccessful" << endl;
            return 1;
        }
    }

    // Mutex destroying
    pthread_mutex_destroy(&bridge_lock);
    return 0;
}