#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "perfinfo.h"

bool is_letter(char c)
{
    return 'a' <= c && c <= 'z' ||
        'A' <= c && c <= 'Z';
}

bool is_number(char c)
{
    return '0' <= c && c <= '9';
}

bool is_valid_symbol(char c)
{
    return c == '(' || c == ')' || c == '@' || c == '_' || c == '.' || c == ',';
}

bool is_valid_char(char c)
{
    return is_letter(c) || is_number(c) || is_valid_symbol(c);
}

bool is_space(char c)
{
    return c == ' ' || c == '\t';
}

void parse_line(const char *line, char *p_buff, char *v_buff)
{
    size_t p_cur = 0, v_cur = 0;
    bool p_is_parsed = false;
    for (size_t i = 0; line[i] != '\0'; ++i) {
        if (line[i] == ':' || (p_cur == 0 && !is_valid_char(line[i]))) {  // parameter is parsed
            p_is_parsed = true;
            p_buff[p_cur] = '\0';
        } else if (is_space(line[i]) && (i > 0 ? is_valid_char(line[i-1]) : false) && is_valid_char(line[i+1])) { // space between valid chars
            if (!p_is_parsed) {
                p_buff[p_cur] = ' ';
                ++p_cur;
            } else {
                v_buff[v_cur]= ' ';
                ++v_cur;
            }
        } else if (is_valid_char(line[i])) {
            if (!p_is_parsed) {
               p_buff[p_cur] = line[i];
                ++p_cur;
            } else {
                v_buff[v_cur]= line[i];
                ++v_cur;
            }
        }
    }
    v_buff[v_cur] = '\0';
}

void get_cpuinfo(CPUINFO *cpu_info)
{
    const char *cpuinfo_path = "/proc/cpuinfo";
    FILE *cpuinfo = fopen(cpuinfo_path, "r");

    char line[1024]  = { 0 };
    char param[1024] = { 0 };
    char value[1024] = { 0 };

    double cpu_mhz = 0;
    size_t cpu_count = 0;
    
    while (fgets(line, ARRAY_LEN(line), cpuinfo) != NULL) {
        parse_line(line, param, value);

        if (strcmp(param, "model name") == 0) {
            for (size_t j = 0; value[j] != '\0'; ++j) {
                cpu_info->model_name[j] = value[j];
            }
        }
        
        if (strcmp(param, "cpu MHz") == 0) {
            cpu_count += 1;
            cpu_mhz += strtod(value, NULL);
        }
    }

    fclose(cpuinfo);
    
    cpu_info->cpu_count = cpu_count;
    cpu_info->mhz = cpu_mhz / cpu_count;
}

void get_meminfo(MEMINFO *mem_info)
{
    const char *meminfo_path = "/proc/meminfo";
    FILE *meminfo = fopen(meminfo_path, "r");

    char line[1024]  = { 0 };
    char param[1024] = { 0 };
    char value[1024] = { 0 };

    while (fgets(line, ARRAY_LEN(line), meminfo) != NULL) {
        parse_line(line, param, value);

        if (strcmp(param, "MemTotal") == 0) {
            mem_info->mem_total = atoi(value);
        } else if (strcmp(param, "MemFree") == 0) {
            mem_info->mem_free = atoi(value);
        } else if (strcmp(param, "MemAvailable") == 0) {
            mem_info->mem_avail = atoi(value);
        }
    }
    fclose(meminfo);
}
