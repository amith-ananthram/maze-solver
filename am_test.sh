###################################################################
#
# am_test.sh
# AMAZING Maze Solver Client Test Utility
#
#	Instructions: run am_test with no arguments and go get dinner
#
###################################################################

filename=am_test__`date`.log
echo AMAZING Maze Solver Test on `date` > "$filename"

make clean >> "$filename"
make amazing >> "$filename"
make amstartup2 >> "$filename"

## Functional Tests
MAXAV=10
MAXDF=9
h=icecream.cs.dartmouth.edu

echo Input Testing >> "$filename"
echo NAvatars >> "$filename"
AMStartup.sh -n -1 >> "$filename"
AMStartup.sh -n 11 >> "$filename"
echo Difficulty >> "$filename"
AMStartup.sh -d -1 >> "$filename"
AMStartup.sh -d 11 >> "$filename"
echo Hostname >> "$filename"
AMStartup.sh -h idontexist

echo Functional Testing >> "$filename"

for (( d=0; d<=MAXDF; d++))
do
	for (( n=1; n<=MAXAV; n++))
	do
		echo Maze Case n=$n d=$d >> "$filename"
		AMStartup.sh -n $n -d $d >> "$filename"
		wait
	done
done

echo Testing completed at `date` >> "$filename"
echo Reference amazing client log files for further details. >> "$filename"

echo Testing completed at `date`

exit 0
