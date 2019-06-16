//simulate_system.h
#ifndef SIMULATE_SYSTEM_H
#define SIMULATE_SYSTEM_H
#include <vector>

using namespace std;

double static T_u = 1.0; // this is the time unit, representing the number of hours in each time slot of the load and solar traces
double static B_inv = 5.19; // cost per cell
double static PV_inv = 2500.0; // cost per unit (kW) of PV

double static num_cells = 200.0; // just a default value that will be updated every time we check a new battery size
double static nominal_voltage_c = 3.8793;
double static nominal_voltage_d = 3.5967;
double static a1_slope = 0.1920;
double static a2_slope = -0.4865;
double static a1_intercept = 0.0*num_cells;
double static kWh_in_one_cell = 0.011284;
double static a2_intercept = kWh_in_one_cell*num_cells;
double static eta_d = 1/0.9; // taking reciprocal so that we don't divide by eta_d when updating the battery energy content
double static eta_c = 0.9942;
double static alpha_d = a2_intercept*1.0; // the 1 indicates the maximum discharging C-rate
double static alpha_c = a2_intercept*1.0; // the 1 indicates the maximum charging C-rate

// information about the data
string static output_data_directory = "results/";


// sizing parameters (These are not used; the values are provided through command line interface)
int static days_in_chunk = 100;
double static epsilon = 0.05;
string static epsilon_str = "0.05";
double static confidence = 0.95;
string static confidence_str = "0.95";

// these are used
int static metric = 0; // 0 for LOLP, 1 for unmet load fraction. epsilon is treated as theta (unmet load fraction target) when metric == 1.
int static number_of_chunks = 100;

// define the upper and lower values to test for battery cells and pv, 
// as well as the step size of the search
double static CELLS_MIN = 0.0;
double static CELLS_MAX = 13000.0;
double static CELLS_STEP = 50.0; // search in step of 50 cells

double static PV_MIN = 0.0;
double static PV_MAX = 60.0;
double static PV_STEP = 0.2; //search in steps of 0.2 kW

struct SimulationResult {

	double B;
	double C;
	double cost;

	SimulationResult(double B_val, double C_val, double cost_val) : 
					B(B_val), C(C_val), cost(cost_val) {}

};

double sim(vector <double> &load_trace, vector <double> &solar_trace, int start_index, int end_index,
				 double cells, double pv, double b_0);

vector<SimulationResult> simulate(vector <double> &load_trace, vector <double> &solar_trace, int start_index, int end_index,
						 double b_0);
#endif
