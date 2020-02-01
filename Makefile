all: sim snc_lolp snc_eue

sim: 
	g++ -O2 run_simulation.cc cheby.cc simulate_system.cc common.cc -o sim

snc_lolp: 
	g++ -O2 run_snc_lolp.cc snc_lolp_pertrace.cc common.cc -o snc_lolp

snc_eue: 
	g++ -O2 run_snc_eue.cc snc_eue_pertrace.cc common.cc -o snc_eue

debug: debug_sim debug_snc_lolp debug_snc_eue

debug_sim: 
	g++ -O2 -D DEBUG run_simulation.cc cheby.cc simulate_system.cc common.cc -o sim

debug_snc_lolp: 
	g++ -O2 -D DEBUG run_snc_lolp.cc snc_lolp_pertrace.cc common.cc -o snc_lolp

debug_snc_eue: 
	g++ -O2 -D DEBUG run_snc_eue.cc snc_eue_pertrace.cc common.cc -o snc_eue

clean: 
	rm sim snc_lolp snc_eue
