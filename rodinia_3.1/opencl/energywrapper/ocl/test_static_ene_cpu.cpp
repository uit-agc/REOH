//for measure energy
#include <MeterPU.h>

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

int main(int argc, char **argv) {

	//for measure energy
	using namespace MeterPU;
	Meter<CPU_Time> time_meter;
	Meter<PCM_Energy> energy_meter_cpu;


	time_meter.start();
	energy_meter_cpu.start();

	sleep(20);

	energy_meter_cpu.stop();
	time_meter.stop();

	time_meter.calc();
	energy_meter_cpu.calc();

	printf("cpu energy: %.2f, time: %.2f\n", energy_meter_cpu.get_value(), time_meter.get_value());
}				


