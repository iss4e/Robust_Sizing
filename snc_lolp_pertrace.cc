#include <cmath>
#include <iostream>
#include <cstring>
#include "snc_lolp_pertrace.h"

void update_parameters(double n) {

	num_cells = n;

	a1_intercept = 0.0*num_cells;
	
	a2_intercept = kWh_in_one_cell*num_cells;

	alpha_d = a2_intercept*1.0;
	alpha_c = a2_intercept*1.0;
	return;
}


double ** P_in;
double ** P_out;
double ** P_net;

void computePowers(vector <double> &load_trace, vector <double> &solar_trace, 
					 vector <int> &start_indices, vector <int> &end_indices, double PV, const int traceLength, double * LOLP_1_array) {

	int numTraces = start_indices.size();
	
	for (int k = 0; k < traceLength; k++){

		for (int trace = 0; trace < numTraces; trace++) {
			
			int index = (start_indices[trace] + k) % solar_trace.size();

			P_in[trace][k] = fmin(fmax((solar_trace[index]*PV) - load_trace[index], 0), alpha_c);
			P_out[trace][k] = fmin(fmax(load_trace[index] - (solar_trace[index]*PV), 0), alpha_d);
			P_net[trace][k] = (1/eta_d)*(eta_d*eta_c*P_in[trace][k] - P_out[trace][k]);

		}
	}  

	int num_loss = 0;
	for (int trace = 0; trace < numTraces; trace++) {
		
		for (int i=0; i < traceLength; i++) {
			int index = (start_indices[trace] + i) % traceLength;
			if (solar_trace[index]*PV < load_trace[index]){
				num_loss++;
			}
		}
		LOLP_1_array[trace] = num_loss/(traceLength*1.0);
	}

}

double ** Y;
void computeY(vector <double> &load_trace, vector <double> &solar_trace, 
					 vector <int> &start_indices, vector <int> &end_indices, double PV, const int traceLength, double * losses) {

	int numTraces = start_indices.size();

	// Computing LOLP_2 for each trace
	for (int trace = 0; trace < numTraces; trace++) {
		
		// Count the number of non-zero values in Y, as well as the average of the non-zero values.
		double sum_Y = 0.0;
		int gt0_Y = 0;

		Y[trace][0] = beta_l * (P_out[trace][0]) - (P_net[trace][0]);

		if (Y[trace][0] > 0) {
			gt0_Y += 1;
			sum_Y += Y[trace][0];
		}
		
		for (int j=1; j < traceLength; j++){

			Y[trace][j] = beta_l * (P_out[trace][j]) - (P_net[trace][j]) + 
					fmax(Y[trace][j-1] - beta_l*(P_out[trace][j-1]), 0);	
//					Y[pv_trace][l_trace][j] = beta_l * (P_out[pv_trace][l_trace][j] - mean_P_out[j]) - (P_net[pv_trace][l_trace][j] - mean_P_net[j]) -sigma + 
//							Math.max(Y[pv_trace][l_trace][j-1] - beta_l*(P_out[pv_trace][l_trace][j-1] - mean_P_out[j-1]), 0);	

			if (Y[trace][j] > 0) {
				gt0_Y += 1;
				sum_Y += Y[trace][j];
			}
		}

		double p1 = (gt0_Y*1.0)/(traceLength*1.0);

		// inverse of the average
		double lambda = (gt0_Y*1.0)/(sum_Y*1.0);
		double exp_exponent = -lambda*((a2_intercept - a1_intercept)/T_u);
		
		losses[trace] = p1*exp(exp_exponent);

	}

}


void print_losses(double * losses, int numTraces) {
	for (int trace = 0; trace < numTraces; trace++) {
		cout << losses[trace] << endl;
	}
}

// check the loss values in the losses array. If at least 'confidence' fraction of them are lower than epsilon, losses array is valid. 
bool check_losses(double *losses, int numTraces, double epsilon, double confidence) {

	int num_valid = 0;
	for (int trace = 0; trace < numTraces; trace++) {
		if (losses[trace] <= epsilon) {
			num_valid++;
		}
	}
	if ((num_valid*1.0)/(numTraces*1.0) >= confidence) {
		return true;
	}
	return false;
}


void snc_lolp_core(vector <double> &load_trace, vector <double> &solar_trace, 
								   vector <int> &start_indices, vector <int> &end_indices,
								   double epsilon, double confidence, const int traceLength, double pv, double * losses) {


	int numTraces = start_indices.size();
	
	double * LOLP_1_array = new double [numTraces];
	computePowers(load_trace, solar_trace, start_indices, end_indices, pv, traceLength, LOLP_1_array);

	double loss_probability_bound = 1.0;
	double best_sigma = 0.0;

	computeY(load_trace, solar_trace, start_indices, end_indices, pv, traceLength, losses);
	for (int trace = 0; trace < numTraces; trace++){
		losses[trace] = fmin(LOLP_1_array[trace], losses[trace]);
	}

	delete [] LOLP_1_array;

	return;

}


SimulationResult snc_lolp(vector <double> &load_trace, vector <double> &solar_trace, 
								   vector <int> &start_indices, vector <int> &end_indices,
								   double epsilon, double confidence, const int traceLength) {

	// Init the Y array
	int numTraces = start_indices.size();
	Y = new double*[numTraces];
	for (int i = 0; i < numTraces; i++) {
		Y[i] = new double[traceLength];
	}

	P_in = new double*[numTraces];
	P_out = new double*[numTraces];
	P_net = new double*[numTraces];
	for (int i = 0; i < numTraces; i++) {
		P_in[i] = new double[traceLength];
		P_out[i] = new double[traceLength];
		P_net[i] = new double[traceLength];
	}


	double *losses = new double [numTraces];

	// first, find the lowest value of cells that will get us epsilon loss when the PV is maximized
	// use binary search
	double cells_U = CELLS_MAX;
	double cells_L = CELLS_MIN;
	double mid_cells = 0.0;
	double loss = 0.0;
	
	while (cells_U - cells_L > CELLS_STEP) {

		mid_cells = (cells_L + cells_U) / 2.0;
		update_parameters(mid_cells);

		snc_lolp_core(load_trace, solar_trace, start_indices, end_indices, epsilon, confidence, traceLength, PV_MAX, losses);
		bool valid = check_losses(losses, numTraces, epsilon, confidence);
		//cout << "snc result with " << a2_intercept << " kWh and " << PV_MAX << " pv: " << endl;
		//print_losses(losses,numTraces);
		if (!valid) {
			cells_L = mid_cells;
		} else {
		 	// (loss <= epsilon)
			cells_U = mid_cells;
		}
	}

	// set the starting number of battery cells to be the upper limit that was converged on
	double starting_cells = cells_U;
	double starting_cost = B_inv*starting_cells + PV_inv*PV_MAX;
	double lowest_feasible_pv = PV_MAX;


	double lowest_cost = starting_cost;
	double lowest_B = starting_cells*kWh_in_one_cell;
	double lowest_C = PV_MAX;

	double *losses_prev = new double [numTraces];

	for (double cells = starting_cells; cells <= CELLS_MAX; cells += CELLS_STEP) {

		update_parameters(cells);

		// for each value of cells, find the lowest pv that meets the epsilon loss constraint

		while (true) {

			snc_lolp_core(load_trace, solar_trace, start_indices, end_indices, epsilon, confidence, traceLength, lowest_feasible_pv - PV_STEP, losses);
			bool valid = check_losses(losses, numTraces, epsilon, confidence);
			if (valid) {
				lowest_feasible_pv -= PV_STEP;
				for (int trace = 0; trace < numTraces; trace++) {
					losses_prev[trace] = losses[trace];
				}
				memcpy(losses_prev, losses, sizeof(double)*numTraces);

			} else {
				break;
			}
			
			// this only happens if the trace is very short, since the battery starts half full
			// and can prevent loss without pv for a short time
			if (lowest_feasible_pv <= 0) {
				lowest_feasible_pv = 0;
				break;
			}
		}

		double cost = (B_inv*cells) + (PV_inv*lowest_feasible_pv);

		if (check_losses(losses_prev, numTraces, epsilon, confidence) && (cost < lowest_cost)) {
			lowest_cost = cost;
			lowest_B = cells*kWh_in_one_cell;
			lowest_C = lowest_feasible_pv;
		}

	} 

	for (int i = 0; i < numTraces; i++) {
		delete [] Y[i];
	}
	delete [] Y;

	for (int i = 0; i < numTraces; i++) {
		delete [] P_in[i];
		delete [] P_out[i];
		delete [] P_net[i];
	}

	delete [] losses_prev;
	delete [] losses;

	return SimulationResult(lowest_B, lowest_C, lowest_cost);

}
