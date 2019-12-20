#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <iomanip>
#include <limits>
#include "snc_lolp_pertrace.h"

using namespace std;

double B_inv; // cost per cell
double PV_inv; // cost per unit (kW) of PV
double epsilon;
double confidence;

// Read data in and run simulations

vector <double> read_data_from_file(string filename) {
    
    vector <double> data;

	ifstream datafile(filename.c_str());
	if (datafile.fail()) {
    	data.push_back(-1);
    	//cout << errno << endl;
    	return data;
  	}

    // read data file into vector
    string line;
    double value;

    while (getline(datafile, line)) {
    	istringstream iss(line);
    	iss >> value;
    	data.push_back(value);
    }

    return data;
}


// load_filename: filename, each line in file contains electricity consumption value
// solar_filename: filename, each line in file contains solar generation value
// id: request id
// metric: 0 for LOLP, 1 for unmet load
// epsilon: number in range [0,1] representing LOLP or unmet load fraction.
// chunk_size: length of time (in days)

void run_snc_lolp(vector <double> &load, vector <double> &solar, string id, double epsilon, double confidence, int chunk_size) {

	// use this random seed
	//srand(10);

	// get number of timeslots in each chunk
	int t_chunk_size = chunk_size*(24/T_u);

	
	ofstream resultsfile;
	resultsfile.open(output_data_directory + id + "_snc_lolp.size");


	vector <int> chunk_starts;
	vector <int> chunk_ends;
	for (int chunk_num = 0; chunk_num < number_of_chunks; chunk_num += 1) {

		int chunk_start = rand() % solar.size();
		int chunk_end = chunk_start + t_chunk_size;
		chunk_starts.push_back(chunk_start);
		chunk_ends.push_back(chunk_end);

	}

	SimulationResult sr = snc_lolp(load, solar, chunk_starts, chunk_ends, epsilon, confidence, t_chunk_size);
	resultsfile << sr.B << "\t" << sr.C << "\t" << sr.cost << endl;

	resultsfile.close();

	return;
}

int main(int argc, char ** argv) {

	// uncomment this to specify a specific seed for RNG
	//srand(10);

	cout << argv[1] << endl;
	string outputfile = argv[1];
	
	cout << argv[2] << endl;
	string inv_PV_string = argv[2];
	PV_inv = stod(inv_PV_string);

	cout << argv[3] << endl;
	string inv_B_string = argv[3];
	B_inv = stod(inv_B_string)*kWh_in_one_cell; // convert from per-kWh to per-cell cost

	cout << argv[4] << endl;
	string epsilon_string = argv[4];
	epsilon = stod(epsilon_string);

	cout << argv[5] << endl;
	string confidence_string = argv[5];
	confidence = stod(confidence_string);

	cout << argv[6] << endl;
	string days_in_chunk_string = argv[6];
	int days_in_chunk = stoi(days_in_chunk_string);

	cout << argv[7] << endl;
	string loadfile = argv[7];

	cout << argv[8] << endl;
	string solarfile = argv[8];

	// this parameter sets the number of random samples we take from the load and solar traces
	//int number_of_chunks = 100;

	// read in the data

	// read in data into vector
	cout << "reading solarfile" << endl;
	vector <double> solar = read_data_from_file(solarfile);
	cout << "reading loadfile" << endl;
	vector <double> load = read_data_from_file(loadfile);

	cout << "checking for errors in solar file" << endl;
	if (solar[0] < 0) {
		cout << "error reading solar file " <<  solarfile << endl;
		return 1;
	}
	cout << "checking for errors in load file" << endl;
	if (load[0] < 0) {
		cout << "error reading load file " << loadfile << endl;
		return 1;
	}


	run_snc_lolp(load, solar, outputfile, epsilon, confidence, days_in_chunk);


}

