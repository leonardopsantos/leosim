
rm -f linked_stats.txt

for sz in 144 233 377 610 987 1597 2584 4181 6765; do
    ./build/leosim $sz >> linked_stats.txt;
done
