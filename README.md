# Robust Sizing

### To compile all three programs
```
make
```

### To run programs
Simulation method for sizing:
```
./sim b c d e f g h i
```

Stochastic network calculus method for sizing with LOLP target:
```
./snc_lolp b c e f g h i
```

Stochastic network calculus method for sizing with EUE target:
```
./snc_eue b c e f g h i
```

Where:

`b`: price per kW of PV panels

`c`: price per kWh of battery

`d`: 0 for LOLP metric, 1 for EUE metric

`e`: epsilon (for LOLP target) or theta (for EUE target) value

`f`: confidence 

`g`: number of days in each sample

`h`: name of load file, values in kW (for example, see load.txt)

`i`: name of pv generation file, values in kW (for example, see pv.txt)

Note that for both SNC methods, parameter d is not used.

For example, suppose we wish to run the simulation method with LOLP target of 1% (or EUE target of 5%), confidence of 95% over any 100 day period. The price of PV panels is 2000/kW, the price of battery storage is 500/kWh, and the names of load and pv files are "load.txt" and "pv.txt" respectively, and the output file should have the prefix "test_output" . After downoading the code and going into the directory into which it was downloaded, we would write:
```bash
make

# Simulation Method LOLP
./sim 2000 500 0 0.01 0.95 100 load.txt pv.txt

# SNC Method LOLP
./snc_lolp 2000 500 0.01 0.95 100 load.txt pv.txt

# Simulation Method EUE
./sim 2000 500 1 0.05 0.95 100 load.txt pv.txt

# SNC Method EUE
./snc_eue 2000 500 0.05 0.95 100 load.txt pv.txt
```

The output values will be in the standard ouput. It contains a line with three values in the following order, separated by tabs: # of kWh of battery, # of kW of PV, and total cost of the system.

To output the results into a file, add `>> output.file` at the end of the command:
```bash
./sim 2000 500 0 0.01 0.95 100 load.txt pv.txt >> output.txt
```

Additional configuration parameters, such as the size of the search space, or the number of samples taken from the input files, can be found in the corresponding .h file of each method, or `common.h`.
