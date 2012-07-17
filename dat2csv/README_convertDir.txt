To convert sample data into csv:

REQS: convertDir.pl - you must have Perl & 7-zip installed and added to your windows PATH.
      dat2csv.exe - you must have visual c++ installed (Visual C++ express or visual c++ redistributable).

create a directory and place the .DAT files you want to convert into there.
Also place convertDir.pl & dat2csv.exe in the newly created directory.

All the <NAME>.DAT files in the directory will be converted to <NAME>.csv and zipped into
a daily archive.  To bypass archiving, open convertDir.pl and set $compress = 0.  The 
original .DAT files are moved into a directory named Orig to prevent double conversion 
on the same data.

enter directory and run (double-click) on convertDir.pl

Once conversion is complete, you can delete convertDir.pl and dat2csv.exe.
