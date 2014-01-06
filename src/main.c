#include <pebble.h>

#define NUM_POINTS_DEFAULT 0
#define PLAYER1_POINTS_PKEY 1
#define PLAYER2_POINTS_PKEY 2
#define PLAYER1_SETS_PKEY 3
#define PLAYER2_SETS_PKEY 4

static Window *window;

static ActionBarLayer *action_bar;

static TextLayer *header_text_layer;
static TextLayer *body_text_layer;
static TextLayer *label_text_layer;
static int player1points = NUM_POINTS_DEFAULT;
static int player2points = NUM_POINTS_DEFAULT;
static int player1sets = NUM_POINTS_DEFAULT;
static int player2sets = NUM_POINTS_DEFAULT;

static void update_text() {
  static char body_text[50];
  static char label_text[50];
  snprintf(body_text, sizeof(body_text), "%u - %u", player1points, player2points);
  text_layer_set_text(body_text_layer, body_text);
  snprintf(label_text, sizeof(label_text), "%u sets - %u sets", player1sets, player2sets);
  text_layer_set_text(label_text_layer, label_text);
}

static void player1_click_handler(ClickRecognizerRef recognizer, void *context) {
  player1points++;
  if ((player1points >= 11) && (player1points > player2points + 1)) {
		player1sets++;
	    player1points = 0;
	    player2points = 0;
  }
  update_text();
}

static void player2_click_handler(ClickRecognizerRef recognizer, void *context) {
  player2points++;
  if ((player2points >= 11) && (player2points > player1points + 1)) {
		player2sets++;
	    player1points = 0;
	    player2points = 0;
  }
  update_text();
}

static void reset_click_handler(ClickRecognizerRef recognizer, void *context) {
  player1points = NUM_POINTS_DEFAULT;
  player2points = NUM_POINTS_DEFAULT;
  player1sets = NUM_POINTS_DEFAULT;
  player2sets = NUM_POINTS_DEFAULT;
  update_text();
}

static void click_config_provider(void *context) {
  const uint16_t repeat_interval_ms = 50;
  const uint16_t reset_hold_ms = 3000;
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) player1_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) player2_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, reset_hold_ms, (ClickHandler) reset_click_handler, NULL);
}

static void window_load(Window *me) {
  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, me);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

  Layer *layer = window_get_root_layer(me);
  const int16_t width = layer_get_frame(layer).size.w - ACTION_BAR_WIDTH - 3;

  header_text_layer = text_layer_create(GRect(4, 0, width, 60));
  text_layer_set_font(header_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(header_text_layer, GColorClear);
  text_layer_set_text(header_text_layer, "Bordtennis");
  layer_add_child(layer, text_layer_get_layer(header_text_layer));

  body_text_layer = text_layer_create(GRect(4, 44, width, 60));
  text_layer_set_font(body_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_background_color(body_text_layer, GColorClear);
  layer_add_child(layer, text_layer_get_layer(body_text_layer));

  label_text_layer = text_layer_create(GRect(4, 44 + 32, width, 60));
  text_layer_set_font(label_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_background_color(label_text_layer, GColorClear);
  text_layer_set_text(label_text_layer, "0 set - 0 set");
  layer_add_child(layer, text_layer_get_layer(label_text_layer));

  update_text();
}

static void window_unload(Window *window) {
  text_layer_destroy(header_text_layer);
  text_layer_destroy(body_text_layer);
  text_layer_destroy(label_text_layer);

  action_bar_layer_destroy(action_bar);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  // Get the count from persistent storage for use if it exists, otherwise use the default
  player1points = persist_exists(PLAYER1_POINTS_PKEY) ? persist_read_int(PLAYER1_POINTS_PKEY) : NUM_POINTS_DEFAULT;
  player2points = persist_exists(PLAYER2_POINTS_PKEY) ? persist_read_int(PLAYER2_POINTS_PKEY) : NUM_POINTS_DEFAULT;
  player1sets = persist_exists(PLAYER1_SETS_PKEY) ? persist_read_int(PLAYER1_SETS_PKEY) : NUM_POINTS_DEFAULT;
  player2sets = persist_exists(PLAYER2_SETS_PKEY) ? persist_read_int(PLAYER2_SETS_PKEY) : NUM_POINTS_DEFAULT;

  window_stack_push(window, true /* Animated */);
}

static void deinit(void) {
  // Save the count into persistent storage on app exit
  persist_write_int(PLAYER1_POINTS_PKEY, player1points);
  persist_write_int(PLAYER2_POINTS_PKEY, player2points);
  persist_write_int(PLAYER1_SETS_PKEY, player1sets);
  persist_write_int(PLAYER2_SETS_PKEY, player2sets);

  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}