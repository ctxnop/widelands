/*
 * Copyright (C) 2008-2010, 2012 by the Widelands Development Team
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

#ifndef WL_AI_DEFAULTAI_H
#define WL_AI_DEFAULTAI_H

#include <map>
#include <memory>

#include "ai/ai_help_structs.h"
#include "ai/computer_player.h"
#include "base/i18n.h"
#include "logic/immovable.h"
#include "logic/ship.h"
#include <unordered_set>

namespace Widelands {
struct Road;
struct Flag;
}

/**
 * Default Widelands Computer Player (defaultAI)
 *
 * The behaviour of defaultAI is controlled via \ref DefaultAI::think() and all
 * functions called by \ref DefaultAI::think().
 * At the moment defaultAI should be able to build up a basic infrastructure
 * including food, mining and smithing infrastructure and a basic street net.
 * It should be able to expand it's territory and to recruit some soldiers from
 * the weapons made out of it's mined resources.
 * It does only construct buildable and allowed (scenario mode) buildings.
 * It behaves after preciousness_ of a ware, which can be defined in wares conf
 * file. The higher the preciousness_, the more will defaultAI care for that ware
 * and will try to build up an infrastructure to create that ware.
 *
 * \NOTE Network safeness:
 * - The current implementation does not care about network safe randomness, as
 *   only the host is running the computer player code and sends it's player
 *   commands to all other players. If this network behaviour is changed,
 *   remember to change some time() in network save random functions.
 */
// TODO(unknown): Improvements:
// - Improve different initialization types (Aggressive, Normal, Defensive)
// - Improve update code - currently the whole buildable area owned by defaultAI
//   is rechecked after construction of a building or a road. Instead it would
//   be better to write down the changed coordinates and only check those and
//   surrounding ones. Same applies for other parts of the code:
//   e.g. check_militarysite checks the whole visible area for enemy area, but
//   it would already be enough, if it checks the outer circle ring.
// - improvements and speedups in the whole defaultAI code.
// - handling of trainingsites (if supply line is broken - send some soldiers
//   out, to have some more forces. Reincrease the number of soldiers that
//   should be trained if inputs_ get filled again.).
struct DefaultAI : ComputerPlayer {
	DefaultAI(Widelands::Game&, const Widelands::PlayerNumber, uint8_t);
	~DefaultAI();
	void think() override;

	enum {
		AGGRESSIVE = 2,
		NORMAL = 1,
		DEFENSIVE = 0,
	};

	/// Implementation for Aggressive
	struct AggressiveImpl : public ComputerPlayer::Implementation {
		AggressiveImpl() {
			name = _("Aggressive");
		}
		ComputerPlayer* instantiate(Widelands::Game& game,
		                            Widelands::PlayerNumber const p) const override {
			return new DefaultAI(game, p, AGGRESSIVE);
		}
	};

	struct NormalImpl : public ComputerPlayer::Implementation {
		NormalImpl() {
			name = _("Normal");
		}
		ComputerPlayer* instantiate(Widelands::Game& game,
		                            Widelands::PlayerNumber const p) const override {
			return new DefaultAI(game, p, NORMAL);
		}
	};

	struct DefensiveImpl : public ComputerPlayer::Implementation {
		DefensiveImpl() {
			name = _("Defensive");
		}
		ComputerPlayer* instantiate(Widelands::Game& game,
		                            Widelands::PlayerNumber const p) const override {
			return new DefaultAI(game, p, DEFENSIVE);
		}
	};

	static AggressiveImpl aggressiveImpl;
	static NormalImpl normalImpl;
	static DefensiveImpl defensiveImpl;

private:
	void late_initialization();

	void update_all_buildable_fields(int32_t);
	void update_all_mineable_fields(int32_t);
	void update_all_not_buildable_fields();

	void update_buildable_field(BuildableField&, uint16_t = 6, bool = false);
	void update_mineable_field(MineableField&);

	void update_productionsite_stats(int32_t);

	void check_ware_necessity(BuildingObserver& bo,
	                          bool* output_is_needed,
	                          int16_t* max_preciousness,
	                          int16_t* max_needed_preciousness);

	bool construct_building(int32_t);

	uint32_t coords_hash(Widelands::Coords coords) {
		uint32_t hash = coords.x << 16 | coords.y;
		return hash;
	}

	Widelands::Coords coords_unhash(uint32_t hash) {
		Widelands::Coords coords;
		coords.x = hash >> 16;  // is cast needed here???
		coords.y = hash;
		return coords;
	}

	// all road management is invoked by function improve_roads()
	// if needed it calls create_shortcut_road() with a flag from which
	// new road should be considered (or is needed)
	bool improve_roads(int32_t);
	bool create_shortcut_road(const Widelands::Flag&, uint16_t maxcheckradius, uint16_t minred);
	// trying to identify roads that might be removed
	bool dispensable_road_test(const Widelands::Road&);
	bool check_economies();
	bool check_productionsites(int32_t);
	bool check_trainingsites(int32_t);
	bool check_mines_(int32_t);
	bool check_militarysites(int32_t);
	bool marine_main_decisions(uint32_t);
	bool check_ships(uint32_t);
	uint32_t get_stocklevel_by_hint(size_t);
	uint32_t get_stocklevel(BuildingObserver&);
	uint32_t get_warehoused_stock(Widelands::WareIndex wt);
	uint32_t get_stocklevel(Widelands::WareIndex);  // count all direct outputs_
	void check_helpersites(int32_t);
	void review_wares_targets(int32_t);

	int32_t recalc_with_border_range(const BuildableField&, int32_t);
	int32_t calculate_need_for_ps(BuildingObserver&, int32_t);

	void
	consider_productionsite_influence(BuildableField&, Widelands::Coords, const BuildingObserver&);
	// considering wood, stones, mines, water, fishes for candidate for colonization (new port)
	uint8_t spot_scoring(Widelands::Coords candidate_spot);

	EconomyObserver* get_economy_observer(Widelands::Economy&);
	BuildingObserver& get_building_observer(char const*);

	void gain_immovable(Widelands::PlayerImmovable&);
	void lose_immovable(const Widelands::PlayerImmovable&);
	void gain_building(Widelands::Building&);
	void lose_building(const Widelands::Building&);
	void expedition_management(ShipObserver&);
	void out_of_resources_site(const Widelands::ProductionSite&);

	bool check_supply(const BuildingObserver&);

	bool consider_attack(int32_t);

	void print_land_stats();

private:
	// Variables of default AI
	uint8_t type_;

	bool m_buildable_changed;
	bool m_mineable_changed;

	Widelands::Player* player_;
	Widelands::TribeDescr const* tribe_;

	std::vector<BuildingObserver> buildings_;
	uint32_t num_constructionsites_;
	uint32_t num_milit_constructionsites;
	uint32_t num_prod_constructionsites;
	uint32_t num_ports;

	std::list<Widelands::FCoords> unusable_fields;
	std::list<BuildableField*> buildable_fields;
	std::list<BlockedField> blocked_fields;
	std::unordered_set<uint32_t> port_reserved_coords;
	// to distinquish which ports are on home teritory and which one are remote
	std::unordered_set<uint32_t> remote_ports_coords;
	std::list<MineableField*> mineable_fields;
	std::list<Widelands::Flag const*> new_flags;
	std::list<Widelands::Coords> flags_to_be_removed;
	std::list<Widelands::Road const*> roads;
	std::list<EconomyObserver*> economies;
	std::list<ProductionSiteObserver> productionsites;
	std::list<ProductionSiteObserver> mines_;
	std::list<MilitarySiteObserver> militarysites;
	std::list<WarehouseSiteObserver> warehousesites;
	std::list<TrainingSiteObserver> trainingsites;
	std::list<ShipObserver> allships;

	std::vector<WareObserver> wares;

	int32_t next_road_due_;
	int32_t next_stats_update_due_;
	int32_t next_construction_due_;
	int32_t next_mine_construction_due_;
	int32_t next_productionsite_check_due_;
	int32_t next_mine_check_due_;
	int32_t next_militarysite_check_due_;
	uint32_t next_ship_check_due;
	uint32_t next_marine_decisions_due;
	int32_t next_attack_consideration_due_;
	int32_t next_trainingsites_check_due_;
	int32_t next_bf_check_due_;
	int32_t next_wares_review_due_;
	int32_t inhibit_road_building_;
	int32_t time_of_last_construction_;
	int32_t enemy_last_seen_;

	uint16_t numof_warehouses_;

	bool new_buildings_stop_;

	// when territory is expanded for every candidate field benefits are calculated
	// but need for water, space, mines can vary
	// so if 255 = resource is needed, 0 = not needed
	uint8_t resource_necessity_territory_;
	uint8_t resource_necessity_mines_;
	uint8_t resource_necessity_stones_;
	uint8_t resource_necessity_water_;
	bool resource_necessity_water_needed_;  // unless atlanteans

	uint16_t unstationed_milit_buildings_;  // counts empty military buildings (ones where no soldier
	                                        // is belogning to)
	uint16_t military_under_constr_;
	uint16_t military_last_dismantle_;
	int32_t military_last_build_;  // sometimes expansions just stops, this is time of last military
	                               // building build
	Widelands::Coords
	   last_attack_target_;         // flag to abuilding (position) that was attacked last time
	int32_t next_attack_waittime_;  // second till the next attack consideration
	bool seafaring_economy;  // false by default, until first port space is found
	uint32_t colony_scan_area_;  // distance from a possible port that is scanned for owned territory
	// it decreases with failed scans
	int32_t spots_;  // sum of buildable fields

	enum {REPRIORITIZE, STOPSHIPYARD, STARTSHIPYARD};
	std::vector<int16_t> marineTaskQueue_;

	std::unique_ptr<Notifications::Subscriber<Widelands::NoteFieldPossession>>
	   field_possession_subscriber_;
	std::unique_ptr<Notifications::Subscriber<Widelands::NoteImmovable>> immovable_subscriber_;
	std::unique_ptr<Notifications::Subscriber<Widelands::NoteProductionSiteOutOfResources>>
	   outofresource_subscriber_;
	std::unique_ptr<Notifications::Subscriber<Widelands::NoteShipMessage>> shipnotes_subscriber_;
};

#endif  // end of include guard: WL_AI_DEFAULTAI_H
