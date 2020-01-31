// common.h
#ifndef COMMON_H
#define COMMON_H

// #define DEBUG

#include <vector>

using namespace std;

double static T_u = 1.0; // this is the time unit, representing the number of hours in each time slot of the load and solar traces

extern double B_inv; // cost per cell
extern double PV_inv; // cost per unit (kW) of PV
extern double epsilon;
extern double confidence;
extern int metric;
extern int days_in_chunk;
extern vector<double> load;
extern vector<double> solar;

// information about the data
string static output_data_directory = "results/";

// defines the number of samples, set via command line input
int static number_of_chunks = 100;

// define the upper and lower values to test for battery cells and pv,
// as well as the step size of the search
double static CELLS_MIN = 0.0;
double static CELLS_MAX = 20000.0;
double static CELLS_STEP = 50.0; // search in step of 50 cells

double static PV_MIN = 0.0;
double static PV_MAX = 70.0;
double static PV_STEP = 0.2; //search in steps of 0.2 kW

double static kWh_in_one_cell = 0.011284;

struct SimulationResult {

	double B;
	double C;
	double cost;

	SimulationResult(double B_val, double C_val, double cost_val) : 
					B(B_val), C(C_val), cost(cost_val) {}

};

vector<double> read_data_from_file(string);

int process_input(char**, bool);

#endif
