#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <iomanip>
#include <limits>
#include "simulate_system.h"
#include "common.h"
#include "cheby.h"

using namespace std;

// run_simulations
// load_filename: filename, each line in file contains electricity consumption value
// solar_filename: filename, each line in file contains solar generation value
// id: request id
// metric: 0 for LOLP, 1 for unmet load
// epsilon: number in range [0,1] representing LOLP or unmet load fraction.
// chunk_size: length of time (in days)
SimulationResult run_simulations(vector <double> &load, vector <double> &solar, int metric, int chunk_size, int number_of_chunks, bool output_curves) {

	// set random seed to a specific value if you want consistency in results
	// srand(10);

	// get number of timeslots in each chunk
	int t_chunk_size = chunk_size*(24/T_u);

	vector <vector<SimulationResult> > results;

	// get random start times and run simulation on this chunk of data
	for (int chunk_num = 0; chunk_num < number_of_chunks; chunk_num += 1) {

		int chunk_start = rand() % solar.size();
		int chunk_end = chunk_start + t_chunk_size;

		vector <SimulationResult> sr = simulate(load, solar, chunk_start, chunk_end, 0);
		results.push_back(sr);

	}

	// print all of the curves
	// if (output_curves) {
	// 	int chunk_index = 1;
	// 	for(vector<vector<SimulationResult> >::iterator it = results.begin() ; it != results.end(); ++it) {
			
	// 		ofstream curvefile;
	// 		curvefile.open(id + "_" + to_string(chunk_index) + ".out");
			
	// 		for (vector<SimulationResult>::iterator it2 = it->begin() ; it2 != it->end(); ++it2) {
	// 			curvefile << it2->B << " " << it2->C << " " << it2->cost << endl;
	// 		}
			
	// 		curvefile.close();
	// 		chunk_index += 1;
	// 	}
	// }

	// calculate the chebyshev curves, find the cheapest system along their upper envelope, and return it
	return calculate_sample_bound(results, epsilon, confidence);
}

int main(int argc, char ** argv) {
	
	int input_process_status = process_input(argv, true);

	if (input_process_status) {
		return 1;
	}
	
	SimulationResult sr = run_simulations(load, solar, metric, days_in_chunk, number_of_chunks, false);
	cout << sr.B << "\t" << sr.C << "\t" << sr.cost << endl;

	return 0;
}
