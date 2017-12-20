dirname = path.dirname (__file__)

tribes:new_productionsite_type {
   msgctxt = "frisians_building",
   name = "frisians_claypit",
   -- TRANSLATORS: This is a building name used in lists of buildings
   descname = pgettext ("frisians_building", "Clay Pit"),
   helptext_script = dirname .. "helptexts.lua",
   icon = dirname .. "menu.png",
   size = "small",

   buildcost = {
      brick = 1,
      granite = 1,
      log = 1,
      thatch_reed = 1
   },
   return_on_dismantle = {
      brick = 1,
      granite = 1
   },

   animations = {
      idle = {
         pictures = path.list_files (dirname .. "idle_??.png"),
         hotspot = { 62, 94 },
         fps = 10,
      },
      working = {
         pictures = path.list_files (dirname .. "working_??.png"),
         hotspot = { 62, 94 },
         fps = 10,
      },
      unoccupied = {
         pictures = path.list_files (dirname .. "unoccupied_?.png"),
         hotspot = { 62, 94 },
      },
   },

   aihints = {
      very_weak_ai_limit = 1,
      weak_ai_limit = 2,
      basic_amount = 1,
      supports_production_of = { pond_dry }
   },

   working_positions = {
      frisians_clay_burner = 1
   },

   inputs = {
      { name = "water", amount = 4 },
   },
   outputs = {
      "clay"
   },

   programs = {
      work = {
         -- TRANSLATORS: Completed/Skipped/Did not start making clay because ...
         descname = _"making clay",
         actions = {
            "sleep=20000",
            "return=skipped unless economy needs clay",
            "return=failed unless site has water",
            "worker=dig",
            "consume=water",
            "sleep=2000",
            "animate=working 17000",
            "sleep=1000",
            "produce=clay"
         },
      },
   },
   out_of_resource_notification = {
      -- Translators: Short for "Out of Earth" for clay pits
      title = _"No flat earth",
      heading = _"Out of Earth",
      message = pgettext ("frisians_building", "The clay burner working at this clay pit can’t find any flat earth in his work area. Consider dismantling this clay pit and rebuilding it somewhere else."),
      productivity_threshold = 33
   },
}
