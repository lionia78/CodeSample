The main challenge is to implement an algorithm which detects keywords contained as substrings within a given string.

Given two text files (allwords.txt, commonwords.txt ) containing a single word on each line, for each word in the first file find and output all substrings matching words from a second file. These files can be found in the payload.zip.

Output in the same order as the primary input file, with the matches in alphabetical order, e.g.:

commonwords.txt:
an
and
do
on

allwords.txt:
access
abandon
banana

output.txt:
access:
abandon: an, and, do, on
banana: an

The solution is implemented in C++. The solution is a multithreaded solution.

Solution:
I have created a simple thread pool.
It is useful when we parse input files as well if the input files are big it is useful in processing.
We also use minimum synchronization as when we run multi-threaded processing we access the input and output collections by the index.
Provided output.txt is based on provided 2 input files: allwords.txt and commonwords.txt
Task.cpp is the file with the main function.