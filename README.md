# Networks-OS-mini-projects

This repository encompasses a collection of five distinct mini projects, each of which encompasses several additional smaller projects within itself.


# Base64 encoding #

This project involves the complete establishment of communication between multiple clients and a server, ensuring the security of messages exchanged using base64 encoding.

To run the project:

1.first execute the server program

    i.input server ip
    
    ii.input server port number
    
2.now execute the client program

    i.input server ip
    
    ii.input server port number
    
3.now client asks for type and content of the message

4.give type of message in first line then message in the new line

5.press enter

6.now check the server's terminal it will display the encoded message and decoded message


# CBR_UDP vs FTP_TCP #

In this project, comparision of CBR traffic over UDP agent and FTP traffic over TCP agent is implemented.

TCP agents considered for FTP traffic are:

~~~
1. TCP HighSpeed

2. TCP Vegas
         
3. TCP Scalable
~~~
Dumbell topology is considered for this project.


# File systems #

This project consists of two mini projects related to file systems.

### File allocation ###

This compares the performance of four file allocation strategies based on their space utilization and file access times.

~~~
1. Contigous allocation

2. Linked allocation

3. Indexed allocation

4. Modified contigous allocation
~~~

### Demand paging ###

Implemented a demand paging system that loads pages from disk to memory on demand and unloads them when they are no longer needed.

Optimized the memory usage and performance of the system by tuning the demand paging parameteres such as the page size, swap space size, and page replacement algorithm.

The page replacement algorithms used are:

~~~
-> Least Recently Used

-> First In First Out

-> Optimal replacement
~~~


# Inter Process Communication #

This project contains three mini projects related to Inter Process Communication

### Directory Merging ###

There are two directories available, and the goal is to move the combined set of files from both directories into each of the directories.

This has been achieved using pipes.

### Online Exam System ###

The aim is to administer an online examination for students using communication through child processes. In this setup, students utilize child processes, while the parent process serves as the coordinator.

The child processes are responsible for responding to questions, and the parent process manages the collection and evaluation of these answers. Subsequently, individual grades are presented to each student.

### Event Registration ###

The objective is to build the multi-threaded event registration system for a convention centre which organizes e events concurrently and has a capacity of c.

Queries involved in this are:

~~~
1. Inquire the number of seats available in 'a' events

2. Book k tickets in 'a' events

3. Cancel a booked ticket
~~~


# Multi threading #

This project contains three mini projects which aims to solve some real world problems with the help of threads and some lock variables or semaphores.

### Restaurant Problem ###

A restaurant has a capacity of N. The constraints are as follows:

~~~
• A diner cannot enter until the restaurant has opened its front door to let people in.

• The restaurant cannot start service until N diners have come in.

• The diners cannot exit until the back door is open.

• The restaurant cannot close the backdoor and prepare for the next batch until all the diners of the previous batch have left.
~~~

### Bridge Problem ###

A bridge connects north and south bound people of a village. If both side people enter the bridge on the same time, then deadlock occurs. 

To prevent the deadlock, concurrency is implemented using threads and mutex locks or semaphores.

Two cases are considered in this:

~~~
➢ Case 1 : Consider the scenario in which northbound people prevent southbound
           people from using the bridge, or vice versa.

➢ Case 2 : Consider the scenario in which northbound people don't prevent
           southbound people from using the bridge, or vice versa.
~~~

### E-Commerce App ###

The e-commerce platform handles millions of transactions per day. Many servers are running parallely on the system. 
Each server is responsible for a specific type of operations. Each server contains several worker threads.

Each worker thread has a priority level assigned to it, and a certain amount of resourcesassigned to it. 
The priority level determines the order in which the threads are scheduled to process requests.
The resources assigned to each thread determine the maximum number of requests it can handle simultaneously.

The objective is to develop a scheduling algorithm that handles the transaction load.
