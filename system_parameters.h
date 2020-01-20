#ifndef SYSTEM_PARAMETERS_H
#define SYSTEM_PARAMETERS_H

using namespace std;

double static T_u = 1.0; // this is the time unit, representing the number of hours in each time slot of the load and solar traces

extern double B_inv; // cost per cell
extern double PV_inv; // cost per unit (kW) of PV

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


#endif
