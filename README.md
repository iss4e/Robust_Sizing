# Robust Sizing

### To compile all three programs
```
make
```

### To run programs
Simulation method for sizing:
```
./bin/sim pv_cost battery_cost pv_max cell_max metric epsilon conf n_days load pv
```

Stochastic network calculus method for sizing with LOLP target:
```
./bin/snc_lolp pv_cost battery_cost pv_max cell_max epsilon conf n_days load pv
```

Stochastic network calculus method for sizing with EUE target:
```
./bin/snc_eue pv_cost battery_cost pv_max cell_max epsilon conf n_days load pv
```

Where:

`pv_cost`: price per kW of PV panels

`battery_cost`: price per kWh of battery

`pv_max`: max number of PV panels available, in terms of kW

`cell_max`: max number of battery available, in terms of kWh

`metric`: 0 for LOLP metric, 1 for EUE metric

`epsilon`: epsilon (for LOLP target) or theta (for EUE target) value

`conf`: confidence 

`n_days`: number of days in each sample

`load`: name of load file, values in kW (for example, see load.txt)

`pv`: name of pv generation file, values in kW (for example, see pv.txt)

Note that for both SNC methods, parameter d is not used.

For example, suppose we wish to run the simulation method with:

- LOLP target of **1%** (or EUE target of **5%**)
- Confidence of **95%** over any **100 day** period.
- The price of PV panels is **2000/kW**, the price of battery storage is **500/kWh**
- There are at most **70 kW** of PV panels and **225 kWh** of batteries available
- The names of load and pv files are "example_inputs/load.txt" and "example_inputs/pv.txt" respectively.

Then after downoading the code and going into the directory into which it was downloaded, we would write:
```bash
make

# Simulation Method LOLP
./bin/sim 2000 500 70 225 0 0.01 0.95 100 example_inputs/load.txt example_inputs/pv.txt

# SNC Method LOLP
./bin/snc_lolp 2000 500 70 225 0.01 0.95 100 example_inputs/load.txt example_inputs/pv.txt

# Simulation Method EUE
./bin/sim 2000 500 70 225 1 0.05 0.95 100 example_inputs/load.txt example_inputs/pv.txt

# SNC Method EUE
./bin/snc_eue 2000 500 70 225 0.05 0.95 100 example_inputs/load.txt example_inputs/pv.txt
```

We can also take electricity load and pv generation metrics through stdin. Instead of entering the file name in the command, enter `--` followed by number of lines to be taken in stdin. For example,
```bash
./bin/sim 2000 500 70 225 0 0.01 0.95 100 -- 8760 -- 8760
```
would first take 8760 lines of load metrics, then 8760 lines of pv metrics.

The output values will be in the standard ouput. It contains a line with three values in the following order, separated by tabs: # of kWh of battery, # of kW of PV, and total cost of the system.

To output the results into a file, add `> output.file` at the end of the command:
```bash
./bin/sim 2000 500 70 225 0 0.01 0.95 100 example_inputs/load.txt example_inputs/pv.txt > results/output.txt
```

Additional configuration parameters, such as the size of the search space, or the number of samples taken from the input files, can be found in the corresponding .h file of each method, or `common.h`.
