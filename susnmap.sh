#!/bin/bash

# Usage: ./launch_pause_resume.sh [command]

# Check if command is provided
if [ $# -lt 1 ]; then
  echo "Usage: $0 [command]"
  exit 1
fi

# Launch the command in a subshell and pause it immediately
(
  # Get the PID of this subshell
#   echo "Starting process..."
  # BASHPID=$$
  # Send SIGSTOP to this subshell to pause it
  kill -SIGSTOP $BASHPID
  # Execute the command (exec replaces the shell process with the command)
  exec "$@"
) &

# Get the PID of the background job (the subshell)
pid=$!
# echo "Process launched with PID $pid"

# Wait a moment to ensure the process is stopped
sleep 1

# Read the memory map of the paused process
# echo "Reading /proc/$pid/maps..."
# cat /proc/$pid/maps
cat /proc/$pid/maps | grep -i stack
cat /proc/$pid/maps | grep -i heap

# Resume the process
# echo "Resuming process..."
kill -SIGCONT $pid

# Wait for the process to complete
wait $pid
# echo "Process $pid completed."
