#include <stdbool.h>

#define ARRAY_LEN(a) sizeof((a))/sizeof((a)[0])

bool is_letter(char c);
bool is_number(char c);
bool is_valid_symbol(char c);
bool is_valid_char(char c);
bool is_space(char c);

void parse_line(const char *line, char *p_buff, char *v_buff);

typedef struct {
    char model_name[100];
    double mhz;           // MHz
    size_t cpu_count;
} CPUINFO;
void get_cpuinfo(CPUINFO *cpu_info);

typedef struct {
    size_t mem_total; // kB
    size_t mem_free;  // kB
    size_t mem_avail; // kB
} MEMINFO;
void get_meminfo(MEMINFO *mem_info);
