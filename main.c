#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <raylib.h>

#include "perfinfo.h"

#define FPS 60

#define WIN_WIDTH 275
#define WIN_HEIGHT 130
#define WIN_FACTOR 1

typedef struct {
    int graph_offset;  
    int graph_thick;   
    int graph_height;  
    int graph_width;   
        
    Vector2 h_line_start;
    Vector2 h_line_end;  
    Vector2 v_line_start;
    Vector2 v_line_end;  
} Graph;

void graph_setup(Graph *graph, int screen_width, int screen_height) {
    graph->graph_offset  = 2;
    graph->graph_thick   = 1;
    graph->graph_height = screen_height - graph->graph_offset - 65;
    graph->graph_width  = screen_width  - 2*graph->graph_offset;
    
    graph->h_line_start = (Vector2) {graph->graph_offset,                      screen_height - graph->graph_offset};
    graph->h_line_end   = (Vector2) {graph->graph_offset + graph->graph_width, screen_height - graph->graph_offset};

    graph->v_line_start = (Vector2) {graph->graph_offset, screen_height - graph->graph_offset};
    graph->v_line_end   = (Vector2) {graph->graph_offset, screen_height - graph->graph_offset - graph->graph_height};
}

int main(int argc, char **argv)
{
    // parse input args
    int refresh_time;
    if (argc < 2) {
        refresh_time = 75;
    } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("perfmon - Super simple cpu and memory performance(perf) monitor(mon) in C\n");
        printf("USAGE:  ./perfmon [refresh_time]\n");
        printf("        refresh_time     Time in ms at which to refresh the statistics\n");
        return 0;
    } else {
        refresh_time = atoi(argv[1]);
        if (refresh_time == 0) {
            fprintf(stderr, "ERROR: 'refresh_time' can't be zero.\n");
            exit(-1);
        }
    }

    InitWindow(WIN_WIDTH*WIN_FACTOR, WIN_HEIGHT*WIN_FACTOR, "Performance Monitoring");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(FPS);

    int screen_height = GetScreenHeight();
    int screen_width  = GetScreenWidth();
    
    CPUINFO cpu_info;
    MEMINFO mem_info;

    char model_name_txt[100];
    char cpu_mhz_txt[100];
    char mem_total_txt[100];
    char mem_free_txt[100];
    char mem_avail_txt[100];

    int font_size = 10;

    // initialise records buffer
    size_t n_mhz_records = 100;
    size_t *mhz_records = (size_t*) calloc(n_mhz_records, sizeof(size_t));
    size_t max_mhz = 0;

    // set up graph
    Graph graph;
    graph_setup(&graph, screen_width, screen_height);

    struct timespec start_time, current_time;
    timespec_get(&start_time, TIME_UTC);

    // set up UI mask
    RenderTexture2D target = LoadRenderTexture(screen_width, screen_height);
    Shader shader = LoadShader(0, "./shaders/bw.fs");
    int APPLY_SHADER = 0;

    while (!WindowShouldClose()) {
        // pupulate info structs
        timespec_get(&current_time, TIME_UTC);
        if ((current_time.tv_sec - start_time.tv_sec) * 1000 +
            (current_time.tv_nsec - start_time.tv_nsec) / 1000000 >= refresh_time)
        {
            start_time = current_time;
            get_cpuinfo(&cpu_info);
            get_meminfo(&mem_info);

            // update records
            memmove(mhz_records, mhz_records + 1, (n_mhz_records - 1)*sizeof(typeof(*mhz_records)));
            mhz_records[n_mhz_records-1] = cpu_info.mhz;
            if (max_mhz < cpu_info.mhz) max_mhz = cpu_info.mhz;
        }

        // build statistic strings
        sprintf(model_name_txt, "model name: %s", cpu_info.model_name);
        sprintf(cpu_mhz_txt,    "cpu MHz (%zu processors): %.3f MHz", cpu_info.cpu_count, cpu_info.mhz);

        sprintf(mem_total_txt, "MemTotal:     %zu kB", mem_info.mem_total);
        sprintf(mem_free_txt,  "MemFree:      %zu kB", mem_info.mem_free);
        sprintf(mem_avail_txt, "MemAvailable: %zu kB", mem_info.mem_avail);

        if (IsWindowResized()) {
            screen_width  = GetScreenWidth();
            screen_height = GetScreenHeight();
            graph_setup(&graph, screen_width, screen_height);

            UnloadTexture(target.texture);
            target = LoadRenderTexture(screen_width, screen_height);
        }
        
        BeginTextureMode(target);
            ClearBackground(BLACK);
            // draw statistics
            DrawText(model_name_txt, 2, 2 + 0*font_size, font_size, LIME);
            DrawText(cpu_mhz_txt,    2, 2 + 1*font_size, font_size, LIME);
            DrawText(mem_total_txt,  2, 2 + 2*font_size, font_size, LIME);
            DrawText(mem_free_txt,   2, 2 + 3*font_size, font_size, LIME);
            DrawText(mem_avail_txt,  2, 2 + 4*font_size, font_size, LIME);

            // draw mhz records
            for (size_t i = 0; i < n_mhz_records - 1; ++i) {
                float graph_segment_offset = (screen_width - 2*graph.graph_offset) / (float) n_mhz_records;
                Vector2 segment_start = {
                    graph_segment_offset * i + graph.graph_offset,
                    screen_height - graph.graph_offset - (mhz_records[i] / (float) max_mhz * graph.graph_height)
                };
                Vector2 segment_end = {
                    graph_segment_offset * (i+1) + graph.graph_offset,
                    screen_height - graph.graph_offset - (mhz_records[i+1] / (float) max_mhz * graph.graph_height)
                };
                DrawLineEx(segment_start, segment_end, graph.graph_thick, YELLOW);
            }

            // draw graph axis
            DrawLineEx(graph.h_line_start, graph.h_line_end, graph.graph_thick, WHITE);
                DrawLineEx(graph.v_line_start, graph.v_line_end, graph.graph_thick, WHITE);            
        EndTextureMode();

        // draw texture to screen
        BeginDrawing();
            ClearBackground(RAYWHITE);
            if (APPLY_SHADER) {
                BeginShaderMode(shader);
                    DrawTextureRec(target.texture, (Rectangle) {0, 0, target.texture.width, -target.texture.height}, (Vector2) {0.0f, 0.0f}, WHITE);
                EndShaderMode();
            } else {
                    DrawTextureRec(target.texture, (Rectangle) {0, 0, target.texture.width, -target.texture.height}, (Vector2) {0.0f, 0.0f}, WHITE);
            }
        EndDrawing();

        // input
        if (IsKeyPressed(KEY_SPACE)) APPLY_SHADER = (APPLY_SHADER + 1) % 2;
        
    }

    free(mhz_records);
    
    UnloadTexture(target.texture);
    UnloadShader(shader);
    CloseWindow();
    
    return 0;
}
