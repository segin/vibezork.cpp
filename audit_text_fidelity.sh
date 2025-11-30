#!/bin/bash
# Text Fidelity Audit Script for Zork I C++ Port
# This script helps identify text discrepancies between the C++ port and original ZIL

echo "=== Zork I Text Fidelity Audit ==="
echo ""
echo "This audit compares text output in the C++ port to the original ZIL source."
echo ""

# Check for room descriptions
echo "## Room Descriptions Audit"
echo ""
echo "Checking West of House..."
grep -n "You are standing in an open field west of a white house" src/world/world.cpp
grep -n "West of House" zil/1dungeon.zil

echo ""
echo "Checking North of House..."
grep -n "You are facing the north side of a white house" src/world/world.cpp
grep -n "North of House" zil/1dungeon.zil

echo ""
echo "Checking South of House..."
grep -n "You are facing the south side of a white house" src/world/world.cpp
grep -n "South of House" zil/1dungeon.zil

echo ""
echo "## Object Descriptions Audit"
echo ""
echo "Checking mailbox..."
grep -n "securely anchored" src/world/world.cpp
grep -n "securely anchored" zil/1dungeon.zil

echo ""
echo "## Verb Messages Audit"
echo ""
echo "Checking TAKE messages..."
grep -rn "You can't take that" src/verbs/
grep -n "can't take" zil/gverbs.zil

echo ""
echo "## Error Messages Audit"
echo ""
echo "Checking parser errors..."
grep -rn "I don't know the word" src/parser/
grep -n "don't know" zil/gparser.zil

echo ""
echo "=== Audit Complete ==="
echo "Review the output above for any discrepancies in:"
echo "- Punctuation (periods, commas, quotes)"
echo "- Capitalization"
echo "- Line breaks and spacing"
echo "- Exact wording"
