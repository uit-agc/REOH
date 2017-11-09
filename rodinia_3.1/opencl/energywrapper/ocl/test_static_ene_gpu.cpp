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

int main(int argc, char **argv) {

	//for measure energy
	using namespace meterpu;
	meter<CPU_Time> time_meter;
	meter< NVML_Energy<1> > energy_meter_gpu;	


	double energy_used = 0.0;
	double time_used = 0.0;

	time_meter.start();
	energy_meter_gpu.start();

	sleep(20);

	energy_meter_gpu.stop();
	time_meter.stop();

	time_meter.calc();
	energy_meter_gpu.calc();

	printf("gpu_energy: %.2f, time: %.2f\n", energy_meter_gpu.get_value(), time_meter.get_value());
}				


