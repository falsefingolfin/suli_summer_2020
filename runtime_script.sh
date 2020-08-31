#!/usr/bin/bash

count=4

rm runtimes.txt
touch runtimes.txt

echo "\"Processes\",\"Trial 1\",\"Trial 2\",\"Trial 3\"" >> runtimes.txt

while [[ $count -le $1 ]]; do

    echo -n "$count" >> runtimes.txt

    for i in 1 2 3; do
        start=$(date +%s.%3N)

        cmd="mpirun --oversubscribe -np $count nsga2r"
        $cmd

        end=$(date +%s.%3N)
        runtime=$(echo "$end - $start" | bc)

        echo -n ",$runtime" >> runtimes.txt
        mv "best_pop.out" "best_pop_$(count)_$(i).out"
    done

    echo "" >> runtimes.txt
    let "count *= 2"
done