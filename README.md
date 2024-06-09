# Election-to-choose-a-new-president
Simulate a two-round election using MPI. Given voter preferences, determine the winning candidate and in which round they win. Generate a preferences file, divide the file among processes for parallel computation, and display election steps, winner, and candidate percentages per round. Each process loads its part of the file.

## Here are the rules for the election process:
1. There are C candidates (numbered from 1 to C), and V voters.
2. The election process consists of up to 2 rounds. All candidates compete in the first
round. If a candidate receives more than 50% of the votes, he wins, otherwise another
round takes place, in which only the top 2 candidates compete for the presidency, the
candidate who receives more votes than his opponent wins and becomes the new
president.
3. The voters' preferences are the same in both rounds, and each voter must vote
exactly once in each round for a currently competing candidate according to his
preferences.
Given the preferences lists, you need to write a program to announce which candidate
will win and in which round.
## For example:
### If the input
3 5 // number of candidates & number of voters <br>
1 2 3 // voter 1 preference list<br>
1 2 3 // voter 2 preference list<br>
2 1 3 // voter 3 preference list<br>
2 3 1 // voter 4 preference list<br>
3 2 1 // voter 5 preference list<br>
### Then the output will be 2 2 // candidate 2 wins in round 2
### And if input
2 3 // number of candidates & number of voters<br>
2 1 // voter 1 preference list<br>
1 2 // voter 2 preference list<br>
2 1 // voter 3 preference list<br>
### Then the output will be 2 1 // candidate 2 wins in round 1
You must use files so firstly generate by the code file which contains the voters'
preferences, and the format of the file must be number of candidates in the first
line and number of voters in the second line, and then followed by voters’
preferences equal to number of voters.
Note: when running the program the file must not be loaded by one process but
every running process should loads its part.
### Run Steps you must follow: 
When run the program prompt the user to generate file like above of voter’s preferences or to calculate the result and if the user choose the
second option enter the filename as input.
### The Output: 
Print to the console the steps happening in every process and print which candidate will win the elections and in which round. And if there are 2 rounds identity
that also and show the percentage of every candidate per each round.
