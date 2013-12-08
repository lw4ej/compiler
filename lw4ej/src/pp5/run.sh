#! /bin/bash
filelist=`ls ./$1`
dfile="./diff.txt"
echo "diff_log $1" >> $dfile
for filename in $filelist
	do
		if [ "${filename##*.}" == "decaf" ];then
			echo "./run ./$1/$filename >& ./$1/${filename%.*}.txt"
			./run  ./$1/$filename >& ./$1/${filename%.*}.txt
			echo "$filename finished"
		fi
		
	done

for filename in $filelist
	do
		if [ "${filename##*.}" == "out" ];then
			echo "$filename :" >> $dfile
			diff ./$1/$filename ./$1/${filename%.*}.txt >> $dfile
		fi
	done
