/*
 * Copyright (C) 2002-4 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __S__FONT_HANDLER_H
#define __S__FONT_HANDLER_H

#include <vector>
#include <list>
#include <queue>
#include <SDL_ttf.h>
#include "font_loader.h"

class RenderTarget;

enum Align {
	Align_Left = 0,
	Align_HCenter = 1,
	Align_Right = 2,
	Align_Horizontal = 3,

	Align_Top = 0,
	Align_VCenter = 4,
	Align_Bottom = 8,
	Align_Vertical = 12,

	Align_TopLeft = 0,
	Align_CenterLeft = Align_VCenter,
	Align_BottomLeft = Align_Bottom,

	Align_TopCenter = Align_HCenter,
	Align_Center = Align_HCenter|Align_VCenter,
	Align_BottomCenter = Align_HCenter|Align_Bottom,

	Align_TopRight = Align_Right,
	Align_CenterRight = Align_Right|Align_VCenter,
	Align_BottomRight = Align_Right|Align_Bottom,
};

/** class Font_Handler
 *
 * This class generates font Pictures out of strings and returns them
 */
class Font_Handler {
public:
	Font_Handler();
   ~Font_Handler();
	void draw_string(RenderTarget* dst, const std::string font, int size, RGBColor fg, RGBColor bg, int x, int y, std::string text,
			Align align = Align_CenterLeft, int wrap = -1) ;
	void get_size(std::string font, int size, std::string text, int *w, int *h, int wrap = -1);
	int calc_linewidth(TTF_Font* f, std::string &text);
	int get_fontheight(std::string font, int size);
	std::string remove_first_space(const std::string &text);
	std::string word_wrap_text(TTF_Font* f, const std::string &unwrapped_text, int max_width);

   // This deletes all cached pictures, it is called 
   // from the graphics code before the graphics are flushed,
   // to make sure that everything is forgotten
   void flush_cache( void );  

private:
   struct _Cache_Infos {
      uint referenced;
      uint surface_id;
      std::string str;
      TTF_Font* f;
      RGBColor fg;
      RGBColor bg;
      int      w;
      int      h;

      inline bool operator== (const _Cache_Infos& who) const {
         return ( str == who.str &&
               f == who.f &&
               fg == who.fg &&
               bg == who.bg);
      }
      inline bool operator< (const _Cache_Infos& who) const {
         return ( referenced > who.referenced) ;
      }
	};

 
private:
   static const uint CACHE_ARRAY_SIZE = 500;

	Font_Loader* m_font_loader;
   std::vector<_Cache_Infos> m_cache;
	
private:
   uint render_string(RenderTarget* dst, TTF_Font *f, RGBColor fg, RGBColor bg, int dstx, int dsty, std::string &text, Align align, int w, int h);
   uint convert_sdl_surface( SDL_Surface* );
	void do_blit(RenderTarget *dst,uint picid, int dstx, int dsty, Align align, int w, int h);
   uint create_static_long_text_surface( RenderTarget* dst, TTF_Font* f, RGBColor fg, RGBColor bg, std::string text, Align align, int wrap);
   uint create_single_line_text_surface( RenderTarget* dst, TTF_Font* f, RGBColor fg, RGBColor bg, std::string text, Align align); 
};

extern Font_Handler* g_fh;	// the default font

#endif

