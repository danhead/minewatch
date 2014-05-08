#include <pebble.h>

//COMMON
static Window *window;
static GFont font_xs, font_s, font_m, font_l;
static InverterLayer *inverter_layer;

//TIME/DATE
static TextLayer *time_layer;
static TextLayer *date_layer;
static char time_text[] = "--:--";
static char date_text[] = "00 Xxxxxxxxx";

//REFRESH ICON
static AppTimer *timeout_timer;
static int timeout_length = 15000; //ms
static BitmapLayer *refresh_layer;
static GBitmap *refresh_icon;

//BLUETOOTH ICON
static BitmapLayer *bluetooth_layer;
static GBitmap *bluetooth_icon;

//BATTERY ICON
static BitmapLayer *battery_layer;
static GBitmap *battery_icon;
static uint8_t battery_percentage = 110;

//INFO SPACE
static TextLayer *info_layer;


//BTC DATA
static Layer *btc_parent_layer;
static TextLayer *worker_text_layer;
static TextLayer *hashrate_text_layer;
static TextLayer *lastshare_text_layer;
static TextLayer *reward_text_layer;
static int current_workers = 0;

//BTC ICONS
static BitmapLayer *worker_icon_layer;
static GBitmap *worker_icon;
static BitmapLayer *hashrate_icon_layer;
static GBitmap *hashrate_icon;
static BitmapLayer *lastshare_icon_layer;
static GBitmap *lastshare_icon;
static BitmapLayer *reward_icon_layer;
static GBitmap *reward_icon;

//LAST UPDATE
static TextLayer *last_update_layer;
static char last_update_text[] = "--:--";


//BTC PROGRESS BAR
static InverterLayer *progress_layer;
static int progress_width = 0;

//KEYS
enum {
  RESPONSE = 0x1,
  UPDATE = 0x2,
  DARK = 0x3,
  NOTIFY = 0x4,
  FLICK = 0x5,
  WORKERS = 0xA,
  HASHRATE = 0xB,
  LASTSHARE = 0xC,
  REWARD = 0xD,
  SENDPROGRESS = 0xE,
  UPDATE_KEY,
  DARK_KEY,
  NOTIFY_KEY,
  FLICK_KEY
};

//COMMON SETTINGS
static bool dark = false;
static bool notify = false;
static bool flick = false;
static bool flick_enabled = false;
static int update_rate = 1;
static int current_min;
static int last_update_min;

static void show_refresh(bool refreshing) {
  gbitmap_destroy(refresh_icon);
  if(refreshing) refresh_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_REFRESH);
  else refresh_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_REFRESH_FAIL);
  bitmap_layer_set_bitmap(refresh_layer,refresh_icon);
  layer_mark_dirty(bitmap_layer_get_layer(refresh_layer));
  layer_set_hidden(bitmap_layer_get_layer(refresh_layer),false);
}
static void timeout_callback(void * data) {
  show_refresh(false);
}
static void fetch_msg(void) {

  //Make sure we don't request more than once per min (be nice to Mr Slush)
  if(current_min == last_update_min) return;
  last_update_min = current_min;
  Tuplet fetch_tuple = TupletInteger(0x0, 1);
  timeout_timer = app_timer_register(timeout_length,timeout_callback, NULL);
  show_refresh(true);
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &fetch_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void handle_flick(AccelAxisType axis, int32_t direction) {
  fetch_msg();
}

static void enable_flick(bool enable) {
  if(enable && !flick_enabled) {
    accel_tap_service_subscribe(&handle_flick);
    flick_enabled = true;
  }
  else if(!enable && flick_enabled) {
    accel_tap_service_unsubscribe();
    flick_enabled = false;
  }
}
static void handle_bluetooth(bool connected) {
  /**
   * Show/hide bluetooth icon
   */
  if(connected) layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer),false);
  else layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer),true);
}

static void handle_battery(BatteryChargeState charge_state) {
  /**
   * Update battery icon
   */
  if(battery_percentage != 110) gbitmap_destroy(battery_icon);
  if (charge_state.is_charging) battery_percentage = 101;
  else battery_percentage = charge_state.charge_percent;
  uint32_t res_id;
  switch(battery_percentage) {
    case 101: res_id = RESOURCE_ID_ICON_BATTERY_CHARGING; break;
    case 10: res_id = RESOURCE_ID_ICON_BATTERY_20; break;
    case 20: res_id = RESOURCE_ID_ICON_BATTERY_20; break;
    case 30: res_id = RESOURCE_ID_ICON_BATTERY_40; break;
    case 40: res_id = RESOURCE_ID_ICON_BATTERY_40; break;
    case 50: res_id = RESOURCE_ID_ICON_BATTERY_60; break;
    case 60: res_id = RESOURCE_ID_ICON_BATTERY_60; break;
    case 70: res_id = RESOURCE_ID_ICON_BATTERY_80; break;
    case 80: res_id = RESOURCE_ID_ICON_BATTERY_80; break;
    case 90: res_id = RESOURCE_ID_ICON_BATTERY_100; break;
    case 100: res_id = RESOURCE_ID_ICON_BATTERY_100; break;
    default: res_id = RESOURCE_ID_ICON_BATTERY_0; break;
  }
  battery_icon = gbitmap_create_with_resource(res_id);
  bitmap_layer_set_bitmap(battery_layer,battery_icon);
  layer_mark_dirty(bitmap_layer_get_layer(battery_layer));

}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  /**
   * Update layers based on app message response
   */

  //UPDATE SETTINGS
  Tuple *dark_tuple = dict_find(iter, DARK);
  if(dark_tuple) {
    if(strcmp(dark_tuple->value->cstring,"true")==0) {
      persist_write_bool(DARK_KEY,true);
      dark = true;
      layer_set_hidden(inverter_layer_get_layer(inverter_layer),false);
    } else {
      persist_write_bool(DARK_KEY,false);
      dark = false;
      layer_set_hidden(inverter_layer_get_layer(inverter_layer),true);
    }
  }
  Tuple *notify_tuple = dict_find(iter, NOTIFY);
  if(notify_tuple) {
    if(strcmp(notify_tuple->value->cstring,"true")==0) {
      persist_write_bool(NOTIFY_KEY,true);
      notify = true;
    }
    else {
      persist_write_bool(NOTIFY_KEY,false);
      notify = false;
    }
  }
  Tuple *flick_tuple = dict_find(iter, FLICK);
  if(flick_tuple) {
    if(strcmp(flick_tuple->value->cstring,"true")==0) {
      persist_write_bool(FLICK_KEY,true);
      flick = true;
      enable_flick(true);
    }
    else {
      persist_write_bool(FLICK_KEY,false);
      flick = false;
      enable_flick(false);
    }
  }
  Tuple *update_tuple = dict_find(iter, UPDATE);
  if(update_tuple) {
    int update = atoi(update_tuple->value->cstring);
    if(update > 0 && update < 61) {
      persist_write_int(UPDATE_KEY,update);
      update_rate = update;
    }
  }


  //UPDATE BTC VALUES
  Tuple *workers_tuple = dict_find(iter, WORKERS);
  Tuple *hashrate_tuple = dict_find(iter, HASHRATE);
  Tuple *reward_tuple = dict_find(iter, REWARD);
  Tuple *lastshare_tuple = dict_find(iter, LASTSHARE);
  Tuple *progress_tuple = dict_find(iter, SENDPROGRESS);
  if(workers_tuple) {
    int n = atoi(workers_tuple->value->cstring);
    if(n<current_workers && notify) vibes_long_pulse();
    current_workers = n;
    text_layer_set_text(worker_text_layer, workers_tuple->value->cstring);
  } 
  if(hashrate_tuple) text_layer_set_text(hashrate_text_layer, hashrate_tuple->value->cstring);
  if(reward_tuple) text_layer_set_text(reward_text_layer, reward_tuple->value->cstring);
  if(lastshare_tuple) text_layer_set_text(lastshare_text_layer, lastshare_tuple->value->cstring);
  if(progress_tuple) {
    int n = atoi(progress_tuple->value->cstring);
    if(n!=progress_width) {
      progress_width = n;
      layer_set_frame(inverter_layer_get_layer(progress_layer),(GRect) { .origin = { 0, 164 }, .size = { n, 4 } });
    }
  }
  //SHOW BTC VALUES or ERROR
  Tuple *response_tuple = dict_find(iter, RESPONSE);
  if(response_tuple) {
    if(strcmp(response_tuple->value->cstring,"NoAPI")==0) {
      layer_set_hidden(btc_parent_layer,true);
      text_layer_set_text(info_layer,"Please enter your Slush's Pool API Key into the MineWatch settings on your phone.");
      layer_set_hidden(text_layer_get_layer(info_layer),false);
    } else if (strcmp(response_tuple->value->cstring,"TIMEOUT")==0) {
      show_refresh(false);
    } else if (strcmp(response_tuple->value->cstring,"InvAPI")==0) {
      layer_set_hidden(btc_parent_layer,true);
      text_layer_set_text(info_layer,"Your API Key is invalid.");
      layer_set_hidden(text_layer_get_layer(info_layer), false);
    } else if(strcmp(response_tuple->value->cstring,"OK")==0) {
      layer_set_hidden(text_layer_get_layer(info_layer), true);
      layer_set_hidden(btc_parent_layer,false);
      strcpy(last_update_text,time_text);
    }
    app_timer_cancel(timeout_timer);
    if(strcmp(response_tuple->value->cstring,"TIMEOUT")!=0) layer_set_hidden(bitmap_layer_get_layer(refresh_layer),true);
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  show_refresh(false);
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  show_refresh(false);
}

static void app_message_init(void) {
  /**
   * Register message handlers
   */
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  // Init buffers
  app_message_open(128, 128);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  /**
   * Update time and date (if applicable)
   */

  //Check if we should get an update
  current_min= tick_time->tm_min;
  if(current_min%update_rate == 0) fetch_msg();

  char *time_format;

  char current_date[] = "00 Xxxxxxxxx";
  strcpy(current_date,date_text);
  strftime(date_text, sizeof(date_text), "%e %B", tick_time);
  if(strcmp(current_date,date_text)!=0) text_layer_set_text(date_layer, date_text);

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(time_layer, time_text);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  /**
   * TIME LAYER
   */
  time_layer = text_layer_create((GRect) { .origin = { 9, 10 }, .size = { 134, 50 } });
  text_layer_set_font(time_layer, font_l);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  /**
   * DATE LAYER
   */
  date_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { 80, 25 } });
  text_layer_set_font(date_layer, font_m);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  /**
   * REFRESH LAYER
   */
  refresh_layer = bitmap_layer_create((GRect) { .origin = { 85, 7 }, .size = { 20, 12 } });
  refresh_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_REFRESH);
  bitmap_layer_set_bitmap(refresh_layer,refresh_icon);
  //layer_set_hidden(bitmap_layer_get_layer(refresh_layer),true);
  layer_add_child(window_layer,bitmap_layer_get_layer(refresh_layer));

  /**
   * BLUETOOTH LAYER
   */
  bluetooth_layer = bitmap_layer_create((GRect) { .origin = { 110, 7 }, .size = { 6, 12 } });
  bluetooth_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_BLUETOOTH);
  bitmap_layer_set_bitmap(bluetooth_layer,bluetooth_icon);
  if(!bluetooth_connection_service_peek()) layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer),true);
  layer_add_child(window_layer,bitmap_layer_get_layer(bluetooth_layer));

  /**
   * BATTERY LAYER
   */
  battery_layer = bitmap_layer_create((GRect) { .origin = { 120, 7 }, .size = { 19, 12 } });
  layer_add_child(window_layer, bitmap_layer_get_layer(battery_layer));
  handle_battery(battery_state_service_peek());

  /**
   * INFO LAYER
   */
  info_layer = text_layer_create((GRect) { .origin = { 5, 70 }, .size = { 134, 90 } });
  text_layer_set_font(info_layer,font_s);
  layer_set_hidden(text_layer_get_layer(info_layer),true);
  layer_add_child(window_layer,text_layer_get_layer(info_layer));

  /**
   * BTC PARENT LAYER
   */
  btc_parent_layer = layer_create((GRect) { .origin = { 5, 62 }, .size = { 134, 90 } });
  layer_set_hidden(btc_parent_layer,true);
  layer_add_child(window_layer, btc_parent_layer);

  /**
   * BTC ICONS LAYERS
   */
  worker_icon_layer = bitmap_layer_create((GRect) { .origin = { 0, 3 }, .size = { 15, 15 } });
  worker_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_WORKER);
  bitmap_layer_set_bitmap(worker_icon_layer,worker_icon);
  layer_add_child(btc_parent_layer,bitmap_layer_get_layer(worker_icon_layer));
  hashrate_icon_layer = bitmap_layer_create((GRect) { .origin = { 0, 26 }, .size = { 15, 15 } });
  hashrate_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_GRAPH);
  bitmap_layer_set_bitmap(hashrate_icon_layer,hashrate_icon);
  layer_add_child(btc_parent_layer,bitmap_layer_get_layer(hashrate_icon_layer));
  lastshare_icon_layer = bitmap_layer_create((GRect) { .origin = { 0, 50 }, .size = { 15, 15 } });
  lastshare_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_CLOCK);
  bitmap_layer_set_bitmap(lastshare_icon_layer,lastshare_icon);
  layer_add_child(btc_parent_layer,bitmap_layer_get_layer(lastshare_icon_layer));
  reward_icon_layer = bitmap_layer_create((GRect) { .origin = { 0, 75 }, .size = { 15, 15 } });
  reward_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON_BTC);
  bitmap_layer_set_bitmap(reward_icon_layer,reward_icon);
  layer_add_child(btc_parent_layer,bitmap_layer_get_layer(reward_icon_layer));

  /**
   * BTC TEXT LAYERS
   */
  worker_text_layer = text_layer_create((GRect) { .origin = { 20, 0 }, .size = { 114, 20 } });
  text_layer_set_font(worker_text_layer,font_m);
  layer_add_child(btc_parent_layer,text_layer_get_layer(worker_text_layer));
  hashrate_text_layer = text_layer_create((GRect) { .origin = { 20, 23 }, .size = { 114, 20 } });
  text_layer_set_font(hashrate_text_layer,font_m);
  layer_add_child(btc_parent_layer,text_layer_get_layer(hashrate_text_layer));
  lastshare_text_layer = text_layer_create((GRect) { .origin = { 20, 47 }, .size = { 114, 20 } });
  text_layer_set_font(lastshare_text_layer,font_m);
  layer_add_child(btc_parent_layer,text_layer_get_layer(lastshare_text_layer));
  reward_text_layer = text_layer_create((GRect) { .origin = { 20, 70 }, .size = { 114, 20 } });
  text_layer_set_font(reward_text_layer,font_m);
  layer_add_child(btc_parent_layer,text_layer_get_layer(reward_text_layer));

  /**
   * PROGRESS LAYER
   */
  progress_layer = inverter_layer_create((GRect) { .origin = { 0, 164 }, .size = { 0, 4 } });
  layer_add_child(window_layer,inverter_layer_get_layer(progress_layer));


  /**
   * LAST UPDATE LAYER
   */
  last_update_layer = text_layer_create((GRect) { .origin = { 114, 149 }, .size = { 28, 14 } });
  text_layer_set_font(last_update_layer,font_xs);
  text_layer_set_text(last_update_layer,last_update_text);
  layer_add_child(window_layer,text_layer_get_layer(last_update_layer));

  /**
   * INVERTER LAYER
   */
   inverter_layer = inverter_layer_create((GRect) { .origin = { 0, 0 }, .size = { 144, 168 } });
   if(!dark) layer_set_hidden(inverter_layer_get_layer(inverter_layer), true);
   layer_add_child(window_layer,inverter_layer_get_layer(inverter_layer));
}

static void window_unload(Window *window) {
  inverter_layer_destroy(inverter_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  bitmap_layer_destroy(refresh_layer);
  gbitmap_destroy(refresh_icon);
  bitmap_layer_destroy(battery_layer);
  if(battery_icon) gbitmap_destroy(battery_icon);
  bitmap_layer_destroy(bluetooth_layer);
  gbitmap_destroy(bluetooth_icon);
  text_layer_destroy(info_layer);
  layer_destroy(btc_parent_layer);
  text_layer_destroy(worker_text_layer);
  text_layer_destroy(hashrate_text_layer);
  text_layer_destroy(lastshare_text_layer);
  text_layer_destroy(reward_text_layer);
  bitmap_layer_destroy(worker_icon_layer);
  gbitmap_destroy(worker_icon);
  bitmap_layer_destroy(hashrate_icon_layer);
  gbitmap_destroy(hashrate_icon);
  bitmap_layer_destroy(lastshare_icon_layer);
  gbitmap_destroy(lastshare_icon);
  bitmap_layer_destroy(reward_icon_layer);
  gbitmap_destroy(reward_icon);
  text_layer_destroy(last_update_layer);
  inverter_layer_destroy(progress_layer);

}

static void init(void) {
  font_xs = fonts_get_system_font(FONT_KEY_FONT_FALLBACK);
  font_s = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  font_m = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  font_l = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
  dark = (persist_exists(DARK_KEY)) ? persist_read_bool(DARK_KEY) : dark;
  notify = (persist_exists(NOTIFY_KEY)) ? persist_read_bool(NOTIFY_KEY) : notify;
  flick = (persist_exists(FLICK_KEY)) ? persist_read_bool(FLICK_KEY) : flick;
  update_rate = (persist_exists(UPDATE_KEY)) ? persist_read_int(UPDATE_KEY) : update_rate;


  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  app_message_init();
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(handle_bluetooth);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  if(flick) enable_flick(true);
  timeout_timer = app_timer_register(timeout_length,timeout_callback, NULL);
}

static void deinit(void) {
  window_destroy(window);
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
