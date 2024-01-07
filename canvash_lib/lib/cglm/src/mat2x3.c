/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "../include/cglm/cglm.h"
#include "../include/cglm/call.h"

CGLM_EXPORT
void
glmc_mat2x3_copy(mat2x3 mat, mat2x3 dest) {
    glm_mat2x3_copy(mat, dest);
}

CGLM_EXPORT
void
glmc_mat2x3_zero(mat2x3 mat) {
    glm_mat2x3_zero(mat);
}

CGLM_EXPORT
void
glmc_mat2x3_make(float *__restrict src, mat2x3 dest) {
    glm_mat2x3_make(src, dest);
}

CGLM_EXPORT
void
glmc_mat2x3_mul(mat2x3 m1, mat3x2 m2, mat2 dest) {
    glm_mat2x3_mul(m1, m2, dest);
}

CGLM_EXPORT
void
glmc_mat2x3_mulv(mat2x3 m, vec3 v, vec2 dest) {
    glm_mat2x3_mulv(m, v, dest);
}

CGLM_EXPORT
void
glmc_mat2x3_transpose(mat2x3 m, mat3x2 dest) {
    glm_mat2x3_transpose(m, dest);
}

CGLM_EXPORT
void
glmc_mat2x3_scale(mat2x3 m, float s) {
    glm_mat2x3_scale(m, s);
}
