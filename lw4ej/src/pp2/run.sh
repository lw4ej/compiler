#! /bin/bash
filelist=`ls ./sample`
dfile="./sample/diff.txt"
echo "">$dfile
for filename in $filelist
	do
		if [ "${filename##*.}" == "decaf" ];then
			./dcc < ./sample/$filename >& ./sample/${filename%.*}.txt
		fi
		
	done

for filename in $filelist
	do
		if [ "${filename##*.}" == "out" ];then
			diff ./sample/$filename ./sample/${filename%.*}.txt >> $dfile
		fi
	done
