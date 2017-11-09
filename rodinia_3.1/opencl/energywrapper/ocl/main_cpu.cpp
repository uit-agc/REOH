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
//#include <MeterPU.h>
//for numa
//#include "cpu-lib.h"
//#include "cpu-lib.c"

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

int main ( int argc, char *argv[] )
{	
	//reading the input_filename
	char *bench;
	char* input;
	int t_ncpus = 1;
	int t_freqind= 1;
	int t_memctrl= 1;
	char *out;

	if (argc>=6) {
		size_t len = strlen(argv[1]);
		bench = (char*)malloc(len+1);
		if (bench == NULL) {
			printf("allocation failed\n");
		}
		strcpy(bench, argv[1]);

		len = strlen(argv[2]);
		input = (char*)malloc(len+1);
		if(input==NULL)
		{
			printf("allocation failed\n");
		}
		strcpy(input, argv[2]);
		printf("%s\n", input );

		t_ncpus = atoi(argv[3]);
		t_freqind = atoi(argv[4]);
		t_memctrl = atoi(argv[5]);

		len = strlen(argv[6]);
		out = (char*)malloc(len+1);
		if(input==NULL)
		{
			printf("allocation failed\n");
		}
		strcpy(out, argv[6]);
	} else {
		printf("invalid arguments\n");
		return -1;
	}
	
	execute_cpu(bench, input, t_ncpus, t_freqind, t_memctrl, out);
}				

/* ----------  end of function main  ---------- */


