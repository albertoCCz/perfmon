#include <stdio.h>

#include <raylib.h>

#include "perfinfo.h"

#define WIN_WIDTH 275
#define WIN_HEIGHT 65
#define WIN_FACTOR 1

int main(void)
{

    SetWindowState(FLAG_WINDOW_TRANSPARENT);
    InitWindow(WIN_WIDTH*WIN_FACTOR, WIN_HEIGHT*WIN_FACTOR, "Performance Monitoring");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(15);

    CPUINFO cpu_info;
    MEMINFO mem_info;

    char model_name_txt[100];
    char cpu_mhz_txt[100];
    char mem_total_txt[100];
    char mem_free_txt[100];
    char mem_avail_txt[100];

    int font_size = 10;
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLANK);
            get_cpuinfo(&cpu_info);
            get_meminfo(&mem_info);

            sprintf(model_name_txt, "model name: %s", cpu_info.model_name);
            sprintf(cpu_mhz_txt,    "cpu MHz (%zu processors): %.3f MHz", cpu_info.cpu_count, cpu_info.mhz);

            sprintf(mem_total_txt, "MemTotal:     %zu kB", mem_info.mem_total);
            sprintf(mem_free_txt,  "MemFree:      %zu kB", mem_info.mem_free);
            sprintf(mem_avail_txt, "MemAvailable: %zu kB", mem_info.mem_avail);
            
            DrawText(model_name_txt, 2, 2 + 0*font_size, font_size, LIME);
            DrawText(cpu_mhz_txt,    2, 2 + 1*font_size, font_size, LIME);
            DrawText(mem_total_txt,  2, 2 + 2*font_size, font_size, LIME);
            DrawText(mem_free_txt,   2, 2 + 3*font_size, font_size, LIME);
            DrawText(mem_avail_txt,  2, 2 + 4*font_size, font_size, LIME);
        EndDrawing();
    }

    CloseWindow();
    
    return 0;
}
