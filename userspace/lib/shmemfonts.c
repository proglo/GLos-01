/* This file is part of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2012-2014 Kevin Lange
 */
/* vim: tabstop=4 shiftwidth=4 noexpandtab
 *
 *
 * Shared-memory font management and access library.
 *
 */

#include <stdint.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

#include "yutani.h"
#include "graphics.h"
#include "shmemfonts.h"
#include "utf8decode.h"

static FT_Library   library;
static FT_Face      faces[FONTS_TOTAL]; /* perhaps make this an array ? */
static FT_GlyphSlot slot;
static FT_UInt      glyph_index;
static int initialized = 0;
static int _font_size = 12;
static int selected_face = 0;

#define SGFX(CTX,x,y,WIDTH) *((uint32_t *)&CTX[((WIDTH) * (y) + (x)) * 4])
#define FONT_SIZE 12

static int fallbacks[] = {FONT_JAPANESE, FONT_SYMBOLA, -1};

/*
 * XXX: take font name as an argument / allow multiple fonts
 */
static void _load_font(int i, char * name) {
	char * font;
	size_t s = 0;
	int error;
	char tmp[100];
	snprintf(tmp, 100, "sys.%s%s", getenv("DISPLAY"), name);

	font = (char *)syscall_shm_obtain(tmp, &s);
	error = FT_New_Memory_Face(library, font, s, 0, &faces[i]);
	error = FT_Set_Pixel_Sizes(faces[i], FONT_SIZE, FONT_SIZE);
}

static void _load_font_f(int i, char * path) {
	int error;
	error = FT_New_Face(library, path, 0, &faces[i]);
	error = FT_Set_Pixel_Sizes(faces[i], FONT_SIZE, FONT_SIZE);
}

static void _load_fonts() {
	_load_font(FONT_SANS_SERIF,             ".fonts.sans-serif");
	_load_font(FONT_SANS_SERIF_BOLD,        ".fonts.sans-serif.bold");
	_load_font(FONT_SANS_SERIF_ITALIC,      ".fonts.sans-serif.italic");
	_load_font(FONT_SANS_SERIF_BOLD_ITALIC, ".fonts.sans-serif.bolditalic");
	_load_font(FONT_MONOSPACE,              ".fonts.monospace");
	_load_font(FONT_MONOSPACE_BOLD,         ".fonts.monospace.bold");
	_load_font(FONT_MONOSPACE_ITALIC,       ".fonts.monospace.italic");
	_load_font(FONT_MONOSPACE_BOLD_ITALIC,  ".fonts.monospace.bolditalic");
	_load_font_f(FONT_JAPANESE, "/usr/share/fonts/VLGothic.ttf");
	_load_font_f(FONT_SYMBOLA, "/usr/share/fonts/Symbola.ttf");
}

void init_shmemfonts() {
	if (!initialized) {
		FT_Init_FreeType(&library);
		_load_fonts();
		selected_face = FONT_SANS_SERIF;
		initialized = 1;
	}
}

void set_font_size(int size) {
	for (int i = 0; i < FONTS_TOTAL; ++i) {
		FT_Set_Pixel_Sizes(faces[i], size, size);
	}
}

void set_font_face(int face_num) {
	selected_face = face_num;
}

char * shmem_font_name(int i) {
	return ((FT_FaceRec *)faces[i])->family_name;
}

/*
 * Draw a character to a context.
 */
static void draw_char(FT_Bitmap * bitmap, int x, int y, uint32_t fg, gfx_context_t * ctx) {
	int i, j, p, q;
	int x_max = x + bitmap->width;
	int y_max = y + bitmap->rows;
	for (j = y, q = 0; j < y_max; j++, q++) {
		for ( i = x, p = 0; i < x_max; i++, p++) {
			uint32_t a = _ALP(fg);
			a = (a * bitmap->buffer[q * bitmap->width + p]) / 255;
			uint32_t tmp = premultiply(rgba(_RED(fg),_GRE(fg),_BLU(fg),a));
			SGFX(ctx->backbuffer,i,j,ctx->width) = alpha_blend_rgba(SGFX(ctx->backbuffer,i,j,ctx->width),tmp);
		}
	}
}

uint32_t draw_string_width(char * string) {
	slot = faces[selected_face]->glyph;
	int pen_x = 0, i = 0;
	int error;

	uint8_t * s = string;

	uint32_t codepoint;
	uint32_t state = 0;

	while (*s) {
		uint32_t o = 0;
		while (*s) {
			if (!decode(&state, &codepoint, (uint8_t)*s)) {
				o = (uint32_t)codepoint;
				s++;
				goto finished_width;
			} else if (state == UTF8_REJECT) {
				state = 0;
			}
			s++;
		}

finished_width:
		if (!o) continue;

		FT_UInt glyph_index;

		glyph_index = FT_Get_Char_Index( faces[selected_face], o);
		if (glyph_index) {
			error = FT_Load_Glyph(faces[selected_face], glyph_index, FT_LOAD_DEFAULT);
			if (error) {
				fprintf(stderr, "Error loading glyph for '%d'\n", o);
				continue;
			}
			slot = (faces[selected_face])->glyph;
		} else {
			int i = 0;
			while (!glyph_index && fallbacks[i] != -1) {
				int fallback = fallbacks[i++];
				glyph_index = FT_Get_Char_Index( faces[fallback], o);
				error = FT_Load_Glyph(faces[fallback], glyph_index, FT_LOAD_DEFAULT);
				if (error) {
					fprintf(stderr, "Error loading glyph for '%d'\n", o);
					continue;
				}
				slot = (faces[fallback])->glyph;
			}
		}
		pen_x += slot->advance.x >> 6;
	}
	return pen_x;
}

void draw_string(gfx_context_t * ctx, int x, int y, uint32_t fg, char * string) {
	slot = faces[selected_face]->glyph;
	int pen_x = x, pen_y = y, i = 0;
	int error;

	uint8_t * s = string;

	uint32_t codepoint;
	uint32_t state = 0;

	while (*s) {
		uint32_t o = 0;
		while (*s) {
			if (!decode(&state, &codepoint, (uint8_t)*s)) {
				o = (uint32_t)codepoint;
				s++;
				goto finished;
			} else if (state == UTF8_REJECT) {
				state = 0;
			}
			s++;
		}

finished:
		if (!o) continue;

		FT_UInt glyph_index;

		glyph_index = FT_Get_Char_Index( faces[selected_face], o);
		if (glyph_index) {
			error = FT_Load_Glyph(faces[selected_face], glyph_index, FT_LOAD_DEFAULT);
			if (error) {
				fprintf(stderr, "Error loading glyph for '%d'\n", o);
				continue;
			}
			slot = (faces[selected_face])->glyph;
			if (slot->format == FT_GLYPH_FORMAT_OUTLINE) {
				error = FT_Render_Glyph((faces[selected_face])->glyph, FT_RENDER_MODE_NORMAL);
				if (error) {
					fprintf(stderr, "Error rendering glyph for '%d'\n", o);
					continue;
				}
			}
		} else {
			int i = 0;
			while (!glyph_index && fallbacks[i] != -1) {
				int fallback = fallbacks[i++];
				glyph_index = FT_Get_Char_Index( faces[fallback], o);
				error = FT_Load_Glyph(faces[fallback], glyph_index, FT_LOAD_DEFAULT);
				if (error) {
					fprintf(stderr, "Error loading glyph for '%d'\n", o);
					continue;
				}
				slot = (faces[fallback])->glyph;
				if (slot->format == FT_GLYPH_FORMAT_OUTLINE) {
					error = FT_Render_Glyph((faces[fallback])->glyph, FT_RENDER_MODE_NORMAL);
					if (error) {
						fprintf(stderr, "Error rendering glyph for '%d'\n", o);
						continue;
					}
				}
			}

		}

		draw_char(&slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top, fg, ctx);
		pen_x += slot->advance.x >> 6;
		pen_y += slot->advance.y >> 6;
	}
}

void draw_string_shadow(gfx_context_t * ctx, int x, int y, uint32_t fg, char * string, uint32_t shadow_color, int darkness, int offset_x, int offset_y, double radius) {
#define OFFSET_X  5
#define OFFSET_Y  5
#define WIDTH_PAD 15
#define HEIGHT_PAD 15

	gfx_context_t * tmp_c, * out_c;
	sprite_t * tmp_s, * out_s;

	size_t width = draw_string_width(string) + WIDTH_PAD;
	size_t height = _font_size + HEIGHT_PAD;

	tmp_s = create_sprite(width, height, ALPHA_EMBEDDED);
	tmp_c = init_graphics_sprite(tmp_s);

	out_s = create_sprite(width, height, ALPHA_EMBEDDED);
	out_c = init_graphics_sprite(out_s);

	draw_fill(tmp_c, rgba(0,0,0,0));
	draw_string(tmp_c, OFFSET_X + offset_x, OFFSET_Y + offset_y + _font_size, shadow_color, string);

	blur_context(out_c, tmp_c, radius);

	draw_string(out_c, OFFSET_X, OFFSET_Y + _font_size, fg, string);

	for (int i = 0; i < darkness; ++i) {
		draw_sprite(ctx, out_s, x - OFFSET_X, y - OFFSET_Y - _font_size);
	}

	sprite_free(tmp_s);
	free(tmp_c);

	sprite_free(out_s);
	free(out_c);
}
