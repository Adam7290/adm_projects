#pragma once

typedef struct dbgtext_t dbgtext_t;
typedef struct sprite_batch_t sprite_batch_t;
typedef struct arena_t arena_t;
typedef struct stream_t stream_t;

dbgtext_t* dbgtext_new(sprite_batch_t* sprite_batch, arena_t* arena);
// Draws text to screen and flushes buffer
void dbgtext_flush(dbgtext_t* dbgtext);
// Creates a stream to write debug text to screen 
stream_t dbgtext_stream(dbgtext_t* dbgtext);
// Draw text to screen without newline
void dbgtext_print(dbgtext_t* dbgtext, const char* cstr_or_format, ...); 
// Draw text to screen with newline
void dbgtext_println(dbgtext_t* dbgtext, const char* cstr_or_format, ...); 
