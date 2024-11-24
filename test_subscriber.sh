#!/bin/bash

# Function to start a single subscriber in the background
start_subscriber() {
  local BROKER_ADDRESS=$1
  local PORT=$2
  local TOPIC=$3
  local SUBSCRIBER_ID=$4

  # Validate if port is a valid number
  if ! [[ "$PORT" =~ ^[0-9]+$ ]]; then
    echo "Error: Port number must be a valid integer."
    exit 1
  fi

  echo "Starting subscriber with the following details:"
  echo "Broker Address: $BROKER_ADDRESS"
  echo "Port: $PORT"
  echo "Topic: $TOPIC"
  echo "Subscriber ID: $SUBSCRIBER_ID"

  # Run the subscriber program in the background
  ./subscriber "$BROKER_ADDRESS" "$PORT" "$TOPIC" "$SUBSCRIBER_ID" &

  # Capture the PID of the subscriber process
  SUBSCRIBER_PID=$!
  
  # Store the PID to wait for all subscribers to finish later
  SUBSCRIBER_PIDS+=($SUBSCRIBER_PID)

  if [ $? -ne 0 ]; then
    echo "Error: Failed to start subscriber $SUBSCRIBER_ID!"
    exit 1
  fi
}

# Ask user how many subscribers to start
read -p "How many subscribers would you like to run? " NUM_SUBSCRIBERS

# Validate if the input is a valid number
if ! [[ "$NUM_SUBSCRIBERS" =~ ^[0-9]+$ ]]; then
  echo "Error: The number of subscribers must be a valid integer."
  exit 1
fi

# Initialize an array to hold subscriber PIDs
SUBSCRIBER_PIDS=()

# Loop to run the specified number of subscribers
for ((i=1; i<=NUM_SUBSCRIBERS; i++))
do
  echo "---------------------------------"
  echo "Running subscriber #$i"
  read -p "Enter broker address for subscriber #$i: " BROKER_ADDRESS
  read -p "Enter port number for subscriber #$i: " PORT
  read -p "Enter topic for subscriber #$i: " TOPIC
  read -p "Enter subscriber ID for subscriber #$i: " SUBSCRIBER_ID
  start_subscriber "$BROKER_ADDRESS" "$PORT" "$TOPIC" "$SUBSCRIBER_ID"
done

# Wait for all background subscriber processes to finish
echo "All subscribers have been started. Waiting for them to finish..."
wait "${SUBSCRIBER_PIDS[@]}"  # Wait for all background processes (subscribers) to finish

echo "All subscribers have stopped."
exit 0
