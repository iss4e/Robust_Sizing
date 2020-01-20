//simulate_system.h
#ifndef SIMULATE_SYSTEM_H
#define SIMULATE_SYSTEM_H
#include <vector>

using namespace std;

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


// sizing parameters (These are not used; the values are provided through command line interface)
extern int days_in_chunk;
extern double epsilon;
extern string epsilon_str;
extern double confidence;
extern string confidence_str;
extern int metric; // 0 for LOLP, 1 for unmet load fraction. epsilon is treated as theta (unmet load fraction target) when metric == 1.

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
