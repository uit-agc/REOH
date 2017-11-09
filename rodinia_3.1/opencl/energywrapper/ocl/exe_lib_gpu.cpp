/*
 * =====================================================================================
 *
 *       Filename:  lud.cu
 *
 *    Description:  The main wrapper for the suite
 *
 *        Version:  1.0
 *        Created:  10/22/2009 08:40:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Liang Wang (lw2aw), lw2aw@virginia.edu
 *        Company:  CS@UVa
 *
 * =====================================================================================
 */
//for measure energy
#include <meterpu.h>

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
#include "exe_lib_gpu.h"

#define NUM_ITERATIONS 1

void execute(char* bench, char* input, char* WG, char *out) {
	FILE *of;
	of = fopen(out, "a+");

	char buf[1024];
	snprintf(buf, sizeof buf, "%s %s", bench, input);

	printf("execute command: %s\n", buf);
	//for measure energy
	using namespace meterpu;
	meter<CPU_Time> time_meter;
	meter< NVML_Energy<1> > energy_meter;	
	std::cout << "Configuration= "<<WG<< std::endl;

	fprintf(of, "%s", WG);
	int i;
	double energy_used = 0.0;
	double time_used = 0.0;
	for (i = 0; i < NUM_ITERATIONS; i++) {
		time_meter.start();
		energy_meter.start();

		system(buf);

		energy_meter.stop();
		time_meter.stop();

		time_meter.calc();
		energy_meter.calc();

		energy_used += energy_meter.get_value();
		time_used += time_meter.get_value();
	}
	fprintf(of, "\t%.2f\t%.2f\n", energy_used/(double)NUM_ITERATIONS, time_used/(double)NUM_ITERATIONS);
	fclose(of);
}				


