#!/bin/bash

run_publisher() {
  read -p "Enter broker address for this publisher: " BROKER_ADDRESS
  read -p "Enter port number for this publisher: " PORT
  read -p "Enter topic for this publisher: " TOPIC
  read -p "Enter message for this publisher: " MESSAGE

  # Validate if port is a valid number
  if ! [[ "$PORT" =~ ^[0-9]+$ ]]; then
    echo "Error: Port number must be a valid integer."
    exit 1
  fi

  echo "Running publisher with the following details:"
  echo "Broker Address: $BROKER_ADDRESS"
  echo "Port: $PORT"
  echo "Topic: $TOPIC"
  echo "Message: $MESSAGE"

  ./publisher "$BROKER_ADDRESS" "$PORT" "$TOPIC" "$MESSAGE"
  
  if [ $? -ne 0 ]; then
    echo "Error: Publisher failed to start!"
    exit 1
  fi
}

read -p "How many publishers would you like to run? " NUM_PUBLISHERS

if ! [[ "$NUM_PUBLISHERS" =~ ^[0-9]+$ ]]; then
  echo "Error: The number of publishers must be a valid integer."
  exit 1
fi

for ((i=1; i<=NUM_PUBLISHERS; i++))
do
  echo "---------------------------------"
  echo "Running publisher #$i"
  run_publisher
done

echo "All publishers have been started."
exit 0
