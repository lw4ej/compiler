#! /bin/bash
filelist=`ls ./samples`
dfile="./samples/diff.txt"
echo "">$dfile
for filename in $filelist
	do
		if [ "${filename##*.}" == "decaf" ];then
			./dcc < ./samples/$filename >& ./samples/${filename%.*}.txt
		fi
		
	done

for filename in $filelist
	do
		if [ "${filename##*.}" == "out" ];then
			diff ./samples/$filename ./samples/${filename%.*}.txt >> $dfile
		fi
	done
