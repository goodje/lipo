#! /bin/env bash

while [[ 1 ]]
do
clear
lsof -p `pidof ./mortise`

sleep 5

done