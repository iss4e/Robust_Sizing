#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <climits>

#include "common.h"

double B_inv; // cost per cell
double PV_inv; // cost per unit (kW) of PV

double cells_min;
double cells_max;
double cells_step; // search in step of x cells
double pv_min;
double pv_max;
double pv_step; // search in steps of x kW

double epsilon;
double confidence;
int metric;
int days_in_chunk;

vector<double> load;
vector<double> solar;

vector<double> read_data_from_file(istream &datafile, int limit = INT_MAX) {

    vector <double> data;

	if (datafile.fail()) {
    	data.push_back(-1);
    	cerr << errno << ": read data file failed." << endl;
    	return data;
  	}

    // read data file into vector
    string line;
    double value;

    for (int i = 0; i < limit && getline(datafile, line); ++i) {
    	istringstream iss(line);
    	iss >> value;
    	data.push_back(value);
    }

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

    string pv_max_string = argv[++i];
    pv_max = stod(pv_max_string);

    // set default pv_min and pv_step
    pv_min = 0;
    pv_step = (pv_max - pv_min) / num_pv_steps;

#ifdef DEBUG
    cout << "pv_max_string = " << pv_max_string
         << ", pv_max = " << pv_max
         << ", pv_min = " << pv_min
         << ", pv_step = " << pv_step
         << endl;
#endif

    string cells_max_string = argv[++i];
    cells_max = stod(cells_max_string) / kWh_in_one_cell;

    // set default cells_min and cells_step
    cells_min = 0;
    cells_step = (cells_max - cells_min) / num_cells_steps;

#ifdef DEBUG
    cout << "cells_max_string = " << cells_max_string
         << ", cells_max = " << cells_max
         << ", cells_min = " << cells_min
         << ", cells_step = " << cells_step
         << endl;
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

    if (loadfile == string("--")) {
        // read from cin
        int limit = stoi(argv[++i]);

#ifdef DEBUG
        cout << "reading load data from stdin. limit = " << limit << endl;
#endif

        load = read_data_from_file(cin, limit);
    } else {

#ifdef DEBUG
        cout << "reading load file" << endl;
#endif

        // read in data into vector
        ifstream loadstream(loadfile.c_str());
        load = read_data_from_file(loadstream);
    }

#ifdef DEBUG
	cout << "checking for errors in load file..." << endl;
#endif

	if (load[0] < 0) {
		cerr << "error reading load file " << loadfile << endl;
		return 1;
	}

    string solarfile = argv[++i];

#ifdef DEBUG
    cout << "solarfile = " << solarfile << endl;
#endif

    if (solarfile == string("--")) {

#ifdef DEBUG
        cout << "reading solar file" << endl;
#endif

        // read from cin
        int limit = stoi(argv[++i]);

#ifdef DEBUG
        cout << "reading solar data from stdin. limit = " << limit << endl;
#endif

        solar = read_data_from_file(cin, limit);
    } else {
        // read in data into vector
        ifstream solarstream(solarfile.c_str());
        solar = read_data_from_file(solarstream);
    }

#ifdef DEBUG
	cout << "checking for errors in solar file..." << endl;
#endif

	if (solar[0] < 0) {
		cerr << "error reading solar file " << solarfile << endl;
		return 1;
	}

    return 0;
}
