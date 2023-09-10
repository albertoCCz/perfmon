#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "perfinfo.h"

#define WIN_WIDTH 275
#define WIN_HEIGHT 130
#define WIN_FACTOR 1

int main(void)
{

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

    size_t *mhz_records = (size_t*) calloc(100, sizeof(size_t));
    size_t max_mhz = 0;
    
    while (!WindowShouldClose()) {
        // pupulate info structs
        get_cpuinfo(&cpu_info);
        get_meminfo(&mem_info);

        // update records
        size_t n_mhz_records = 100;
        memmove(mhz_records, mhz_records + 1, (n_mhz_records - 1)*sizeof(typeof(*mhz_records)));
        mhz_records[n_mhz_records-1] = cpu_info.mhz;
        if (max_mhz < cpu_info.mhz) max_mhz = cpu_info.mhz;
        
        sprintf(model_name_txt, "model name: %s", cpu_info.model_name);
        sprintf(cpu_mhz_txt,    "cpu MHz (%zu processors): %.3f MHz", cpu_info.cpu_count, cpu_info.mhz);

        sprintf(mem_total_txt, "MemTotal:     %zu kB", mem_info.mem_total);
        sprintf(mem_free_txt,  "MemFree:      %zu kB", mem_info.mem_free);
        sprintf(mem_avail_txt, "MemAvailable: %zu kB", mem_info.mem_avail);

        int screen_height = GetScreenHeight();
        int screen_width  = GetScreenWidth();
        
        int graph_offset  = 2;
        int graph_thick   = 1;
        int graph_height  = screen_height - graph_offset - 65;
        int graph_width   = screen_width  - 2*graph_offset;
        
        Vector2 h_line_start = {graph_offset,               screen_height - graph_offset};
        Vector2 h_line_end   = {graph_offset + graph_width, screen_height - graph_offset};

        Vector2 v_line_start = {graph_offset, screen_height - graph_offset};
        Vector2 v_line_end   = {graph_offset, screen_height - graph_offset - graph_height};

        BeginDrawing();
        ClearBackground(BLACK);
            // draw statistics
            DrawText(model_name_txt, 2, 2 + 0*font_size, font_size, LIME);
            DrawText(cpu_mhz_txt,    2, 2 + 1*font_size, font_size, LIME);
            DrawText(mem_total_txt,  2, 2 + 2*font_size, font_size, LIME);
            DrawText(mem_free_txt,   2, 2 + 3*font_size, font_size, LIME);
            DrawText(mem_avail_txt,  2, 2 + 4*font_size, font_size, LIME);

            // draw mhz records
            for (size_t i = 0; i < n_mhz_records - 1; ++i) {
                float graph_segment_offset = (screen_width - 2*graph_offset) / (float) n_mhz_records;
                Vector2 segment_start = {
                    graph_segment_offset * i + graph_offset,
                    screen_height - graph_offset - (mhz_records[i] / (float) max_mhz * graph_height)
                };
                Vector2 segment_end = {
                    graph_segment_offset * (i+1) + graph_offset,
                    screen_height - graph_offset - (mhz_records[i+1] / (float) max_mhz * graph_height)
                };
                DrawLineEx(segment_start, segment_end, graph_thick, YELLOW);
            }

            // draw graph axis
            DrawLineEx(h_line_start, h_line_end, graph_thick, WHITE);
            DrawLineEx(v_line_start, v_line_end, graph_thick, WHITE);
            
        EndDrawing();
    }

    free(mhz_records);
    CloseWindow();
    
    return 0;
}
