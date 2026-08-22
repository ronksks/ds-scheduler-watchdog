#!/bin/bash
# setup-repo.sh - Copy scheduler/watchdog files from ds-temp and push to ds-scheduler-watchdog

set -e

# Paths
DS_TEMP="$HOME/ron-ylensky-ds/ds"
REPO_DIR="$HOME/ds-scheduler-watchdog"

echo "=== Setting up ds-scheduler-watchdog repo ==="

# Create repo directory if it doesn't exist
if [ ! -d "$REPO_DIR" ]; then
    mkdir -p "$REPO_DIR"
    cd "$REPO_DIR"
    git init
    git remote add origin git@github.com:ronksks/ds-scheduler-watchdog.git
else
    echo "Repo directory already exists at $REPO_DIR"
fi

cd "$REPO_DIR"

# Create directory structure
mkdir -p include src tests examples docs

echo "Copying headers..."
# Headers
cp "$DS_TEMP/include/scheduler.h" include/
cp "$DS_TEMP/include/watchdog.h" include/
cp "$DS_TEMP/include/watchdog_utils.h" include/
cp "$DS_TEMP/include/task.h" include/
cp "$DS_TEMP/include/pqueue.h" include/
cp "$DS_TEMP/include/heap.h" include/
cp "$DS_TEMP/include/dlist.h" include/
cp "$DS_TEMP/include/slist.h" include/
cp "$DS_TEMP/include/uid.h" include/
cp "$DS_TEMP/include/cbuffer.h" include/

echo "Copying sources..."
# Sources
cp "$DS_TEMP/src/scheduler.c" src/
cp "$DS_TEMP/src/watchdog.c" src/
cp "$DS_TEMP/src/watchdog_utils.c" src/
cp "$DS_TEMP/src/task.c" src/
cp "$DS_TEMP/src/pqueue.c" src/
cp "$DS_TEMP/src/heap.c" src/
cp "$DS_TEMP/src/dlist.c" src/
cp "$DS_TEMP/src/slist.c" src/
cp "$DS_TEMP/src/uid.c" src/
cp "$DS_TEMP/src/cbuffer.c" src/

echo "Copying tests..."
# Tests
cp "$DS_TEMP/test/test_scheduler.c" tests/
cp "$DS_TEMP/test/test_watchdog.c" tests/
cp "$DS_TEMP/test/test_watchdog_utils.c" tests/
cp "$DS_TEMP/test/test_task.c" tests/
cp "$DS_TEMP/test/test_pqueue.c" tests/
cp "$DS_TEMP/test/test_heap.c" tests/
cp "$DS_TEMP/test/test_dlist.c" tests/
cp "$DS_TEMP/test/test_slist.c" tests/
cp "$DS_TEMP/test/test_uid.c" tests/
cp "$DS_TEMP/test/test_cbuffer.c" tests/

echo "Copying example..."
# Example app
cp "$DS_TEMP/src/watchdog_app.c" examples/

echo "Staging all files..."
git add .

echo "Committing..."
git commit -m "Add scheduler, watchdog, and dependency modules with tests"

echo "Pushing to GitHub..."
git branch -M main 2>/dev/null || true
git push -u origin main

echo "=== Done! Repo is ready at https://github.com/ronksks/ds-scheduler-watchdog ==="
