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

The output will be in the results directory, with the suffix _sim.size, _snc_lolp.size, or _snc_eue.size, corresponding to the method used.

Additional configuration parameters, such as the size of the search space, or the number of samples taken from the input files, can be found in the corresponding .h file of each method.
