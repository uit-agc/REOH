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

int main ( int argc, char *argv[] )
{
	//reading the input_filename
	char* input;
	char *bench;
	char *WG;
	char *out;

	if (argc==5)
	{
		size_t len = strlen(argv[1]);
		bench = (char*)malloc(len+1);
		strcpy(bench, argv[1]);

		len = strlen(argv[2]);
		input = (char*)malloc(len+1);
		strcpy(input, argv[2]);
		printf("input: %s\n", input );

		len = strlen(argv[3]);
		WG = (char*)malloc(len+1);
		strcpy(WG, argv[3]);

		len = strlen(argv[4]);
		out = (char*)malloc(len+1);
		strcpy(out, argv[4]);

	}

	execute(bench, input, WG, out);	
	/* ----------  end of function main  ---------- */			
}				


