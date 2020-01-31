// simulate_system.h
#ifndef SIMULATE_SYSTEM_H
#define SIMULATE_SYSTEM_H

#include <vector>
#include "common.h"

using namespace std;

double static num_cells = 200.0; // just a default value that will be updated every time we check a new battery size
double static nominal_voltage_c = 3.8793;
double static nominal_voltage_d = 3.5967;
double static a1_slope = 0.1920;
double static a2_slope = -0.4865;
double static a1_intercept = 0.0*num_cells;
double static a2_intercept = kWh_in_one_cell*num_cells;
double static eta_d = 1/0.9; // taking reciprocal so that we don't divide by eta_d when updating the battery energy content
double static eta_c = 0.9942;
double static alpha_d = a2_intercept*1.0; // the 1 indicates the maximum discharging C-rate
double static alpha_c = a2_intercept*1.0; // the 1 indicates the maximum charging C-rate

double sim(vector <double> &load_trace, vector <double> &solar_trace, int start_index, int end_index,
				 double cells, double pv, double b_0);

vector<SimulationResult> simulate(vector <double> &load_trace, vector <double> &solar_trace, int start_index, int end_index,
						 double b_0);

#endif
