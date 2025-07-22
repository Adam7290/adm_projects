// TODO: Write tests!!!!!

#include "stream.h"

#include "panic.h"

#include <string.h>

void stream_write(stream_t* stream, const void* bytes, usize length) {
    PANIC_ASSERT_DEBUG(stream->_vtable->write_bytes != NULL, "This stream does not have write capabilities.");
    stream->_vtable->write_bytes(stream, bytes, length);
}

void stream_write_cstr(stream_t* stream, const char* cstr) {
    stream_write(stream, cstr, strlen(cstr));
}

void stream_write_char(stream_t* stream, char c) {
    stream_write(stream, &c, sizeof(char));
}

void* stream_user_data(stream_t* stream) {
    return stream->_user_data;
}
