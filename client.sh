#!/bin/bash 
./client '10 long response' &
./client '1 quick response' &
./client 'bad message' &
./client '5 medium response' &
