
rm -f matrix_stats.txt

for sz in 13 21 34 55 89 144 233 377 610; do
    ./build/leosim $sz >> matrix_stats.txt;
done
