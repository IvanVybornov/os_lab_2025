#1/bin/bash
len=$#
sum=0
for (( number = 1; number <= $len; number++ ))
do
sum=$[ $sum + ${!number} ]
done
echo $len
echo $[$sum / $len]