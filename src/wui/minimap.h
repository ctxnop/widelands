/*
 * Copyright (C) 2002-2004, 2006, 2008-2011 by the Widelands Development Team
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef WL_WUI_MINIMAP_H
#define WL_WUI_MINIMAP_H

#include <boost/signals2.hpp>

#include "graphic/render/minimaprenderer.h"
#include "ui_basic/button.h"
#include "ui_basic/unique_window.h"

struct Interactive_Base;

struct MiniMap : public UI::UniqueWindow {
	struct Registry : public UI::UniqueWindow::Registry {
		MiniMapLayer flags; /**< Combination of \ref MiniMapLayer flags */

		Registry()
		   : flags(MiniMapLayer::Terrain | MiniMapLayer::Owner |
		                           MiniMapLayer::Flag | MiniMapLayer::Road |
		                           MiniMapLayer::Building) {
		}
	};

	MiniMap(Interactive_Base & parent, Registry *);

	boost::signals2::signal<void (int32_t, int32_t)> warpview;

	void set_view_pos(int32_t const x, int32_t const y) {
		m_view.set_view_pos(x, y);
	}

private:
	void toggle(MiniMapLayer);
	void update_button_permpressed();
	void resize();

	/**
	 * MiniMap::View is the panel that represents the pure representation of the
	 * map, without any borders or gadgets.
	 *
	 * If the size of MiniMapView is not the same as the size of the map itself,
	 * it will either show a subset of the map, or it will show the map more than
	 * once.
	 * The minimap always centers around the current viewpoint.
	 */
	struct View : public UI::Panel {
		View
			(UI::Panel & parent, MiniMapLayer * flags,
			 int32_t x, int32_t y, uint32_t w, uint32_t h,
			 Interactive_Base &);

		void set_view_pos(int32_t x, int32_t y);

		void draw(RenderTarget &) override;

		bool handle_mousepress  (Uint8 btn, int32_t x, int32_t y) override;
		bool handle_mouserelease(Uint8 btn, int32_t x, int32_t y) override;

		void set_zoom(int32_t z);


	private:
		Interactive_Base & m_ibase;
		int32_t                m_viewx, m_viewy;
		const Image* m_pic_map_spot;
	public:
		MiniMapLayer * m_flags;
	};

	uint32_t number_of_buttons_per_row() const;
	uint32_t number_of_button_rows    () const;
	uint32_t but_w                    () const;
	uint32_t but_h                    () const;

	View     m_view;
	UI::Button button_terrn;
	UI::Button button_owner;
	UI::Button button_flags;
	UI::Button button_roads;
	UI::Button button_bldns;
	UI::Button button_zoom;
};

#endif  // end of include guard: WL_WUI_MINIMAP_H
