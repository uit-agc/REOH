#define _GNU_SOURCE
#include <sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <numa.h>

int set_cpu_cores_used(int ncores)
{
    char command[256];
    int retvalsyscall;
    
    sprintf(command, "ps -eLf | awk '(/%d/) && (!/awk/) {print $4}' | xargs -n1 taskset -p -c 0-%u",
            getpid(), ncores);
    printf("apply_cpu_config_taskset: Applying core allocation: %s\n", command);
    retvalsyscall = system(command);
    if (retvalsyscall != 0) {
      fprintf(stderr, "apply_cpu_config_taskset: ERROR running taskset: %d\n",
              retvalsyscall);
    }
}

int set_numa_cpu_node(int n)
{
    	// set NUMA zone binding to strict
    	numa_set_bind_policy(1);

	// Bind process to numa node n
        if (numa_run_on_node(n) != 0) {
                printf("could not assign current process to cpu node %d!\n", n);
        }
}


int set_numa_mem_node(int n)
{
    	//set NUMA zone binding to strict
    	numa_set_bind_policy(1);

    	struct bitmask *mask = numa_allocate_nodemask();
    	mask = numa_bitmask_setbit(mask, n);

	// Bind process to memory node n
    	numa_set_membind(mask);

	// Test whether memory binding was successful
	struct bitmask *mask_after = numa_get_membind();

	if(!numa_bitmask_equal(mask, mask_after))
                printf("could not assign current process to memory node %d!\n", n);

	//does the effect is removed after these commands?
	numa_free_nodemask(mask);
    numa_free_nodemask(mask_after);

        return 0;
}

void check_numa()
{
	if(numa_available() < 0){
                printf("System does not support NUMA API!\n");
        }
        int n = numa_max_node();
        printf("There are %d nodes on your system\n", n + 1);
}

unsigned int get_current_cpu_count() {
  unsigned int curr_cpu_count = 0;
  long i;
  cpu_set_t* cur_mask;
  long num_configured_cpus = sysconf(_SC_NPROCESSORS_CONF);
  if (num_configured_cpus < 0) {
    fprintf(stderr, "get_current_cpu_count: Failed to get the number of "
            "configured CPUs\n");
    return 0;
  }
  cur_mask = CPU_ALLOC(num_configured_cpus);
  if (cur_mask == NULL) {
    fprintf(stderr, "get_current_cpu_count: Failed to alloc cpu_set_t\n");
    return 0;
  }

  if (sched_getaffinity(getpid(), sizeof(cur_mask), cur_mask)) {
    fprintf(stderr, "get_current_cpu_count: Failed to get CPU affinity\n");
  } else {
    for (i = 0; i < num_configured_cpus; i++) {
      if (CPU_ISSET(i, cur_mask)) {
        curr_cpu_count++;
      }
    }
  }
  CPU_FREE(cur_mask);

  return curr_cpu_count;
}


/**
 * Compare the current CPU governor state with the provided one.
 * Returns -1 on failure.
 */
int cpu_governor_cmp(unsigned int cpu, char* governor) {
  FILE* fp;
  char buffer[128];
  int governor_cmp = -1;
  size_t len;

  sprintf(buffer,
          "/sys/devices/system/cpu/cpu%u/cpufreq/scaling_governor", cpu);
  fp = fopen(buffer, "r");
  if (fp == NULL) {
    fprintf(stderr, "cpu_governor_cmp: Failed to open %s\n", buffer);
    return -1;
  }

  if (fgets(buffer, sizeof(buffer), fp) != NULL) {
    // replace trailing newline
    len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    governor_cmp = strcmp(governor, buffer);
  }
  fclose(fp);

  return governor_cmp;
}

/**
 * Attempt to get the current frequency of the cpu.
 */
unsigned long get_current_cpu_frequency(unsigned int cpu) {
  FILE* fp;
  char buffer[128];
  unsigned long curr_freq = 0;

  sprintf(buffer,
          "/sys/devices/system/cpu/cpu%u/cpufreq/scaling_cur_freq", cpu);
  fp = fopen(buffer, "r");
  if (fp == NULL) {
    fprintf(stderr, "get_current_cpu_frequency: Failed to open %s\n", buffer);
    return 0;
  }

  if (fgets(buffer, sizeof(buffer), fp) != NULL) {
    curr_freq = strtoul(buffer, NULL, 0);
  }
  fclose(fp);

  return curr_freq;
}

void apply_cpu_freq(int cpu, unsigned long freq){
	char command[256];
	int retvalsyscall;
	
	sprintf(command,
            "echo %lu > /sys/devices/system/cpu/cpu%u/cpufreq/scaling_setspeed",
            freq,cpu);
    retvalsyscall = system(command);
    if (retvalsyscall != 0) {
      fprintf(stderr, "apply_cpu_freq: ERROR setting frequency for CPU %d with freq %lu: %d\n",
              cpu, freq, retvalsyscall);
    }
  
}

void apply_cpu_gov(int cpu, char *governor){
	char command[256];
	int retvalsyscall;
	
	sprintf(command,
            "echo %s > /sys/devices/system/cpu/cpu%u/cpufreq/scaling_governor",
            governor,cpu);
    retvalsyscall = system(command);
    if (retvalsyscall != 0) {
      fprintf(stderr, "apply_cpu_freq: ERROR setting governor for CPU %d with governor %s: %d\n",
              cpu, governor, retvalsyscall);
    }
  
}
