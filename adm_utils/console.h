#pragma once

typedef struct stream_t stream_t;

// Write to stdout
void console_print(const char* cstr_or_format, ...);
// Write to stdout with an added newline
void console_println(const char* cstr_or_format, ...);

// Returns a stream that writes to stdout
stream_t console_stdout_stream();
// Returns a stream that writes to stderr
stream_t console_stderr_stream();
