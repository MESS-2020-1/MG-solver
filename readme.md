# Multistart Greedy

This repository stores the C++ source code of the Multistart Greedy solver used for the [MESS-2020+1 competition](https://www.ants-lab.it/mess2020/#competition).

Compile the solver with:

`make`

Run it with: 

`./WL_Greedy.exe <input_file> <solution_file> <timeout> <random_seed>`

For example, the command line:

`WL_Greedy.exe ../Instances/Public/wlp18.dzn sol18.txt 200 0`

Runs for 200s the solver on instance `wlp18.dzn` stored in the directory `../Instances/Public/` and delivers the solution in the file `sol18.txt`.

 