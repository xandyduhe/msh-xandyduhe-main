#! /usr/bin/env bash

echo "Running msh Tests" 
echo "-------------------------" 
for file in *.in; do
     ARGS=$(cat ${file%.in}.args)
     diff -w <(../../bin/msh $ARGS < ${file} | sed  -e '$a\') ${file%.in}.ans &> /dev/null
     if [[ "$?" -eq 0 ]]; then 
          printf "Test (%s) passed\n" $file 
     else 
          printf "Test (#%s) failed\n-----------\n" $file 
          diff -w <(../../bin/msh $ARGS < ${file} | sed  -e '$a\') ${file%.in}.ans
          echo "-----------"
     fi 
done 