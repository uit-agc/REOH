#include <MeterPU.h>
//for numa
#include "cpu-lib.h"
#include "cpu-lib.c"

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
//#include "common.h"
#include <sys/time.h>
#include <CL/cl.h>
#include <string.h>
#include <string>
#include "exe_lib_cpu.h"

#define NUM_ITERATIONS 1

void execute_cpu(char* bench, char* input, int t_ncpus, int t_freqind, int t_memctrl, char *out)
{

	FILE *of;

	of = fopen(out, "a+");

	using namespace MeterPU;
	Meter<CPU_Time> time_meter;
	Meter<PCM_Energy> energy_meter;

	int maxncpus = 0;	
	maxncpus = get_current_cpu_count();
	printf("maxncpus %d\n", maxncpus);
	char buf[1024];
	snprintf(buf, sizeof buf, "%s %s", bench, input);
	
	printf("from execute_cpu: %s", buf);
	free(input);

	int ncpus = t_ncpus;
	int freqind = t_freqind;
	int memctrl = t_memctrl;
	
	std::cout << "Configuration= "<< ncpus<<"--"<<freqind<<"--"<<memctrl<< std::endl;
	//apply governor to userspace for all cores
	char* expectedgov="userspace";
	for (int i = 0; i< maxncpus ; i++){
		apply_cpu_gov(i, expectedgov);
	}
	check_numa();
				
	//set frequency index
	unsigned long expectedfreq = available_freqs[freqind];
	printf("Change frequency now to %lu!\n", expectedfreq);
	for (int i = 0; i< maxncpus ; i++)
		apply_cpu_freq(i, expectedfreq);
				
	//set number of cores
	set_cpu_cores_used( ncpus-1);
	printf("Now we have %d CPUS\n", get_current_cpu_count());
				
	//set memory channel	
	set_numa_mem_node(memctrl); // one channel
		
	// Vi: start energy meter

	fprintf(of, "%d\t%d\t%d", ncpus, freqind, memctrl);

	unsigned int i;
	double energy_used = 0.0;
	double time_used = 0.0;
	for (i = 0; i < NUM_ITERATIONS; i++) {

		// beginning of timing point				
		time_meter.start();
		energy_meter.start();

		system(buf);

		// end of timing point							
		energy_meter.stop();
		time_meter.stop();
				
		time_meter.calc();
		energy_meter.calc();
				
//		energy_meter.show_meter_reading();	
//		time_meter.show_meter_reading();
//		fprintf(of, ", %d, %d", energy_meter.get_value(), time_meter.get_value());
		energy_used += energy_meter.get_value();
		time_used += time_meter.get_value();
	}
	fprintf(of, "\t%.2f\t%.2f\n", energy_used/(double)NUM_ITERATIONS, time_used/(double)NUM_ITERATIONS);
				
	fclose(of);
}
