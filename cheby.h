#ifndef CHEBY_H
#define CHEBY_H
#include <vector>
#include "simulate_system.h"


SimulationResult calculate_sample_bound(vector < vector <SimulationResult> > &sizing_curves, double epsilon, double confidence);
#endif