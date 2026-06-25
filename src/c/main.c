#include <pebble.h>

typedef enum {
  MODE_WARM_UP,
  MODE_MATCH,
  MODE_BETWEEN_GAMES,
  MODE_MATCH_REVIEW,
  MODE_COUNT
} AppMode;

static Window *s_window;
static TextLayer *s_title_layer;
static TextLayer *s_body_layer;
static TextLayer *s_hint_layer;
static AppMode s_mode = MODE_WARM_UP;
static int s_lost_streak;
static int s_between_step;
static int s_review_step;
static int s_choice_index;
static int s_headroom_rating = 3;
static bool s_in_menu = true;
static bool s_match_prompt_active;

static const char *s_mode_names[] = {
  "Warm-up",
  "Match Mode",
  "Between Games",
  "Match Review"
};

static const char *s_plans[] = {
  "Control length",
  "Push pace",
  "Net pressure"
};

static const char *s_results[] = {"win", "loss", "skip"};
static const char *s_headroom[] = {"good", "mixed", "poor"};
static const char *s_issues[] = {"rushed", "tired", "tilt", "fear", "other"};

static void show_screen(const char *title, const char *body, const char *hint) {
  text_layer_set_text(s_title_layer, title);
  text_layer_set_text(s_body_layer, body);
  text_layer_set_text(s_hint_layer, hint);
}

static void show_mode_menu(void) {
  s_in_menu = true;
  s_match_prompt_active = false;
  static char body[96];
  snprintf(body, sizeof(body), "%s\n\nUP/DOWN mode\nSELECT start", s_mode_names[s_mode]);
  show_screen("Headroom", body, "Hold SELECT: emergency");
}

static void show_warm_up(void) {
  show_screen("Warm-up",
              "Shoulder loose\nCalves springy\nBreathing calm\nFirst 3 simple",
              "SELECT: modes");
}

static void show_match_mode(void) {
  s_lost_streak = 0;
  s_match_prompt_active = false;
  show_screen("Match Mode", "Breathe.\nSee early.\nOne rally.", "UP won  DOWN lost");
}

static void show_reset_prompt(void) {
  s_match_prompt_active = true;
  vibes_short_pulse();
  show_screen("Reset", "Breathe.\nSee early.\nOne rally.", "SELECT: back");
}

static void show_tilt_reset(void) {
  s_match_prompt_active = true;
  vibes_double_pulse();
  show_screen("Break spiral",
              "Clear long.\nRecover base.\nMake them play.",
              "SELECT: back");
}

static void show_emergency_reset(void) {
  s_match_prompt_active = true;
  static const uint32_t segments[] = {200, 100, 200, 100, 400};
  VibePattern pattern = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments)
  };
  vibes_enqueue_custom_pattern(pattern);
  show_screen("Emergency", "Feet.\nBreath.\nShuttle.\nNext shot.", "SELECT: back");
}

static void show_between_games(void) {
  static char body[96];

  if (s_between_step == 0) {
    snprintf(body, sizeof(body), "Headroom?\n\n%d", s_headroom_rating);
    show_screen("Between Games", body, "UP/DOWN  SELECT");
  } else {
    snprintf(body, sizeof(body), "Plan:\n\n%s", s_plans[s_choice_index]);
    show_screen("Between Games", body, "UP/DOWN  SELECT");
  }
}

static void show_review(void) {
  static char body[96];

  if (s_review_step == 0) {
    snprintf(body, sizeof(body), "Result:\n\n%s", s_results[s_choice_index]);
  } else if (s_review_step == 1) {
    snprintf(body, sizeof(body), "Headroom:\n\n%s", s_headroom[s_choice_index]);
  } else {
    snprintf(body, sizeof(body), "Main issue:\n\n%s", s_issues[s_choice_index]);
  }

  show_screen("Match Review", body, "UP/DOWN  SELECT");
}

static void open_mode(void) {
  s_choice_index = 0;
  s_between_step = 0;
  s_review_step = 0;

  s_in_menu = false;
  s_match_prompt_active = false;

  switch (s_mode) {
    case MODE_WARM_UP:
      show_warm_up();
      break;
    case MODE_MATCH:
      show_match_mode();
      break;
    case MODE_BETWEEN_GAMES:
      s_headroom_rating = 3;
      show_between_games();
      break;
    case MODE_MATCH_REVIEW:
      show_review();
      break;
    default:
      show_mode_menu();
      break;
  }
}

static void cycle_choice(int delta, int count) {
  s_choice_index += delta;
  if (s_choice_index < 0) {
    s_choice_index = count - 1;
  } else if (s_choice_index >= count) {
    s_choice_index = 0;
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_in_menu) {
    s_mode = (s_mode + MODE_COUNT - 1) % MODE_COUNT;
    show_mode_menu();
  } else if (s_mode == MODE_MATCH) {
    s_match_prompt_active = false;
    s_lost_streak = 0;
    show_screen("Won point", "Good.\nNext rally.", "UP won  DOWN lost");
  } else if (s_mode == MODE_BETWEEN_GAMES) {
    if (s_between_step == 0 && s_headroom_rating < 5) {
      s_headroom_rating++;
    } else if (s_between_step == 1) {
      cycle_choice(-1, ARRAY_LENGTH(s_plans));
    }
    show_between_games();
  } else if (s_mode == MODE_MATCH_REVIEW) {
    int count = (s_review_step == 0) ? ARRAY_LENGTH(s_results) :
                (s_review_step == 1) ? ARRAY_LENGTH(s_headroom) : ARRAY_LENGTH(s_issues);
    cycle_choice(-1, count);
    show_review();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_in_menu) {
    s_mode = (s_mode + 1) % MODE_COUNT;
    show_mode_menu();
  } else if (s_mode == MODE_MATCH) {
    s_match_prompt_active = false;
    s_lost_streak++;
    if (s_lost_streak >= 3) {
      s_lost_streak = 0;
      show_tilt_reset();
    } else {
      show_screen("Lost point", "Reset now.\nOne rally.", "UP won  DOWN lost");
    }
  } else if (s_mode == MODE_BETWEEN_GAMES) {
    if (s_between_step == 0 && s_headroom_rating > 1) {
      s_headroom_rating--;
    } else if (s_between_step == 1) {
      cycle_choice(1, ARRAY_LENGTH(s_plans));
    }
    show_between_games();
  } else if (s_mode == MODE_MATCH_REVIEW) {
    int count = (s_review_step == 0) ? ARRAY_LENGTH(s_results) :
                (s_review_step == 1) ? ARRAY_LENGTH(s_headroom) : ARRAY_LENGTH(s_issues);
    cycle_choice(1, count);
    show_review();
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_in_menu) {
    open_mode();
  } else if (s_mode == MODE_MATCH) {
    if (s_match_prompt_active) {
      show_match_mode();
    } else {
      show_reset_prompt();
    }
  } else if (s_mode == MODE_BETWEEN_GAMES) {
    if (s_match_prompt_active) {
      show_between_games();
      s_match_prompt_active = false;
    } else if (s_between_step == 0) {
      s_between_step = 1;
      s_choice_index = 0;
      show_between_games();
    } else if (s_between_step == 1) {
      s_between_step = 2;
      show_screen("Plan set", s_plans[s_choice_index], "SELECT: modes");
    } else {
      show_mode_menu();
    }
  } else if (s_mode == MODE_MATCH_REVIEW) {
    if (s_match_prompt_active) {
      show_review();
      s_match_prompt_active = false;
    } else if (s_review_step < 2) {
      s_review_step++;
      s_choice_index = 0;
      show_review();
    } else if (s_review_step == 2) {
      s_review_step = 3;
      show_screen("Review saved", "Done.\nLet it go.", "SELECT: modes");
    } else {
      show_mode_menu();
    }
  } else {
    show_mode_menu();
  }
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_in_menu = false;
  show_emergency_reset();
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_mode_menu();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 500, select_long_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

static TextLayer *make_text_layer(GRect frame, GFont font, GTextAlignment alignment) {
  TextLayer *layer = text_layer_create(frame);
  text_layer_set_background_color(layer, GColorClear);
  text_layer_set_text_color(layer, GColorBlack);
  text_layer_set_font(layer, font);
  text_layer_set_text_alignment(layer, alignment);
  return layer;
}

static void window_load(Window *window) {
  Layer *root_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root_layer);

  s_title_layer = make_text_layer(GRect(4, 4, bounds.size.w - 8, 28),
                                  fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
                                  GTextAlignmentCenter);
  s_body_layer = make_text_layer(GRect(8, 36, bounds.size.w - 16, 100),
                                 fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
                                 GTextAlignmentCenter);
  s_hint_layer = make_text_layer(GRect(4, bounds.size.h - 24, bounds.size.w - 8, 20),
                                 fonts_get_system_font(FONT_KEY_GOTHIC_18),
                                 GTextAlignmentCenter);

  layer_add_child(root_layer, text_layer_get_layer(s_title_layer));
  layer_add_child(root_layer, text_layer_get_layer(s_body_layer));
  layer_add_child(root_layer, text_layer_get_layer(s_hint_layer));

  show_mode_menu();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_title_layer);
  text_layer_destroy(s_body_layer);
  text_layer_destroy(s_hint_layer);
}

static void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_window, true);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
