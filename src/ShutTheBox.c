#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xB5, 0x1A, 0xB6, 0xD6, 0x49, 0x6B, 0x47, 0xD0, 0xB0, 0x15, 0x9B, 0x55, 0x84, 0x12, 0x1D, 0x09 }
PBL_APP_INFO(MY_UUID,
             "Shut the box", "Dan Head",
             1, 0, /* App version */
             RESOURCE_ID_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window_menu, window_rules, window_game, window_gameover;
Layer menu, rules, box, dice, scoreboard, gameover;
BmpContainer splash_image, box_background, tab_open, tab_open_selected, tab_closed, dice_images[6], me_image;
GFont font_22, font_28;
GPath arrows[4];

GPathInfo arrow_up={3, (GPoint []){{10, 10}, {-10, 10}, {0, -10}}};
GPathInfo arrow_right={3, (GPoint []){{-10, -10}, {10, 0}, {-10, 10}}};
GPathInfo arrow_down={3, (GPoint []){{0, 10}, {-10, -10}, {10, -10}}};
GPathInfo arrow_left={3, (GPoint []){{-10, 0}, {10, -10}, {10, 10}}};

typedef struct{
    bool open;
} object;

object tabs[9];
int selected, game_stage, d1, d2, counter, rules_posY = 5, menu_pointer, dice_selected, tabs_closed, random=0;
PblTm current_time;
long seed = 0;

/* ------------------------------------------------------------------ //

    METHODS

// ------------------------------------------------------------------ */

void new_game() {
  get_time(&current_time);
  seed = (current_time.tm_hour*3600)*(current_time.tm_min*60)*current_time.tm_sec;
  counter = 0;
  selected = 0;
  dice_selected = 0;
  tabs_closed = 0;
  game_stage = 1;
  for(int i=0;i<9;i++) {
    tabs[i].open = true;
  }
}

void new_gameover() {
  window_stack_push(&window_gameover, true);
  layer_mark_dirty(&gameover);
  new_game();
}
bool move_possible() {
  //get remaining numbers
  int nLeft[9], h = 0;
  int target = d1+d2;
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

bool is_legal(int x) {
  if(x>d1+d2) return false;
  if(x>d1+d2-counter) return false;
  return true;
}

bool check_move_is_done() {
  if(counter==d1+d2) {
    return true;
  } else if(counter>d1+d2) {

  }
  return false;
}

int total_left() {
  int total = 0;
  for(int i=0;i<9;i++) {
    if(tabs[i].open) {
      total+=i+1;
    }
  }
  return total;
}

void roll_dice(int n) {
  seed = (((seed * 214013L + 2531011L) >> 16) & 32767);
  if ((seed%6) == 0) {
    seed++;
    seed = (((seed * 214013L + 2531011L) >> 16) & 32767);
    d1=((seed%6)+1);
    d2=((seed%6)+1);
  } else {
    d1=((seed%6)+1);
    seed++;
    seed = (((seed * 214013L + 2531011L) >> 16) & 32767);
    d2=((seed%6)+1);
  }
  if(n==1) d2=0;
  if(d1+d2<=total_left()) {
    game_stage=2;
  }
}

void menu_move_arrow() {
  if(menu_pointer == 0) {
    gpath_move_to(&arrows[0], GPoint(15, 150));
    menu_pointer = 1;
  } else {
    gpath_move_to(&arrows[0], GPoint(15, 120));
    menu_pointer = 0;
  }
  layer_mark_dirty(&menu);
}

/* ------------------------------------------------------------------ //

    UPDATE CALLBACKS

// ------------------------------------------------------------------ */

void menu_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;
  graphics_draw_bitmap_in_rect(ctx, &splash_image.bmp, GRect(0, 0, 144, 100));
  gpath_draw_filled(ctx, &arrows[0]);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_text_draw(ctx,
    "Play",
    font_22,
    GRect(30, 105, 80, 30),
    GTextOverflowModeWordWrap,
    GTextAlignmentLeft,
    NULL);
  graphics_text_draw(ctx,
    "Rules",
    font_22,
    GRect(30, 135, 120, 30),
    GTextOverflowModeWordWrap,
    GTextAlignmentLeft,
    NULL);

}

void rules_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;
  graphics_context_set_text_color(ctx, GColorBlack);

  graphics_text_draw(ctx,"The objective of the game is to close all 9 tiles and shut the box.\n\nRoll two dice and close any combination of tiles that equal the sum of the rolled dice.\n\nYou must use the sum of the dice each turn, if you cannot the game is over. Once the sum total of all the open tiles equals 6 or less, you have the choice of using one die instead of two.\n\n\nWritten by Daniel Head.\nme@danielhead.com",
    fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
    GRect(5, rules_posY, 120, 400),
    GTextOverflowModeWordWrap,
    GTextAlignmentLeft,
    NULL);

  if(rules_posY!=5) {
    gpath_draw_filled(ctx, &arrows[1]);
  }
  if(rules_posY!=-250) {
    gpath_draw_filled(ctx, &arrows[2]);
  }
}

void box_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;
  GRect dest = layer_get_frame(&box_background.layer.layer);
  dest.origin.x = 4;
  dest.origin.y = 10;
  graphics_draw_bitmap_in_rect(ctx, &box_background.bmp, dest);


  for(int i=0;i<9;i++) {
    int x,y;
    BmpContainer image;
    if(i<5) {
      x = 8+(i*26);
      y = (tabs[i].open)?4:14;
    } else {
      x = 12+((i-5)*32);
      y = (tabs[i].open)?60:70;
    }
    if(tabs[i].open) {
      if(i==selected) {
        image = tab_open_selected;
      } else {
        image = tab_open;
      }
    } else {
      image = tab_closed;
    }
    graphics_draw_bitmap_in_rect(ctx, &image.bmp, GRect(x, y, 24, 44));
  }
  graphics_context_set_text_color(ctx, GColorBlack);
  if(tabs[0].open) {
    graphics_text_draw(ctx,
      "1",
      font_28,
      GRect(12, 15, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[1].open) {
    graphics_text_draw(ctx,
      "2",
      font_28,
      GRect(38, 15, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[2].open) {
    graphics_text_draw(ctx,
      "3",
      font_28,
      GRect(64, 16, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[3].open) {
    graphics_text_draw(ctx,
      "4",
      font_28,
      GRect(90, 15, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[4].open) {
    graphics_text_draw(ctx,
      "5",
      font_28,
      GRect(116, 15, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[5].open) {
    graphics_text_draw(ctx,
      "6",
      font_28,
      GRect(16, 71, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[6].open) {
    graphics_text_draw(ctx,
      "7",
      font_28,
      GRect(48, 71, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[7].open) {
    graphics_text_draw(ctx,
      "8",
      font_28,
      GRect(80, 71, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
  if(tabs[8].open) {
    graphics_text_draw(ctx,
      "9",
      font_28,
      GRect(112, 71, 20, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }
}

void dice_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;

  graphics_context_set_text_color(ctx, GColorBlack);
  if(game_stage == 1) {

    graphics_text_draw(ctx,
      "Roll Dice",
      font_22,
      GRect(30, 5, 100, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  } else if(game_stage == 2 || game_stage == 4) {
    graphics_draw_bitmap_in_rect(ctx, &dice_images[d1-1].bmp, GRect(25, 10, 24, 24));
    if(d2!=0) {
      graphics_draw_bitmap_in_rect(ctx, &dice_images[d2-1].bmp, GRect(90, 10, 24, 24));
    }

  } else if(game_stage == 3) {

    graphics_text_draw(ctx,
      "1 Die",
      font_22,
      GRect(30, 5, 100, 25),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
    graphics_text_draw(ctx,
      "2 Dice",
      font_22,
      GRect(30, 30, 100, 25),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);

    gpath_draw_filled(ctx, &arrows[3]);

  }
  if(game_stage == 4) {
    graphics_text_draw(ctx,
      "Game over",
      font_22,
      GRect(20, 30, 120, 30),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
  }

}

void gameover_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;
  graphics_draw_bitmap_in_rect(ctx, &me_image.bmp, GRect(0, 0, 144, 130));
  graphics_context_set_text_color(ctx, GColorBlack);
      graphics_text_draw(ctx,
      "You win!",
      font_28,
      GRect(0, 125, 134, 40),
      GTextOverflowModeWordWrap,
      GTextAlignmentCenter,
      NULL);

}

/* ------------------------------------------------------------------ //

    CLICK HANDLERS

// ------------------------------------------------------------------ */

void menu_up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  menu_move_arrow();
}
void menu_down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  menu_move_arrow();

}
void menu_select_single_click_handler(ClickConfig **config, Window *window) {
  if(menu_pointer == 1) {
    window_stack_push(&window_rules, true);
    layer_mark_dirty(&rules);   
  } else {
    new_game();
    window_stack_push(&window_game, true);
    layer_mark_dirty(&box);
  }
}
void rules_up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  rules_posY=rules_posY+25;
  if(rules_posY>5) {
    rules_posY=5;
  }
  layer_mark_dirty(&rules);
}
void rules_down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  rules_posY=rules_posY-25;
  if(rules_posY<-250) {
    rules_posY=-250;
  }
  layer_mark_dirty(&rules);
}
void rules_select_single_click_handler(ClickConfig **config, Window *window) {

}
void game_up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
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
      gpath_move_to(&arrows[3], GPoint(15, 45));
    } else {
      dice_selected = 1;
      gpath_move_to(&arrows[3], GPoint(15, 20));
    }
  } else if(game_stage == 4) {
    new_game();
    layer_mark_dirty(&dice);
  }
  layer_mark_dirty(&box);

}
void game_down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
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
      gpath_move_to(&arrows[3], GPoint(15, 45));
    } else {
      dice_selected = 1;
      gpath_move_to(&arrows[3], GPoint(15, 20));
    }
  } else if(game_stage == 4) {
    new_game();
    layer_mark_dirty(&dice);
  }
  layer_mark_dirty(&box);
  
}
void game_select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  if(game_stage == 1) {
    if(total_left()<7) {
      game_stage = 3;
      dice_selected = 1;
      gpath_move_to(&arrows[3], GPoint(15, 20));
      layer_mark_dirty(&dice);
    } else {
      roll_dice(2);
      if(!move_possible()) {
        game_stage=4;
        layer_mark_dirty(&scoreboard);
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
            new_gameover();
          }
          counter = 0;
          game_stage = 1;
          selected = 0;
          while(!tabs[selected].open) {
            selected++;
          }
          layer_mark_dirty(&dice);
        }
      }
    }
  } else if(game_stage == 3) {
    roll_dice(dice_selected);
    if(!move_possible()) {
      game_stage=4;
    }
  } else if(game_stage == 4) {
    new_game();
    layer_mark_dirty(&dice);
  }
  layer_mark_dirty(&box);
}
void menu_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) menu_up_single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) menu_down_single_click_handler;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) menu_select_single_click_handler;
  (void)window;
}
void rules_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) rules_up_single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) rules_down_single_click_handler;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) rules_select_single_click_handler;
  (void)window;
}
void game_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) game_up_single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) game_down_single_click_handler;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) game_select_single_click_handler;
  (void)window;
}

/* ------------------------------------------------------------------ //

    INIT / DEINIT STUFF

// ------------------------------------------------------------------ */

void handle_init(AppContextRef ctx) {
  (void)ctx;
  window_init(&window_menu, "Shut the box");
  window_init(&window_rules, "Shut the box rules");
  window_init(&window_game, "Shut the box");
  window_init(&window_gameover, "Game Over");
  window_set_fullscreen(&window_menu, true);
  window_set_fullscreen(&window_game, true);
  window_set_fullscreen(&window_gameover, true);
  window_stack_push(&window_menu, true);
  layer_init(&menu, window_menu.layer.frame);
  layer_init(&rules, window_rules.layer.frame);
  layer_init(&box, window_game.layer.frame);
  layer_init(&dice, GRect(0, 110, 144, 58));
  layer_init(&gameover, window_gameover.layer.frame);
  menu.update_proc = &menu_update_callback;
  rules.update_proc = &rules_update_callback;
  box.update_proc = &box_update_callback;
  dice.update_proc = &dice_update_callback;
  gameover.update_proc = &gameover_update_callback;
  layer_add_child(&window_menu.layer, &menu);
  layer_add_child(&window_rules.layer, &rules);
  layer_add_child(&window_game.layer, &box);
  layer_add_child(&window_game.layer, &dice);
  layer_add_child(&window_gameover.layer, &gameover);

  resource_init_current_app(&APP_RESOURCES);
  font_22 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_22));
  font_28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_28));
  bmp_init_container(RESOURCE_ID_SPLASH, &splash_image);
  bmp_init_container(RESOURCE_ID_BOX_BACKGROUND, &box_background);
  bmp_init_container(RESOURCE_ID_TAB_CLOSED, &tab_closed);
  bmp_init_container(RESOURCE_ID_TAB_OPEN, &tab_open);
  bmp_init_container(RESOURCE_ID_TAB_OPEN_SELECTED, &tab_open_selected);
  bmp_init_container(RESOURCE_ID_ME, &me_image);
  bmp_init_container(RESOURCE_ID_DICE_1, &dice_images[0]);
  bmp_init_container(RESOURCE_ID_DICE_2, &dice_images[1]);
  bmp_init_container(RESOURCE_ID_DICE_3, &dice_images[2]);
  bmp_init_container(RESOURCE_ID_DICE_4, &dice_images[3]);
  bmp_init_container(RESOURCE_ID_DICE_5, &dice_images[4]);
  bmp_init_container(RESOURCE_ID_DICE_6, &dice_images[5]);
  window_set_click_config_provider(&window_menu, (ClickConfigProvider) menu_config_provider);
  window_set_click_config_provider(&window_rules, (ClickConfigProvider) rules_config_provider);
  window_set_click_config_provider(&window_game, (ClickConfigProvider) game_config_provider);

  gpath_init(&arrows[0], &arrow_right);
  gpath_move_to(&arrows[0], GPoint(15, 120));
  gpath_init(&arrows[1], &arrow_up);
  gpath_move_to(&arrows[1], GPoint(133, 10));
  gpath_init(&arrows[2], &arrow_down);
  gpath_move_to(&arrows[2], GPoint(133, 138));
  gpath_init(&arrows[3], &arrow_right);
  layer_mark_dirty(&menu);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;
  bmp_deinit_container(&splash_image);
  bmp_deinit_container(&box_background);
  bmp_deinit_container(&tab_closed);
  bmp_deinit_container(&tab_open);
  bmp_deinit_container(&tab_open_selected);
  bmp_deinit_container(&me_image);
  for(int i=0;i<6;i++){
    bmp_deinit_container(&dice_images[i]);
  }
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
