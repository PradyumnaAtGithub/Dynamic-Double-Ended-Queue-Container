CS540 - Advanced Topics in Object Oriented Programming
Assignment - 1
Problem: DYNAMIC DOUBLE ENDED QUEUE

Name: Pradyumna Kaushik
Email: pkaushi1@binghamton.edu
B-Number: B00594994
-------------------------------------------------------

Implementation Details
----------------------

1. I have used a different approach as compared to the Circular Buffer mentioned by the professor.
2. I have implemented the dynamic double ended queue using 2 dynamic arrays which grow in the opposite direction.
3. Below is how my deque is structured,

	n                            0 0                            n
	------------------------------ ------------------------------
	|                            | |			    |
	|	    Array1	     | |	   Array2	    |
	------------------------------ ------------------------------
	ff			    bf fb			    bb

	ff --> front of the Array1
	bf --> back of the Array1
	fb --> front of the Array2
	bb --> back of the Array2

4. Both the arrays together are made to behave as a single deque.
5. Reallocation of Memory
   ----------------------
	1. The laod of each of the arrays is monitored constantly.
	2. When the load of an array has been reached (load factor = 100%) then the array is allocated more memory.
	3. The size of each block of memory allocated is 40,000,000*sizeof(type) bytes.
