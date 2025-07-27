#pragma once

typedef struct app_t app_t;
typedef struct stream_t stream_t;
typedef struct dbgtext_t dbgtext_t;

void _dbgtext_init(app_t* app); 
void _dbgtext_frame(dbgtext_t* dbgtext);

// Creates a stream to write debug text to screen 
stream_t dbgtext_stream(app_t* app);
// Draw text to screen without newline
void dbgtext_print(app_t* app, const char* cstr_or_format, ...); 
// Draw text to screen with newline
void dbgtext_println(app_t* app, const char* cstr_or_format, ...); 
