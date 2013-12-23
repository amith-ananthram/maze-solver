##############################################################
#
#	program: AMStartup.sh
#
#	@Amith Ananthram
#	@Matthew Knight
#
##############################################################

#### Input verification
# First check: appropriate number of fields provided
if [ $# -le 0 ]
then
	echo $0: No options specified!
	echo $0: Executing default configuration...
elif [ $# -ge 7 ]
then
	# Scream and quit if too many flags
	echo -e $0: Usage: Allowed flags are:
	echo -e \t-n Number of Avatars	\n
	echo -e \t-d Difficulty			\n
	echo -e \t-h Hostname			\n
	echo -e $0: Exiting...
	exit 0
fi

# Variables for argument values
n_av=0
diff=0
host=0

# Switches for triggering defaults
n_sw=0
d_sw=0
h_sw=0

#### Getopts to sort out flags
while getopts "n:d:h:" sw
do
	case "$sw" in
		n)	n_sw=1
			n_av="$OPTARG"
			;;
		d)	d_sw=1
			diff="$OPTARG"
			;;
		h)	h_sw=1
			host="$OPTARG"
			;;
	esac
done

#### Apply defaults if switch is not triggered
# Number of Avatars
if [ "$n_sw" -eq 0 ]
then						# Switch is not triggered, enable default
	n_av=4					# 4 avatars
	echo $0: DEFAULT Number of avatars set to $n_av !
else						# Switch triggered, check input value
	if [ "$n_av" -le 0 ]
	then
		echo $0: USAGE Number of avatars $n_av must be between 1 and 10.
		exit 1
	elif [ "$n_av" -gt 10 ]
	then
		echo $0: USAGE Number of avatars $n_av must be between 1 and 10.
		exit 1
	fi
	echo $0: Number of avatars set to $n_av !
fi

# Difficulty
if [ "$d_sw" -eq 0 ]
then						# Switch is not triggered, enable default
	diff=4					# Difficulty 4
	echo $0: DEFAULT Difficulty set to $diff !
else						# Switch triggered, check input value
	if [ "$diff" -lt 0 ]
	then
		echo $0: USAGE Difficulty $diff must be between 0 and 9.
		exit 1
	elif [ "$diff" -gt 9 ]
	then
		echo $0: USAGE Difficulty $diff must be between 0 and 9.
		exit 1
	fi
	echo $0: Difficulty set to $diff !
fi

# Hostname
if [ "$h_sw" -eq 0 ]		# Switch is not triggered, enable default
then						# Default host = icecream.cs.dartmouth.edu
	host="icecream.cs.dartmouth.edu"
	echo $0: DEFAULT Host address set to $host !
else						# Switch triggered, check input value
	ping -c1 $host
	if [ "$?" -ne 0 ]
	then
		echo $0: Error: unable to connect to host $host !
		exit 1
	fi
	echo $0: Host address set to $host !
fi

# Initialize log file name
filename=Amazing_"$USER"_"$n_av"_"$diff"

# Get IP address from hostname
# TODO error check for bad hostname

ipaddr=`host $host | head -1 | awk '{print $4}'`

# AMStartup2 C program to query the server, get MPort and maze dimensions
AMStartup2 "$n_av" "$diff" "$ipaddr" "$filename"

mport=`cat $filename | awk '{print $1}'`
mwidth=`cat $filename | awk '{print $2}'`
mheight=`cat $filename | awk '{print $3}'`

# Rewrite first line of logfile according to spec
echo $USER $mport `date` > $filename

# Start up avatar clients
for (( i=0; i<$n_av; i++ ))
do
	amazing $i $n_av $diff $ipaddr $mport $mwidth $mheight $filename &
#	echo $mport $mwidth $mheight
done

echo $0: AMAZING is running...
wait
echo $0: Done!

exit 0
