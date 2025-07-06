#pragma once

typedef struct stream_t stream_t;

void console_print(const char* cstr_or_format, ...);
void console_println(const char* cstr_or_format, ...);

// Returns a stream that writes to stdout
stream_t console_stdout_stream();
stream_t console_stderr_stream();