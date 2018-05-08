1. Create a file "inp-params.txt" and put the initial input parameters into this file.
	Format to be followed is :
	n (No. writer threads) k(No. reader threads) preLam postLam

	Example :
	10 4 1 2
	

2. Compile the new barrier and pthread barrier code by following commands :
	a) g++ Assgn4-newbarrcs16btech110222.cpp -std=c++11 -pthread -o new
	b) g++ Assgn4-ptbarrcs16btech11022.cpp -std=c++11 -pthread -o -pt

3. Run by executing :
	a) ./new
	b) ./pt

