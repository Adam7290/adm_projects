// A linear algebra library inspired by linmath.h, glm, and System.Numerics

#pragma once

#include <adm_utils/util.h>
#include <math.h>

#define deg_to_rad(x) (x * 0.017453292519943295)
#define rad_to_deg(x) (x * 57.29577951308232)

#define _GTYPES_DEFINE_VEC(dims, type, prefix, ...) \
typedef struct type { \
	union { \
		float array[dims]; \
		struct { \
			__VA_ARGS__ \
		}; \
	}; \
} type; \
 \
static inline type prefix##_add(const type* v0, const type* v1) { \
	type ret; \
	for (int i = 0; i < dims; i++) { \
		ret.array[i] = v0->array[i] + v1->array[i]; \
	} \
	return ret; \
} \
 \
static inline type prefix##_sub(const type* v0, const type* v1) { \
	type ret; \
	for (int i = 0; i < dims; i++) { \
		ret.array[i] = v0->array[i] - v1->array[i]; \
	} \
	return ret; \
} \
 \
static inline type prefix##_mul(const type* v0, const type* v1) { \
	type ret; \
	for (int i = 0; i < dims; i++) { \
		ret.array[i] = v0->array[i] * v1->array[i]; \
	} \
	return ret; \
} \
 \
static inline type prefix##_div(const type* v0, const type* v1) { \
	type ret; \
	for (int i = 0; i < dims; i++) { \
		ret.array[i] = v0->array[i] / v1->array[i]; \
	} \
	return ret; \
} \
 \
static inline type prefix##_div_scalar(const type* v0, float scalar) { \
	type ret; \
	for (int i = 0; i < dims; i++) { \
		ret.array[i] = v0->array[i] / scalar; \
	} \
	return ret; \
} \
 \
static inline type prefix##_mul_scalar(const type* v0, float scalar) { \
	type ret; \
	for (int i = 0; i < dims; i++) { \
		ret.array[i] = v0->array[i] * scalar; \
	} \
	return ret; \
}

_GTYPES_DEFINE_VEC(2, vec2f_t, vec2, float x, y;);
_GTYPES_DEFINE_VEC(3, vec3f_t, vec3, float x, y, z;);
_GTYPES_DEFINE_VEC(4, vec4f_t, vec4, float x, y, z, w;);
_GTYPES_DEFINE_VEC(4, color4f_t, color4f, float r, g, b, a;);

typedef struct color4b_t {
	union {
		u8 array[4];
		u64 hex;
		struct { u8 r, g, b, a; };
	};
} color4b_t;

static inline color4f_t color4b_to_color4f(const color4b_t* b) {
	return (color4f_t){ b->r / 255.f, b->g / 255.f, b->b / 255.f, b->a / 255.f };
}

static inline color4b_t color4f_to_color4b(const color4f_t* f) {
	return (color4b_t){ f->r * 255, f->g * 255, f->b * 255, f->a * 255 };
}

typedef struct mat4x4_t {
	union {
		float array[4*4];
		float array2d[4][4];
	};
} mat4x4_t;

// The equivalent to 1.0 but for matrices
static inline mat4x4_t mat4x4_identity() {
	return (mat4x4_t){
		.array = {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		},
	};
}

// Combines two matrices together
static inline void mat4x4_mul(mat4x4_t* m0, const mat4x4_t* m1) {
	mat4x4_t tmp;
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			float sum = 0;
			for (int i = 0; i < 4; i++) {
				sum += m0->array2d[x][i] * m1->array2d[i][y];
			}
			tmp.array2d[x][y] = sum;
		}
	}
	*m0 = tmp;
}

// Translate a matrix
static inline void mat4x4_translate(mat4x4_t* mat, const vec3f_t* translation) {
	mat4x4_mul(mat, &(mat4x4_t){
		.array = {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			translation->x, translation->y, translation->z, 1.f,
		},
	});
}

// Matrix that scales a point
static inline void mat4x4_scale(mat4x4_t* mat, const vec3f_t* scale) {
	mat4x4_mul(mat, &(mat4x4_t){
		.array = {
			scale->x, 0.f, 0.f, 0.f,
			0.f, scale->y, 0.f, 0.f,
			0.f, 0.f, scale->z, 0.f,
			0.f, 0.f, 0.f, 1.f,
		},
	});
}

// Matrix that rotates a point on the z axis
static inline void mat4x4_rotate_z(mat4x4_t* mat, float z) {
	float s = sin(z);
	float c = cos(z);
	mat4x4_mul(mat, &(mat4x4_t){
		.array = {
			c, s, 0.f, 0.f,
			-s, c, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		},
	});
}

// Matrix that converts a point in pixels (ex: 100, 200) into normalized opengl coodinates (ex: 0.6, 0.7)
static inline mat4x4_t mat4x4_orthographic(float left, float right, float bottom, float top, float near, float far) {
	return (mat4x4_t){
		.array = {
			2/(right-left), 0.f, 0.f, 0.f,
			0.f, 2/(top-bottom), 0.f, 0.f,
			0.f, 0.f, -2/(far-near), 0.f,
			-((right+left)/(right-left)), -((top+bottom)/(top-bottom)), -((far+near)/(far-near)), 1.f,
		},
	};
}

#undef _GTYPES_DEFINE_VEC
