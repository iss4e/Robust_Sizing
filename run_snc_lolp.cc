#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <iomanip>
#include <limits>
#include "snc_lolp_pertrace.h"
#include "common.h"

using namespace std;

// run_snc_lolp
// load_filename: filename, each line in file contains electricity consumption value
// solar_filename: filename, each line in file contains solar generation value
// id: request id
// metric: 0 for LOLP, 1 for unmet load
// epsilon: number in range [0,1] representing LOLP or unmet load fraction.
// chunk_size: length of time (in days)
SimulationResult run_snc_lolp(vector <double> &load, vector <double> &solar, double epsilon, double confidence, int chunk_size) {

	// use this random seed
	// srand(10);

	// get number of timeslots in each chunk
	int t_chunk_size = chunk_size*(24/T_u);

	vector <int> chunk_starts;
	vector <int> chunk_ends;
	for (int chunk_num = 0; chunk_num < number_of_chunks; chunk_num += 1) {

		int chunk_start = rand() % solar.size();
		int chunk_end = chunk_start + t_chunk_size;
		chunk_starts.push_back(chunk_start);
		chunk_ends.push_back(chunk_end);

	}

	return snc_lolp(load, solar, chunk_starts, chunk_ends, epsilon, confidence, t_chunk_size);
}

int main(int argc, char ** argv) {

	int input_process_status = process_input(argv, false);

	if (input_process_status) {
		return 1;
	}

	SimulationResult sr = run_snc_lolp(load, solar, epsilon, confidence, days_in_chunk);
	cout << sr.B << "\t" << sr.C << "\t" << sr.cost << endl;

	return 0;
}
