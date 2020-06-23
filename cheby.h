// cheby.h
#ifndef CHEBY_H
#define CHEBY_H

#include <vector>

#include "common.h"
#include "simulate_system.h"

SimulationResult calculate_sample_bound(vector < vector <SimulationResult> > &sizing_curves, double epsilon, double confidence);

#endif