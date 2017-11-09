#ifndef cpu_lib_h
#define cpu_lib_h

//static unsigned long available_freqs[] = {1801000, 1800000, 1700000, 1600000, 1500000, 1400000, 1300000, 1200000};
static unsigned long available_freqs[] = {1200000, 1300000, 1400000, 1500000, 1600000, 1700000, 1800000, 1801000};
static char* available_govs[] = {"conservative", "userspace", "powersave", "ondemand", "performance"};

unsigned int get_current_cpu_count();
int cpu_governor_cmp(unsigned int cpu, char* governor);
unsigned long get_current_cpu_frequency(unsigned int cpu);
void apply_cpu_freq(int cpu, unsigned long freq);
void apply_cpu_gov(int cpu, char *governor);

int set_cpu_cores_used(int ncores);
int set_numa_cpu_node(int n);
int set_numa_mem_node(int n);
void check_numa();


#endif
