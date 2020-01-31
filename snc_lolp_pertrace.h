//snc_lolp_pertrace.h
#ifndef SNC_LOLP_PERTRACE_H
#define SNC_LOLP_PERTRACE_H

#include <vector>
#include "common.h"

using namespace std;

double static num_cells = 200.0; // just a default value that will be updated every time we check a new battery sizee
double static nominal_voltage_c = 3.8793;
double static nominal_voltage_d = 3.5967;
double static a1_slope = 0.1920;
double static a2_slope = -0.4865;
double static a1_intercept = 0.0*num_cells;
double static a2_intercept = kWh_in_one_cell*num_cells;
double static eta_d = 0.9;
double static eta_c = 0.9942;
double static alpha_d = a2_intercept*1.0; // the 1 indicates the maximum discharging C-rate
double static alpha_c = a2_intercept*1.0; // the 1 indicates the maximum charging C-rate
double static beta_u = a2_slope/(T_u*nominal_voltage_c);
double static beta_l = a1_slope/(T_u*nominal_voltage_d);

SimulationResult snc_lolp(vector <double> &load_trace, vector <double> &solar_trace, 
						  vector <int> &start_indices, vector <int> &end_indices, double epsilon, double confidence, const int traceLength);

#endif
