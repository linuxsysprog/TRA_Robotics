# TRA_Robotics: Job interview assignment
In April 2018 I was interviewed by [TRA Robotics](https://www.tra.ai/) and was given this short assignment to test my C++ skills.
I approached it with Boost.Asio, a very compact way to write communication software in C++.
Here's the assignment:
```
/*
 * Job interview assignment.
 * Employer: TRA Robotics    Candidate: Andrey Chislenko
 * Date:     April 2018
 * Problem statement: Write a TCP/IP client-server demo; include a test.
 * 1) Server accepts connections from Clients.
 * 2) A Client sends a message to Server containing a positive
 * integer number (a wait interval in secs) and a block of text.
 * 3) Server:
 *     a) logs text
 *     b) goes into sleep for the specified number of seconds
 *     c) forms and sends a message to Client containing:
 *         1) original text
 *         2) reversed text, by char
 *         3) reversed text, by word
 *         4) char frequency statistics
 * Environment: CentOS 6.8 x86, GCC 4.4.7, Boost 1.41
 * This is Server.
 * Build: make clean; make
 * Run: ./server
 **/
```
