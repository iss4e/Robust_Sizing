//simulate_system.h
#include <vector>
using namespace std;

double static T_u = 1.0; // this is the time unit, representing the number of hours in each time slot of the load and solar traces
double static B_inv = 5.19; // cost per cell
double static PV_inv = 2500.0; // cost per unit (kW) of PV

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


// define the upper and lower values to test for battery cells and pv, 
// as well as the step size of the search
// one cell holds 0.011284 kWh (roughly 11 Wh)
double static cells_min = 0.0;
double static cells_max = 20000.0;
double static cells_step = 50.0;

// one unit of PV has 1 kW of capacity
double static pv_min = 0.0;
double static pv_max = 60.0;
double static pv_step = 0.2;

int static number_of_chunks = 100;

// these values are not used; command line input is used instead
double static epsilon = 0.05;
double static confidence = 0.95;

string static output_data_directory = "results/";


struct SimulationResult {

	double B;
	double C;
	double cost;

	SimulationResult(double B_val, double C_val, double cost_val) : 
					B(B_val), C(C_val), cost(cost_val) {}

};

SimulationResult snc_eue(vector <double> &load_trace, vector <double> &solar_trace, 
						  vector <int> &start_indices, vector <int> &end_indices, double epsilon, double confidence, const int traceLength);