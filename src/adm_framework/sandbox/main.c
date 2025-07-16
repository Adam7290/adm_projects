#include <adm_framework/app.h>
#include <adm_framework/input.h>
#include <adm_framework/gpu.h>
#include <adm_framework/time.h>
#include <adm_framework/image.h>

#include <adm_utils/arena.h>
#include <adm_utils/console.h>

#include <adm_framework/external/glad/include/glad/glad.h>

#include <stdio.h>
#include <math.h>

typedef struct vertex_t {
    float x, y;
	float u, v;
    float r, g, b;
} vertex_t;

int main() {
    arena_t arena = arena_new();
    app_t* app = app_new(&arena, &(app_def_t){
        .title = "Test Window",
        .window_size = (app_window_size_t){ 1280, 720 },
        .debug = true,
    });

    gpu_vert_decl_t decl = gpu_vert_decl_new(app, 2,
        (gpu_vert_attr_t){ 2, GPU_VERT_ATTR_TYPE_FLOAT }, // Position
        (gpu_vert_attr_t){ 2, GPU_VERT_ATTR_TYPE_FLOAT }, // Texture Coord 
        (gpu_vert_attr_t){ 3, GPU_VERT_ATTR_TYPE_FLOAT } // Color
    );

    vertex_t vertices[] = {
		// x      y      u     v      r     g     b
        {  0.5f,  0.5f,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f, }, // Top-right
        {  0.5f, -0.5f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f, }, // Bottom-right
		{ -0.5f, -0.5f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, }, // Bottom-left
        { -0.5f,  0.5f,  0.0f, 1.0f,  1.0f, 1.0f, 1.0f, }, // Top-left
    };

	gpu_index_t indices[] = {
		0, 1, 3,
		1, 2, 3,
	};

    const char* vertex_shader_source = 
        "#version 460 core\n"
        "layout (location = 0) in vec2 pos;\n"
		"layout (location = 1) in vec2 uv;\n"
        "layout (location = 2) in vec3 color;\n"
        "uniform float v;\n"
        "out vec3 vertex_color;\n"
		"out vec2 vertex_uv;\n"
        "void main() {\n"
        "   vertex_color = color;\n"
		"	vertex_uv = uv;\n"
        "   gl_Position = vec4(pos.x * v, pos.y * v, 0.0, 1.0);\n"
        "}\0";

    const char* fragment_shader_source =
        "#version 460 core\n"
        "in vec3 vertex_color;\n"
		"in vec2 vertex_uv;\n"
		"uniform sampler2D texture0;\n"
        "out vec4 out_color;\n"
        "void main() {\n"
        "   out_color = texture(texture0, vertex_uv) * vec4(vertex_color.xyz, 1.0);\n"
        "}\0";

    gpu_verts_t* mesh = gpu_verts_create(app, &arena, &decl, true);
    gpu_verts_upload(mesh, vertices, sizeof(vertices));
	gpu_verts_upload_indices(mesh, indices, sizeof(indices));

    gpu_shader_t* shader = gpu_shader_create(app, &arena);
    gpu_shader_upload_source(shader, vertex_shader_source, fragment_shader_source);

	image_t image = image_create_from_file(&arena, "assets/test.png");
	gpu_texture_t* texture = gpu_texture_create(app, &arena);
	gpu_texture_upload(texture, &image);

    app_show(app);
    while (app_frame(app) == true) {
        gpu_clear(app, input_key_down(app, INPUT_KEY_SPACE) ? COLOR_WHITE : COLOR_BLACK);
	    
        gpu_shader_use(shader);
        gpu_shader_set_float(shader, "v", sinf(time_app_elapsed(app).time));
		gpu_texture_use(texture, 0);
        gpu_shader_set_int(shader, "texture0", 0); 
        gpu_verts_draw(mesh);

        console_println(
			"Elapsed: {}, Delta: {}, FPS: {}", 
			FORMAT(float, time_app_elapsed(app).time), 
			FORMAT(float, time_app_delta(app).time), 
			FORMAT(int, app_frames_per_second(app))
		);

		if (input_key_down(app, INPUT_KEY_ESCAPE) == true) {
			app_exit(app);	
		}
    }

    arena_destroy(&arena);
}
