# Network Communication and File System Optimization #

This project consists of two mini projects related to file systems and Encrypted Network Communication.

## Secure Message Exchange ##

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
