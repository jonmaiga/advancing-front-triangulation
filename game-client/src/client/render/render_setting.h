#pragma once

#include "xgl/xgl.h"

namespace playchilla::internal {
inline void glEnableEx(GLenum state, bool enable) {
	if (enable) {
		gl_check(glEnable(state));
	}
	else {
		gl_check(glDisable(state));
	}
}
}

namespace playchilla {
struct bind_result {
	bool shader_program_bound = false;
	bool buffer_bound = false;
};

struct render_setting {
	GLuint program_id = {};
	GLuint buffer_id = {};

	bool blend = false;
	GLenum src_blend = GL_SRC_ALPHA;
	GLenum dst_blend = GL_ONE_MINUS_SRC_ALPHA;

	bool depth_test = true;
	GLboolean depth_write = true;
	GLenum depth_test_func = GL_LEQUAL;

	bool is_cull = true;
	GLenum cull_face = GL_BACK;

	bool stencil_test = false;

	GLenum draw_mode = GL_TRIANGLES;
	GLint draw_offset = 0;

	bind_result bind(const render_setting& rs) {
		bind_result result;

		draw_mode = rs.draw_mode;
		draw_offset = rs.draw_offset;

		if (rs.program_id != program_id) {
			program_id = rs.program_id;
			gl_check(glUseProgram(program_id));
			result.shader_program_bound = true;
		}

		if (rs.buffer_id != buffer_id) {
			buffer_id = rs.buffer_id;
			gl_check(glBindBuffer(GL_ARRAY_BUFFER, buffer_id));
			result.buffer_bound = true;
		}

		if (rs.blend != blend) {
			blend = rs.blend;
			internal::glEnableEx(GL_BLEND, blend);
		}
		if (blend) {
			if (rs.src_blend != src_blend || rs.dst_blend != dst_blend) {
				src_blend = rs.src_blend;
				dst_blend = rs.dst_blend;
				gl_check(glBlendFunc(src_blend, dst_blend));
			}
		}

		if (rs.depth_test != depth_test) {
			depth_test = rs.depth_test;
			internal::glEnableEx(GL_DEPTH_TEST, depth_test);
		}

		if (depth_test) {
			if (rs.depth_write != depth_write) {
				depth_write = rs.depth_write;
				gl_check(glDepthMask(depth_write));
			}
			if (rs.depth_test_func != depth_test_func) {
				depth_test_func = rs.depth_test_func;
				gl_check(glDepthFunc(depth_test_func));
			}
		}

		if (rs.is_cull != is_cull) {
			is_cull = rs.is_cull;
			internal::glEnableEx(GL_CULL_FACE, is_cull);
		}
		if (is_cull) {
			if (rs.cull_face != cull_face) {
				cull_face = rs.cull_face;
				gl_check(glCullFace(cull_face));
			}
		}

		if (rs.stencil_test != stencil_test) {
			stencil_test = rs.stencil_test;
			internal::glEnableEx(GL_STENCIL_TEST, stencil_test);
		}
		if (stencil_test) {
			if (rs.stencil_test != stencil_test) {
				gl_check(glStencilMaskSeparate(GL_FRONT, 0xff));
				gl_check(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_REPLACE));
			}
		}

		return result;
	}
};

inline void setup_gl_bindings(const render_setting& rs) {
	using namespace internal;
	gl_check(glUseProgram(rs.program_id));
	gl_check(glBindBuffer(GL_ARRAY_BUFFER, rs.buffer_id));

	glEnableEx(GL_BLEND, rs.blend);
	gl_check(glBlendFunc(rs.src_blend, rs.dst_blend));

	glEnableEx(GL_CULL_FACE, rs.is_cull);
	gl_check(glCullFace(rs.cull_face));

	glEnableEx(GL_DEPTH_TEST, rs.depth_test);
	gl_check(glDepthMask(rs.depth_write));
	gl_check(glDepthFunc(rs.depth_test_func));

	glEnableEx(GL_STENCIL_TEST, rs.stencil_test);
	gl_check(glStencilMaskSeparate(GL_FRONT, 0xff));
	gl_check(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_REPLACE));
}
}
