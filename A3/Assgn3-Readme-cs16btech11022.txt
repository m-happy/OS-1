1. Create a file "inp-params.txt" and put the initial input parameters into this file.
	Format to be followed is :
	nw(No. writer threads) nr(No. reader threads)
	kw(Repetitions) kr(Repititions)
	csSeed remSeed

	Example :
	5 5
	3 4
	1 2

2. Compile the Reader_Writer and Fair_Reader_Writer code by following commands :
	a) g++ Assgn3-RWcs16btech110222.cpp -std=c++11 -pthread -o rw
	b) g++ Assgn3-Fair_RWcs16btech11022.cpp -std=c++11 -pthread -o frw

3. Run by executing :
	a) ./rw
	b) ./frw

4. Check the RW-log.txt for checking the logs of Reader Writer and check FairRW-log.txt for Fair Reader Writer.

5. Check out Average_times.txt for checking out the Average Waiting times
