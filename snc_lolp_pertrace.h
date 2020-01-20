//simulate_system.h
#include <vector>
#include "system_parameters.h"
using namespace std;


double static num_cells = 200.0; // just a default value that will be updated every time we check a new battery sizee
double static nominal_voltage_c = 3.8793;
double static nominal_voltage_d = 3.5967;
double static a1_slope = 0.1920;
double static a2_slope = -0.4865;
double static a1_intercept = 0.0*num_cells;
double static kWh_in_one_cell = 0.011284;
double static a2_intercept = kWh_in_one_cell*num_cells;
double static eta_d = 0.9;
double static eta_c = 0.9942;
double static alpha_d = a2_intercept*1.0; // the 1 indicates the maximum discharging C-rate
double static alpha_c = a2_intercept*1.0; // the 1 indicates the maximum charging C-rate
double static beta_u = a2_slope/(T_u*nominal_voltage_c);
double static beta_l = a1_slope/(T_u*nominal_voltage_d);

// these values are not used; command line input is used instead
extern double epsilon;
extern double confidence;

struct SimulationResult {

	double B;
	double C;
	double cost;

	SimulationResult(double B_val, double C_val, double cost_val) : 
					B(B_val), C(C_val), cost(cost_val) {}

};

SimulationResult snc_lolp(vector <double> &load_trace, vector <double> &solar_trace, 
						  vector <int> &start_indices, vector <int> &end_indices, double epsilon, double confidence, const int traceLength);
