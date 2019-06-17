To compile all three programs:
make

To run simulation method for sizing:
./sim a b c d e f g h i

To run stochastic network calculus method for sizing with LOLP target:
./snc_lolp a b c e f g h i

To run stochastic network calculus method for sizing with EUE target:
./snc_eue a b d e f g h i


a: name of output file 

b: price per kW of PV panels

c: price per kWh of battery

d: 0 for LOLP metric, 1 for EUE metric

e: epsilon (LOLP target) or theta (eue target) value

f: confidence 

g: number of days in each sample

h: name of load file (for example, see load.txt)

i: name of pv generation file (for example, see pv.txt)

Note that for both SNC methods, parameter d is not used.


For example, suppose we wish to run the simulation method with LOLP target of 5%, confidence of 95% over any 100 day period. The price of PV panels is 2000/kW, the price of battery storage is 500/kWh, and the names of load and pv files are "load.txt" and "pv.txt" respectively, and the output file should have the prefix "test_output" . After downoading the code and going into the directory into which it was downloaded, we would write:

> make
> ./sim test_ouput 2000 500 0 0.05 0.95 100 load.txt pv.txt

Then, check the results directory for the file test_output_sim.size, which will contain a line with three values in the following order: # of kWh of battery, # of kW of PV, and total cost of system.

The output will be in the results directory, with the suffix _sim.size, _snc_lolp.size, or _snc_eue.size, corresponding to the method used.

Additional configuration parameters, such as the size of the search space, or the number of samples taken from the input files, can be found in the corresponding .h file of each method.
