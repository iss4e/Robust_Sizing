#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>

#include "common.h"

double B_inv; // cost per cell
double PV_inv; // cost per unit (kW) of PV
double epsilon;
double confidence;
int metric;
int days_in_chunk;
vector<double> load;
vector<double> solar;

vector<double> read_data_from_file(string filename) {
    
    vector <double> data;

	ifstream datafile(filename.c_str());

	if (datafile.fail()) {
    	data.push_back(-1);
    	cerr << errno << ": read data file " << filename << " failed." << endl;
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

    cout << "read data " << filename << " size = " << data.size() << endl;

    return data;
}

int process_input(char** argv, bool process_metric_input) {
    int i = 0;
    
    string inv_PV_string = argv[++i];
    PV_inv = stod(inv_PV_string);

#ifdef DEBUG
    cout << "inv_PV_string = " << PV_inv
         << ", PV_inv = " << PV_inv << endl;
#endif

    string inv_B_string = argv[++i];
    B_inv = stod(inv_B_string)*kWh_in_one_cell; // convert from per-kWh to per-cell cost

#ifdef DEBUG
    cout << "inv_B_string = " << inv_B_string 
         << ", B_inv = " << B_inv << endl;
#endif

    if (process_metric_input) {
        string metric_string = argv[++i];
        metric = stoi(metric_string);

#ifdef DEBUG
        cout << "metric_string = " << metric_string
            << ", metric = " << metric << endl;
#endif
    }

    string epsilon_string = argv[++i];
    epsilon = stod(epsilon_string);

#ifdef DEBUG
    cout << "epsilon_string = " << epsilon_string
         << ", epsilon = " << epsilon << endl;
#endif

    string confidence_string = argv[++i];
    confidence = stod(confidence_string);

#ifdef DEBUG
    cout << "confidence_string = " << confidence_string
         << ", confidence = " << confidence << endl;
#endif

    string days_in_chunk_string = argv[++i];
    days_in_chunk = stoi(days_in_chunk_string);

#ifdef DEBUG
    cout << "days_in_chunk_string = " << days_in_chunk_string
         << ", days_in_chunk = " << days_in_chunk << endl;
#endif

    string loadfile = argv[++i];

#ifdef DEBUG
    cout << "loadfile = " << loadfile << endl;
#endif

    string solarfile = argv[++i];

#ifdef DEBUG
    cout << "solarfile = " << solarfile << endl;
#endif

	// read in data into vector

#ifdef DEBUG
	cout << "reading solarfile..." << endl;
#endif

	solar = read_data_from_file(solarfile);

#ifdef DEBUG
	cout << "checking for errors in solar file..." << endl;
#endif

	if (solar[0] < 0) {
		cerr << "error reading solar file " << solarfile << endl;
		return 1;
	}

#ifdef DEBUG
	cout << "reading loadfile..." << endl;
#endif

	load = read_data_from_file(loadfile);

#ifdef DEBUG
	cout << "checking for errors in load file..." << endl;
#endif

	if (load[0] < 0) {
		cerr << "error reading load file " << loadfile << endl;
		return 1;
	}

    return 0;
}