#include <cmath>
#include <iostream>
#include <cstring>
#include "snc_eue_pertrace.h"

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
double *mean_D; // TODO: ask Yashar whether we need one of these for each trace
double ** Y;
double * pI;
double * lambdaI;
double * pII;
double * lambdaII;
void compute_I(vector <double> &load_trace, vector <double> &solar_trace, 
					 vector <int> &start_indices, vector <int> &end_indices, double PV, const int traceLength) {

	int numTraces = start_indices.size();
	int solar_size = solar_trace.size();
	int load_size = load_trace.size();

	/////////////////////////////
	// Yashar: Mean(Pin) and Mean(Pout) is the average of what is happening at the same time at any day within the same season

	/*double sum_P_in = 0;
	double sum_P_out = 0;
	double sum_P_net = 0;
	double sum_mean_P_out = 0;
	double sum_mean_P_net = 0;*/


	vector <int> gt0_diff (numTraces,0);
	vector <double> sum_diff (numTraces,0);

	/*double sum_P_in = 0;
	double sum_P_out = 0;
	double sum_P_net = 0;*/
	for (int trace = 0; trace < numTraces; trace++) {
		mean_D[trace] = 0;
		double sum_diff = 0.0;
		int gt0_diff = 0;
		for (int k = 0; k < traceLength; k++) {		
			
			int index_solar = (start_indices[trace] + k) % solar_size;
			int index_load = (start_indices[trace] + k) % load_size;

			P_in[trace][k] = fmin(fmax((solar_trace[index_solar]*PV) - load_trace[index_load], 0), alpha_c);
			P_out[trace][k] = fmin(fmax(load_trace[index_load] - (solar_trace[index_solar]*PV), 0), alpha_d);
			P_net[trace][k] = (1/eta_d)*(eta_d*eta_c*P_in[trace][k] - P_out[trace][k]); //Yashar: defined as the round-trip efficiency

			double diff = load_trace[index_load] - solar_trace[index_solar]*PV;
			if (diff > 0) {
				gt0_diff += 1;
				sum_diff += diff;
			}

			mean_D[trace] += load_trace[index_load];

			/*sum_P_in += P_in[trace][k];
			sum_P_out += P_out[trace][k];
			sum_P_net += P_net[trace][k];*/

		}
		mean_D[trace] = mean_D[trace]/traceLength;
		pI[trace] = gt0_diff/(traceLength*1.0);
		lambdaI[trace] = gt0_diff/(sum_diff*1.0);
	}
	//cout << "sum P_in: " << sum_P_in << ", sum P_out: " << sum_P_out << ", sum P_net: " << sum_P_net << endl;

}


void compute_II(vector <double> &load_trace, vector <double> &solar_trace, 
					 vector <int> &start_indices, vector <int> &end_indices, double PV, const int traceLength) {

	int numTraces = start_indices.size();

	/*
	double maxG [traceLength];
	maxG[0] = beta_l * mean_P_out[0] - mean_P_net[0] + sigma;
	// double sigma_Star = Math.max(0, maxG[0]);
	double sigma_Star = maxG[0];
	for (int i = 1; i < traceLength; i++) {
		maxG[i] = beta_l * mean_P_out[i] - mean_P_net[i] +  
				+ fmax(sigma, maxG[i-1] - beta_l * mean_P_out[i-1] + mean_P_net[i-1] );	
		sigma_Star = fmax(sigma_Star, maxG[i]);
	}

	
	if (((a2_intercept - a1_intercept)/T_u) - sigma_Star <= 0){

		for(int trace = 0; trace < numTraces; trace++) {
			losses[trace] = 1;
		}
		return;
		
	}*/

	// Computing LOLP_2 for each trace
	for (int trace = 0; trace < numTraces; trace++) {
		
		// Count the number of non-zero values in Y, as well as the average of the non-zero values.
		double sum_Y = 0.0;
		int gt0_Y = 0;

		Y[trace][0] = beta_l * (P_out[trace][0] ) - (P_net[trace][0]);

		if (Y[trace][0] > 0) {
			gt0_Y += 1;
			sum_Y += Y[trace][0];
		}
		
		for (int j=1; j < traceLength; j++){

			Y[trace][j] = beta_l * P_out[trace][j] - P_net[trace][j] + 
					fmax(Y[trace][j-1] - beta_l*(P_out[trace][j-1]), 0); // TODO: this used to be max (...., -sigma). is it right to replace it with 0?
//					Y[pv_trace][l_trace][j] = beta_l * (P_out[pv_trace][l_trace][j] - mean_P_out[j]) - (P_net[pv_trace][l_trace][j] - mean_P_net[j]) -sigma + 
//							Math.max(Y[pv_trace][l_trace][j-1] - beta_l*(P_out[pv_trace][l_trace][j-1] - mean_P_out[j-1]), 0);	

			if (Y[trace][j] > 0) {
				gt0_Y += 1;
				sum_Y += Y[trace][j];
			}
		}

		pII[trace] = (gt0_Y*1.0)/(traceLength*1.0);

		// inverse of the average
		lambdaII[trace] = (gt0_Y*1.0)/(sum_Y*1.0);
		pII[trace] = pII[trace]*exp(-lambdaII[trace]*(a2_intercept-a1_intercept)/T_u);
		//double exp_exponent = -lambda*(((a2_intercept - a1_intercept)/T_u) - sigma_Star);
		//double exp_exponent = -lambda*((a2_intercept - a1_intercept)/T_u);
		//cout << "p1: " << p1 << ", exp_exponent: " << exp_exponent << ", sigma: " << sigma << endl;
		//losses[trace] = p1*exp(exp_exponent);

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


void snc_eue_core(vector <double> &load_trace, vector <double> &solar_trace, 
								   vector <int> &start_indices, vector <int> &end_indices,
								   double epsilon, double confidence, const int traceLength, double pv, double * losses) {

	int numTraces = start_indices.size();

	compute_I(load_trace, solar_trace, start_indices, end_indices, pv, traceLength);
	compute_II(load_trace, solar_trace, start_indices, end_indices, pv, traceLength);
	

	/*cout << "pI: " << pI[0] << endl;
	cout << "lambdaI: " << lambdaI[0] << endl;
	cout << "pII: " << pII[0] << endl;
	cout << "lambdaII: " << lambdaII[0] << endl;*/

	for (int trace = 0; trace < numTraces; trace++){

		double EUL_temp = 0;
		if ((pI[trace] > pII[trace]) && (lambdaI[trace] > lambdaII[trace])){
			EUL_temp = pII[trace]/lambdaII[trace] * (1 - pow(pII[trace]/pI[trace], lambdaII[trace]/(lambdaI[trace] - lambdaII[trace]))) + pI[trace]/lambdaI[trace] * pow(pII[trace]/pI[trace], lambdaI[trace]/(lambdaI[trace] - lambdaII[trace]));
			//cout << "1" << endl;
		}
		else if ((pII[trace] > pI[trace]) && (lambdaII[trace] > lambdaI[trace])){
			EUL_temp = pI[trace]/lambdaI[trace] * (1 - pow(pI[trace]/pII[trace], lambdaI[trace]/(lambdaII[trace] - lambdaI[trace]))) + pII[trace]/lambdaII[trace] * pow(pI[trace]/pII[trace], lambdaII[trace]/(lambdaII[trace] - lambdaI[trace]));
			//cout << "2" << endl;
		}
		else if ((pI[trace] > pII[trace]) && (lambdaII[trace] > lambdaI[trace])){
			EUL_temp = pII[trace] / lambdaII[trace];
			//cout << "3" << endl;
		}
		else {
			EUL_temp = pI[trace] / lambdaI[trace];
			//cout << "4" << endl;
		}
		losses[trace] = EUL_temp / mean_D[trace];
	}

	return;

}


SimulationResult snc_eue(vector <double> &load_trace, vector <double> &solar_trace, 
								   vector <int> &start_indices, vector <int> &end_indices,
								   double epsilon, double confidence, const int traceLength) {

	// Init the Y array
	int numTraces = start_indices.size();
	Y = new double*[numTraces];
	for (int i = 0; i < numTraces; i++) {
		Y[i] = new double[traceLength];
		/*for (int j = 0; j < traceLength; j++) {
			Y[i][j] = 0;
		}*/
	}

	P_in = new double*[numTraces];
	P_out = new double*[numTraces];
	P_net = new double*[numTraces];
	for (int i = 0; i < numTraces; i++) {
		P_in[i] = new double[traceLength];
		P_out[i] = new double[traceLength];
		P_net[i] = new double[traceLength];
	}

	mean_D = new double[numTraces];
	pI = new double[numTraces];
	pII = new double[numTraces];
	lambdaI = new double[numTraces];
	lambdaII = new double[numTraces];

	double *losses = new double [numTraces];

	// first, find the lowest value of cells that will get us epsilon loss when the PV is maximized
	// use binary search
	double cells_U = CELLS_MAX;
	double cells_L = CELLS_MIN;
	double mid_cells = 0.0;
	double loss = 0.0;

	// debug code:
	// update_parameters(20/0.011284);
	// loss = snc_lolp_core(load_trace, solar_trace, start_indices, end_indices, epsilon, traceLength, 15);
	// cout << "loss: " << loss << endl;
	// return SimulationResult(0,0,0);
	
	while (cells_U - cells_L > CELLS_STEP) {

		mid_cells = (cells_L + cells_U) / 2.0;
		update_parameters(mid_cells);

		snc_eue_core(load_trace, solar_trace, start_indices, end_indices, epsilon, confidence, traceLength, PV_MAX, losses);
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

			snc_eue_core(load_trace, solar_trace, start_indices, end_indices, epsilon, confidence, traceLength, lowest_feasible_pv - PV_STEP, losses);
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
		//cout << "snc result with " << a2_intercept << " kWh and " << lowest_feasible_pv << " kW pv:"  << endl;
		//print_losses(losses_prev,numTraces);
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

	delete [] mean_D;
	delete [] pI;
	delete [] pII;
	delete [] lambdaI;
	delete [] lambdaII;

	delete [] losses_prev;
	delete [] losses;

	return SimulationResult(lowest_B, lowest_C, lowest_cost);

}
