#include <adm_framework/app.h>
#include <adm_framework/input.h>
#include <adm_framework/gpu.h>
#include <adm_framework/time.h>

#include <adm_utils/arena.h>
#include <adm_utils/console.h>

#include <stdio.h>
#include <math.h>

typedef struct Vertex {
    float x, y;
    float r, g, b;
} Vertex;

int main() {
    arena_t arena = arena_new();
    app_t* app = app_new(&arena, &(app_def_t){
        .title = "Test Window",
        .window_size = (app_window_size_t){ 1280, 720 },
        .debug = true,
    });

    gpu_vert_decl_t decl = gpu_vert_decl_new(app, 2,
        (gpu_vert_attr_t){ 2, GPU_VERT_ATTR_TYPE_FLOAT }, // Position
        (gpu_vert_attr_t){ 3, GPU_VERT_ATTR_TYPE_FLOAT } // Color
    );

    Vertex vertices[] = {
        { -0.5f, -0.5f, 1.0f, 0.0f, 0.0f },
        {  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
        {  0.0f,  0.5f, 0.0f, 0.0f, 1.0f },
    };

    const char* vertex_shader_source = 
        "#version 460 core\n"
        "layout (location = 0) in vec2 pos;\n"
        "layout (location = 1) in vec3 color;\n"
        "uniform float v;\n"
        "out vec3 vertex_color;\n"
        "void main() {\n"
        "   vertex_color = color;\n"
        "   gl_Position = vec4(pos.x * v, pos.y * v, 0.0, 1.0);\n"
        "}\0";

    const char* fragment_shader_source =
        "#version 460 core\n"
        "in vec3 vertex_color;\n"
        "out vec4 out_color;\n"
        "void main() {\n"
        "   out_color = vec4(vertex_color.xyz, 1.0);\n"
        "}\0";

    gpu_verts_t* mesh = gpu_verts_create(app, &arena, &decl);
    gpu_verts_upload(mesh, vertices, sizeof(vertices));

    gpu_shader_t* shader = gpu_shader_create(app, &arena);
    gpu_shader_upload_source(shader, vertex_shader_source, fragment_shader_source);

    app_show(app);
    while (app_frame(app) == true) {
        gpu_clear(app, input_key_down(app, INPUT_KEY_SPACE) ? COLOR_WHITE : COLOR_BLACK);

        gpu_shader_set_f32(shader, "v", sinf(time_app_elapsed(app).time));
        gpu_shader_use(shader);
        gpu_verts_draw(mesh);

        printf("Elapsed: %f, Delta: %f, FPS: %u\n", time_app_elapsed(app).time, time_app_delta(app).time, app_frames_per_second(app));
    }

    arena_destroy(&arena);
}