# Bryan Orr & Alex Dunn, 2012 APS, ANL
# 	Iterates through current directory and calls
#  	the dat2csv.exe converter then archives newly
#	created files.

use File::Copy;


# Change this to 0 to skip compression
$compress = 1;


opendir currentDIR, ".";

# Make sure we have directory setup
system("mkdir", "Orig");

while( $infile = readdir(currentDIR)) {
  if($infile =~ m/.DAT/) {
  
	# Create output file name
    $outfile = $infile;
	$outfile =~ s/.DAT/.csv/;
	
	# Run the converter
	print "Converting: $infile\n";
	system("dat2csv.exe", "$infile", "$outfile");
	
	# Move the original data to sub folder
	print "Moving: $infile to Orig directory\n";
    move("./$infile" , "./Orig/$infile");

	# Compress the csv into archive to save disk
	if(($outfile =~ m/_(\d.*\d)_/) && ($compress == 1)) {
		$zipfile = $1 . ".zip";
		print "Trying Archive: $zipfile\n";
		if($zipfile) {
			system("7z", "a", "-tzip", "$zipfile", "$outfile");
			system("del", "$outfile");
		} else {
			print "Error: Cannot create archive.";
			print "\tLeaving $outfile as is.";
		}
	} else {
		print "Skipping Compression";
	}
		
	# complete
	print "Iteration Complete\n";
  }
}

closedir currentDir;
