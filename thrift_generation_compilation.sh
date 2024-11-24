#!/bin/bash

echo "Generating C++ code from pubsub.thrift..."
  thrift --gen cpp pubsub.thrift

  if [ $? -ne 0 ]; then
    echo "Error: Thrift code generation failed!"
    exit 1
  fi

  echo "Running make to compile the generated C++ code..."
  make

# Function to start a single broker in the background
start_broker() {
  local PORT=$1

  # Validate if the input is a valid number
  if ! [[ "$PORT" =~ ^[0-9]+$ ]]; then
    echo "Error: Port number must be a valid integer."
    exit 1
  fi

  if [ $? -ne 0 ]; then
    echo "Error: Compilation failed!"
    exit 1
  fi

  echo "Starting the broker program on port $PORT..."
  ./broker "$PORT" &  # Run broker in the background
  # Capture the PID of the broker process
  BROKER_PID=$!
  
  # Store the PID to wait for all brokers to finish later
  BROKER_PIDS+=($BROKER_PID)

  if [ $? -ne 0 ]; then
    echo "Error: Failed to start broker on port $PORT!"
    exit 1
  fi
}

read -p "How many brokers would you like to run? " NUM_BROKERS

# Validate if the input is a valid number
if ! [[ "$NUM_BROKERS" =~ ^[0-9]+$ ]]; then
  echo "Error: The number of brokers must be a valid integer."
  exit 1
fi

# Initialize an array to hold broker PIDs
BROKER_PIDS=()

for ((i=1; i<=NUM_BROKERS; i++))
do
  echo "---------------------------------"
  echo "Running broker #$i"
  read -p "Enter port number for broker #$i: " PORT
  start_broker "$PORT"
done

# Wait for all background broker processes to finish
echo "All brokers have been started. Waiting for them to finish..."
wait "${BROKER_PIDS[@]}"  # Wait for all background processes (brokers) to finish

echo "All brokers have stopped."
exit 0
