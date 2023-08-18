#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <chrono>

using namespace std;

const int N = 10; // # of diners the restaurant can hold at a time

mutex m; 
condition_variable cv_front, cv_back; // condition variables for front door and back door

bool front = false; // bool variable to indicate whether front door is open
bool back = false; // bool variable to indicate whether back door is open
int diners_inside = 0; // # of diners inside the restaurant currently
int diners_served = 0; // # of diners served so far

auto finish = std::chrono::system_clock::now() + std::chrono::seconds{10};
auto main_finish = std::chrono::system_clock::now() + std::chrono::seconds{15};

void diner_thread(int id)
{
    while(std::chrono::system_clock::now() < finish)
    {
        unique_lock<mutex> lock_var(m);
    
        // wait for front door to open
        while (!front) 
        {
            cv_front.wait(lock_var);
        }
        
        // enter the restaurant
        diners_inside++;
        cout << "Diner " << id << " entered the restaurant.\n";
        
        // wait for all diners to enter before service starts
        if (diners_inside == N) 
        {
            front = false;
            cv_front.notify_all();
            sleep(3);
            back = true;
            cv_back.notify_all();
            cout << "All diners inside. Service starting.\n";
            
        }
        
        // wait for service to finish
        else 
        {
            cv_back.wait(lock_var);
            
        }
        
        // exit the restaurant
        diners_inside--;
        diners_served++;
        cout << "Diner " << id << " exited the restaurant.\n";
        
        // wait for all diners to exit before next batch is admitted
        if (diners_inside == 0) {
            back = true;
            cv_back.notify_all();
            cout << "All diners served. Next batch can enter.\n";
            diners_served = 0;
        }
    }
    
}

int main()
{
    // create threads for diners
    thread diners[N*2];
    for (int i = 0; i < N*2; i++) 
    {
        diners[i] = thread(diner_thread, i);
    }
    
    // start admitting diners in batches
    while (std::chrono::system_clock::now() < main_finish) 
    {
        unique_lock<mutex> lock_var(m);
        
        // open front door for next batch
        front = true;
        cv_front.notify_all();
        cout << "Restaurant is open. Waiting for diners to enter...\n";
        
        // wait for all diners to enter before closing front door
        if (diners_inside < N) {
            cv_front.wait(lock_var);
        }
        
        // wait for service to finish before opening back door
        if (!back) {
            cv_back.wait(lock_var);
        }
        
        // close back door after all diners have exited
        if (diners_inside > 0) {
            cv_back.wait(lock_var);
        }
    }
    for (int i = 0; i < N*2; i++) 
    {
        diners[i].join();
    }
    
    return 0;
}