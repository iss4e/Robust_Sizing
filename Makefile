all: sim snc_lolp snc_eue

sim: 
	g++ -std=c++14 -O2 run_simulation.cc cheby.cc simulate_system.cc common.cc -o bin/sim

snc_lolp: 
	g++ -std=c++14 -O2 run_snc_lolp.cc snc_lolp_pertrace.cc common.cc -o bin/snc_lolp

snc_eue: 
	g++ -std=c++14 -O2 run_snc_eue.cc snc_eue_pertrace.cc common.cc -o bin/snc_eue

debug: debug_sim debug_snc_lolp debug_snc_eue

debug_sim: 
	g++ -std=c++14 -O2 -D DEBUG run_simulation.cc cheby.cc simulate_system.cc common.cc -o bin/debug/sim

debug_snc_lolp: 
	g++ -std=c++14 -O2 -D DEBUG run_snc_lolp.cc snc_lolp_pertrace.cc common.cc -o bin/debug/snc_lolp

debug_snc_eue: 
	g++ -std=c++14 -O2 -D DEBUG run_snc_eue.cc snc_eue_pertrace.cc common.cc -o bin/debug/snc_eue

clean: 
	rm bin/sim bin/snc_lolp bin/snc_eue bin/debug/sim bin/debug/snc_lolp bin/debug/snc_eue
