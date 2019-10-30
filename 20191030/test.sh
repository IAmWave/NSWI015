#!/bin/bash
n_failed=0

function test_file {
    file=$1
    expected_result=$2
    ./build/palindrome $file
    if [ $? -eq $expected_result ]; then
        echo "$file: OK"
    else
        echo "$file: Failed"
        n_failed=$((n_failed+1))
    fi
}

for f in data/y_*.in; do
    test_file $f 0
done

for f in data/n_*.in; do
    test_file $f 1
done

test_file "data/nonexistent" 1

echo "Done, failed tests: $n_failed"

if [ $n_failed -gt 0 ]; then
    exit 1
else
    exit 0
fi
