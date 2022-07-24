#!/bin/env bash

# Author : Liming Kuang
# Date   : 2022-07-24

# execution path check
chmod u+x ./e2etest.sh
OUT_MSG=$(cat e2e_in.txt 2>&1)
if [[ "$OUT_MSG" =~ "No such file or directory" ]]; then
    echo "To run the script properly, please first use \"cd\" command to navigate to "
    echo "the directory where the script is located. "
    echo "Normally it's under: /team109/Implementierung/test/"
    exit 1
fi

echo "Welcome to the E2E Test Script of Project Team 109 -- XTEA En/Decryption"
echo "--------------------------------------------------------------------------------"
echo "Input file: e2e_in.txt"
echo "Content:"
cat e2e_in.txt
echo ""
echo "--------------------------------------------------------------------------------"
rm -f e2e_in_* e2e_out.txt
PASS_NUM=0
TOTAL_NUM=5
pattern="EVERYTHING OK"
CMD=""

echo "Test 1: The most simple encrypt command possible:" 
CMD="../main e2e_in.txt"
echo "Command: $CMD"
OUT_MSG=$(eval $CMD)
echo "$OUT_MSG"
OUT_CONT=$(cat e2e_in_encrypted.txt)
if [[ "$OUT_MSG" =~ $pattern ]] && [[ "$OUT_CONT" =~ "854E1AB5B99A1F2846" ]]
then
    PASS_NUM=$((PASS_NUM+1))
    echo "Test 1 PASS"
else
    echo "Test 1 FAIL"
fi
echo ""
echo "Test 2: The most simple decrypt command possible:" 
CMD="../main e2e_in_encrypted.txt -d"
echo "Command: $CMD"
OUT_MSG=$(eval $CMD)
echo "$OUT_MSG"
OUT_CONT=$(cat e2e_in_encrypted_decrypted.txt)
if [[ "$OUT_MSG" =~ $pattern ]] && [[ "$OUT_CONT" == $(cat e2e_in.txt) ]]
then
    PASS_NUM=$((PASS_NUM+1))
    echo "Test 2 PASS"
else
    echo "Test 2 FAIL"
fi
echo ""

echo "Test 3: The most comprehensive encrypt command with benchmark possible:" 
CMD="../main -V1 -B20 e2e_in.txt -k 100,50,9,7 -i 12,15 -o e2e_out.txt"
echo "Command: $CMD"
OUT_MSG=$(eval $CMD)
echo "$OUT_MSG"
OUT_CONT=$(cat e2e_out.txt)
if [[ "$OUT_MSG" =~ $pattern ]] && [[ "$OUT_CONT" =~ "6D091E709AADEBFF63972A19" ]]
then
    PASS_NUM=$((PASS_NUM+1))
    echo "Test 3 PASS"
else
    echo "Test 3 FAIL"
fi
echo ""


echo "Test 4: The most comprehensive decrypt command with benchmark possible:"
CMD="../main -V1 -B20 e2e_out.txt -k 100,50,9,7 -i 12,15 -o e2e_out.txt -d"
echo "Command: $CMD"
OUT_MSG=$(eval $CMD)
echo "$OUT_MSG"
OUT_CONT=$(cat e2e_out.txt)
if [[ "$OUT_MSG" =~ $pattern ]] && [[ "$OUT_CONT" == $(cat e2e_in.txt) ]]
then
    PASS_NUM=$((PASS_NUM+1))
    echo "Test 4 PASS"
else
    echo "Test 4 FAIL"
fi
echo ""

TOTAL_ERROR_COUNT=11
ERROR_COUNT=0
echo "Test 5: Error handling for invalid inputs:"
echo "(FYI: In this section we will test $TOTAL_ERROR_COUNT invalid inputs of different types. To make things neat and clear, we won't show the exact command executed, but you can read the shell script to find out what they exactly are.)"
echo ""

OUT_MSG=$(../main -V0 -k 100,50,9,7 -i 12,15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "No input" ]] && echo "No input file: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "No input file: FAIL"
OUT_MSG=$(../main e2e_in.txt e2e_in.txt -k 100,50,9,7 -i 12,15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "multiple" ]] && echo "Multiple input file: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Multiple input file: FAIL"

OUT_MSG=$(../main -V0 -k 100,50,9,7 e2e_in.txt -i 12,15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "sequence" ]] && echo "Wrong args sequence: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Wrong args sequence: FAIL"

OUT_MSG=$(../main -V0 e2e_in.txt -k a,b,9,7 -i 12,15  2>&1)
[[ "$OUT_MSG" =~ "integer" ]] && echo "Non-integer key: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Non-integer key: FAIL"
OUT_MSG=$(../main -V0 e2e_in.txt -k 50,9,7  -i 12,15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "comma" ]] && echo "Key format wrong/not enough integers: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Key format wrong/not enough integers: FAIL"

OUT_MSG=$(../main -V0 e2e_in.txt -k 100,50,9,7 -i x,15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "integer" ]] && echo "Non-integer iv: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Non-integer iv: FAIL"
OUT_MSG=$(../main -V0 e2e_in.txt -k 100,50,9,7 -i 15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "comma" ]] && echo "Iv format wrong/not enough integers: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Iv format wrong/not enough integers: FAIL"

OUT_MSG=$(../main -V100  e2e_in.txt -k 100,50,9,7 -i 12,15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "imp" ]] && echo "Invalid implementation number: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Invalid implementation number: FAIL"
OUT_MSG=$(../main -Bx -k 100,50,9,7 e2e_in.txt -i 12,15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "benchmark" ]] && echo "Invalid benchmark number: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Invalid benchmark number: FAIL"

OUT_MSG=$(../main -V0 e2e_in.txt -x 12,15 -o e2e_out.txt 2>&1)
[[ "$OUT_MSG" =~ "Unknown" ]] && echo "Unknown argument: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Unknown argument: FAIL"

OUT_MSG=$(../main e2e_in_encrypted.txt -k 1,2,3,44 -d 2>&1)
[[ "$OUT_MSG" =~ "Possible false decryption key or iv." ]] && echo "Possible wrong decryption key: PASS" && ERROR_COUNT=$((ERROR_COUNT+1)) || echo "Possible wrong decryption key: FAIL"

echo ""
echo "Total error handled: $ERROR_COUNT / $TOTAL_ERROR_COUNT"
if [ $ERROR_COUNT -eq $TOTAL_ERROR_COUNT ]
then
    PASS_NUM=$((PASS_NUM+1))
    echo "Test 5 PASS"
else
    echo "Test 5 FAIL"
fi
echo "--------------------------------------------------------------------------------"
echo "Total PASS: $PASS_NUM / $TOTAL_NUM"
echo "--------------------------------------------------------------------------------"
echo "Clean up en-/decrypted files generated during the test? (Y/y for yes, any other key to exit.)"
read -n 1 -s -r cln
if [ "$cln" = "Y" ] || [ "$cln" = "y" ] 
then
    echo "Cleaning up..."
    rm -f e2e_in_* e2e_out.txt
    echo "Clean up complete."
fi