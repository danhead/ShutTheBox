#include <pebble.h>
#include "gameover.h"

static struct {
	GBitmap *background;
	GBitmap *tab_closed;
	GBitmap *tab_open;
	GBitmap *tab_open_selected;
} boxImgs;
GBitmap *diceImgs[6];

static struct {
	Window *window;
	Layer *box;
	Layer *dice;
  Layer *overlay;
	Layer *scoreboard;
  Layer *gameover;
  GPath *pointer;
} ui;
static GFont font_s, font_l;
static GPathInfo arrow = {
	.num_points = 3,
	.points = (GPoint []) {{-7, -7}, {7, 0}, {-7, 7}}
};
typedef struct{
	bool open;
} object;
object tabs[9];
int selected, game_stage, d1, d2, counter, dice_selected, tabs_closed;

static void new_game(void) {
  srand(time(0));
	counter = 0;
	selected = 0;
	dice_selected = 0;
	tabs_closed = 0;
	game_stage = 1;
	for(int i=0;i<9;i++) {
    	tabs[i].open = true;
  }
}

static bool move_possible(void) {
  //get remaining numbers
  int nLeft[9], h = 0;
  int target = d1+d2-counter;
  for(int i=0;i<target;i++) {
    if(tabs[i].open) {
      nLeft[h] = i+1;
      h++;
    }
  }
  //check individual numbers and combinations
  for(int i=0;i<h;i++) {
    if(nLeft[i] == target) return true;
    for(int j=0;j<h;j++) {
      if(nLeft[i]+nLeft[j] == target && nLeft[i]!=nLeft[j]) return true;
      for(int k=0;k<h;k++) {
        if(nLeft[i]!=nLeft[j] &&
        nLeft[i]!=nLeft[k] &&
        nLeft[j]!=nLeft[k] &&
        nLeft[i]+nLeft[j]+nLeft[k] == target) return true;
        for(int l=0;l<h;l++) {
          if(nLeft[i]!=nLeft[j] &&
          nLeft[i]!=nLeft[k] &&
          nLeft[i]!=nLeft[l] &&
          nLeft[j]!=nLeft[k] &&
          nLeft[j]!=nLeft[l] &&
          nLeft[k]!=nLeft[l] &&
          nLeft[i]+nLeft[j]+nLeft[k]+nLeft[l] == target) return true;
        }
      }
    }
  }
  return false;
}

static bool is_legal(int x) {
  if(x>d1+d2) return false;
  if(x>d1+d2-counter) return false;
  return true;
}

static bool check_move_is_done(void) {
  if(counter==d1+d2) {
    return true;
  } else if(counter<d1+d2 && !move_possible()) {
    game_stage = 4;
    layer_mark_dirty(ui.dice);
  }
  return false;
}

static int total_left(void) {
  int total = 0;
  for(int i=0;i<9;i++) {
    if(tabs[i].open) {
      total+=i+1;
    }
  }
  return total;
}

static void roll_dice(int n) {
  d1=(rand()%5)+1;
  d2=0;
  if(n==2) d2=(rand()%5)+1;
  if(d1+d2<=total_left()) {
    game_stage=2;
  }
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {

}

static void box_update_callback(Layer *me, GContext* ctx) {
	graphics_draw_bitmap_in_rect(ctx, boxImgs.background, GRect(4,10,136,108));
	for(int i=0;i<9;i++) {
    	int x,y;
	    GBitmap *image;
	    if(i<5) {
	    	x = 8+(i*26);
	    	y = (tabs[i].open)?4:14;
	    } else {
	    	x = 12+((i-5)*32);
	    	y = (tabs[i].open)?60:70;
	    }
	    if(tabs[i].open) {
	    	if(i==selected) {
	    		image = boxImgs.tab_open_selected;
	    	} else {
	    		image = boxImgs.tab_open;
	    	}
	    } else {
	    	image = boxImgs.tab_closed;
	    }
	    graphics_draw_bitmap_in_rect(ctx, image, GRect(x, y, 24, 44));
	}
	graphics_context_set_text_color(ctx, GColorBlack);
  if(tabs[0].open) {
    graphics_draw_text(ctx,
      "1",
      font_l,
      GRect(15, 15, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[1].open) {
    graphics_draw_text(ctx,
      "2",
      font_l,
      GRect(41, 15, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[2].open) {
    graphics_draw_text(ctx,
      "3",
      font_l,
      GRect(67, 16, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[3].open) {
    graphics_draw_text(ctx,
      "4",
      font_l,
      GRect(93, 15, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[4].open) {
    graphics_draw_text(ctx,
      "5",
      font_l,
      GRect(119, 15, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[5].open) {
    graphics_draw_text(ctx,
      "6",
      font_l,
      GRect(19, 71, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[6].open) {
    graphics_draw_text(ctx,
      "7",
      font_l,
      GRect(51, 71, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[7].open) {
    graphics_draw_text(ctx,
      "8",
      font_l,
      GRect(83, 71, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[8].open) {
    graphics_draw_text(ctx,
      "9",
      font_l,
      GRect(115, 71, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
}
static void dice_update_callback(Layer *me, GContext* ctx) {
	graphics_context_set_text_color(ctx, GColorBlack);
	if(game_stage == 1) {
		graphics_draw_text(ctx,
			"Roll Dice",
			font_l,
			GRect(30, 0, 100, 29),
			GTextOverflowModeWordWrap,
			GTextAlignmentLeft,
			NULL);
	} else if(game_stage == 2 || game_stage == 4) {
    int y = 10;
    if(game_stage == 4) y = 0;
		graphics_draw_bitmap_in_rect(ctx, diceImgs[d1-1], GRect(30, y, 24, 24));
		if(d2!=0) {
			graphics_draw_bitmap_in_rect(ctx, diceImgs[d2-1], GRect(90, y, 24, 24));
		}
    if(game_stage == 4) { 
  		graphics_draw_text(ctx,
  			"GAME OVER",
  			font_l,
  			GRect(20, 18, 120, 30),
  			GTextOverflowModeWordWrap,
  			GTextAlignmentLeft,
  			NULL);
    }
	}
}

static void overlay_update_callback(Layer *me, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx,
    GRect(0,0,114,80),
    8,
    GCornersAll);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx,
    GRect(3,3,108,74),
    8,
    GCornersAll);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx,
    "How many dice?",
    font_s,
    GRect(0, 0, 114, 25),
    GTextOverflowModeWordWrap,
    GTextAlignmentCenter,
    NULL);
  graphics_draw_text(ctx,
    "1 Die",
    font_s,
    GRect(30, 25, 70, 25),
    GTextOverflowModeWordWrap,
    GTextAlignmentLeft,
    NULL);
  graphics_draw_text(ctx,
    "2 Dice",
    font_s,
    GRect(30, 50, 70, 25),
    GTextOverflowModeWordWrap,
    GTextAlignmentLeft,
    NULL);
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, ui.pointer);
}

static void click_up(ClickRecognizerRef recognizer, void *context) {
	if(game_stage == 1) {
		roll_dice(2);
	} else if(game_stage == 2) {
		int newtab = selected+1;
		if(newtab>8) newtab = 0;
		while(!tabs[newtab].open) {
			newtab++;
			if(newtab>8) newtab = 0;
			if(newtab==selected) break;
		}
		selected = newtab;
	} else if(game_stage == 3) {
		if(dice_selected == 1) {
			dice_selected = 2;
			gpath_move_to(ui.pointer, GPoint(15, 65));
		} else {
			dice_selected = 1;
			gpath_move_to(ui.pointer, GPoint(15, 42));
      layer_mark_dirty(ui.dice);
		}
	} else if(game_stage == 4) {
		new_game();
		//layer_mark_dirty(&dice);
	}
	layer_mark_dirty(ui.box);
}
static void click_down(ClickRecognizerRef recognizer, void *context) {
	if(game_stage == 1) {
		roll_dice(2);
	} else if(game_stage == 2) {
		int newtab = selected-1;
		if(newtab<0) newtab = 8;
		while(!tabs[newtab].open) {
			newtab--;
			if(newtab<0) newtab = 8;
			if(newtab==selected) break;
		}
		selected = newtab;
	} else if(game_stage == 3) {
		if(dice_selected == 1) {
			dice_selected = 2;
			gpath_move_to(ui.pointer, GPoint(15, 65));
		} else {
			dice_selected = 1;
			gpath_move_to(ui.pointer, GPoint(15, 42));
		}
    layer_mark_dirty(ui.dice);
	} else if(game_stage == 4) {
		new_game();
		layer_mark_dirty(ui.dice);
	}
	layer_mark_dirty(ui.box);
}
static void click_select(ClickRecognizerRef recognizer, void *context) {
	if(game_stage == 1) {
    if(total_left()<7) {
      layer_set_hidden(ui.overlay,false);
      game_stage = 3;
      dice_selected = 1;
      gpath_move_to(ui.pointer, GPoint(15, 42));
      layer_mark_dirty(ui.dice);
    } else {
      roll_dice(2);
      if(!move_possible()) {
        game_stage=4;
        layer_mark_dirty(ui.dice);
      }
    }
  } else if(game_stage == 2) {
    if(tabs[selected].open) {
      int x = selected+1;
      if(is_legal(x)) {
        counter+=x;
        tabs[selected].open = false;
        tabs_closed++;
        if(check_move_is_done()) {
          if(tabs_closed==9) {
            gameover_init(true);
          }
          counter = 0;
          game_stage = 1;
          selected = 0;
          while(!tabs[selected].open) {
            selected++;
          }
          layer_mark_dirty(ui.dice);
        }
      }
    }
  } else if(game_stage == 3) {
    layer_set_hidden(ui.overlay,true);
    roll_dice(dice_selected);
    if(!move_possible()) {
      game_stage=4;
      layer_mark_dirty(ui.dice);
    }
  } else if(game_stage == 4) {
    new_game();
    layer_mark_dirty(ui.dice);
  }
  layer_mark_dirty(ui.box);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  font_s = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  font_l = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  boxImgs.background = gbitmap_create_with_resource(RESOURCE_ID_BOX_BACKGROUND);
  boxImgs.tab_closed = gbitmap_create_with_resource(RESOURCE_ID_TAB_CLOSED);
  boxImgs.tab_open = gbitmap_create_with_resource(RESOURCE_ID_TAB_OPEN);
  boxImgs.tab_open_selected = gbitmap_create_with_resource(RESOURCE_ID_TAB_OPEN_SELECTED);
  diceImgs[0] = gbitmap_create_with_resource(RESOURCE_ID_DICE_1);
  diceImgs[1] = gbitmap_create_with_resource(RESOURCE_ID_DICE_2);
  diceImgs[2] = gbitmap_create_with_resource(RESOURCE_ID_DICE_3);
  diceImgs[3] = gbitmap_create_with_resource(RESOURCE_ID_DICE_4);
  diceImgs[4] = gbitmap_create_with_resource(RESOURCE_ID_DICE_5);
  diceImgs[5] = gbitmap_create_with_resource(RESOURCE_ID_DICE_6);
  ui.pointer = gpath_create(&arrow);

  ui.box = layer_create((GRect){
  	.origin = { 0, 0 },
  	.size = { 144, 118 }
  });
  ui.dice = layer_create((GRect){
  	.origin = { 0, 120 },
  	.size = { 144, 48 }
  });
  ui.overlay = layer_create((GRect) {
    .origin = { 15, 70 },
    .size = { 114, 80 }
  });
  layer_set_hidden(ui.overlay,true);
  layer_set_update_proc(ui.box, box_update_callback);
  layer_set_update_proc(ui.dice, dice_update_callback);
  layer_set_update_proc(ui.overlay, overlay_update_callback);
  layer_add_child(window_layer, ui.box);
  layer_add_child(window_layer, ui.dice);
  layer_add_child(window_layer, ui.overlay);

  new_game();
}

static void window_unload(Window *window) {
	gbitmap_destroy(boxImgs.background);
	gbitmap_destroy(boxImgs.tab_closed);
	gbitmap_destroy(boxImgs.tab_open);
	gbitmap_destroy(boxImgs.tab_open_selected);
	gbitmap_destroy(diceImgs[0]);
	gbitmap_destroy(diceImgs[1]);
	gbitmap_destroy(diceImgs[2]);
	gbitmap_destroy(diceImgs[3]);
	gbitmap_destroy(diceImgs[4]);
	gbitmap_destroy(diceImgs[5]);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, click_select);
  window_single_click_subscribe(BUTTON_ID_UP, click_up);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_down);
}

void game_init(void) {
	ui.window = window_create();
	window_set_fullscreen(ui.window, true);
	window_set_click_config_provider(ui.window, click_config_provider);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(ui.window, true);
}
void game_deinit(void) {
  window_destroy(ui.window);
}