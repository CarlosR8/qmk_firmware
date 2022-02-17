/* Copyright 2015-2021 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "muse.h"
#include "keymap_spanish.h"

//TAP DANCING
#define MODS_SHIFT_MASK (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT))
void tap_key(uint16_t keycode) {
  register_code(keycode);
  unregister_code(keycode);
}
// Quad
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

td_state_t cur_dance(qk_tap_dance_state_t *state);

td_state_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

// TAB HOLD = BLOQ MAYUS
static td_tap_t tabtap_state = {
    .is_press_action = true,
    .state = TD_NONE
};
void dance_tab_finished(qk_tap_dance_state_t *state, void *user_data) {
    tabtap_state.state = cur_dance(state);
    switch (tabtap_state.state) {
        case TD_SINGLE_TAP: tap_key(KC_TAB); break;
        case TD_SINGLE_HOLD: tap_key(KC_CAPS); break;
        default: break;
    }
}
// Quad ends
enum {
  TD_N = 0,
  TD_C = 1,
  M_SLSH = 2,
  TD_TAB = 3,
  TD_EXLM = 4, 
};

void matrix_init_user(void) {
    set_unicode_input_mode(UC_WIN);
};
//N and Ñ
void dance_enie_tap(qk_tap_dance_state_t *state, void *user_data) {
  //  tabtap_state.state = cur_dance(state);
  //   switch (tabtap_state.state) {
  //       case TD_SINGLE_TAP: tap_key(KC_N); break;
  //       // case TD_SINGLE_HOLD: register_code(KC_LCTL); break;
  //       case TD_DOUBLE_TAP: tap_key(KC_SEMICOLON); break;
  //       default: break;
  //   }
  unregister_code(KC_LCTL);
}
void dance_enie_finished(qk_tap_dance_state_t *state, void *user_data) {
   tabtap_state.state = cur_dance(state);
    switch (tabtap_state.state) {
        case TD_SINGLE_TAP: tap_key(KC_N); break;
        case TD_SINGLE_HOLD: register_code(KC_LCTL); break; 
        case TD_DOUBLE_TAP: tap_key(KC_SEMICOLON); break;
        default: break;
    }
  // unregister_code(KC_LCTL);
    // if (state->count == 1) {
    //     tap_key(KC_N);
    // } else {
    //     tap_key(KC_SEMICOLON);
    // }
}
//
//C and Ç
void dance_cedille_finished(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        tap_key(KC_C);
    } else {
        tap_key(KC_BSLS);
    }
}
//
// / ? ¿
void dance_slash_finished(qk_tap_dance_state_t *state, void *user_data) {
	uint8_t temp_mod = get_mods();
	clear_mods();
	if (temp_mod & MODS_SHIFT_MASK) { // Questions marks
		if (state->count == 1) {
			register_code(KC_LSFT);	
			tap_key(ES_QUOT); // ?
		}else{
			register_code(KC_LSFT);	
			tap_key(ES_IEXL); // ¿
		}
	}else{ // Slash
		register_code(KC_LSFT);	
        tap_key(ES_7); // /
	}
    set_mods(temp_mod);
}
// ! ¡
void dance_exlm_finished(qk_tap_dance_state_t *state, void *user_data) {
	uint8_t temp_mod = get_mods();
	clear_mods();
  if (state->count == 1) {
    register_code(KC_LSFT);	
    tap_key(ES_EXLM); // !
  }else{
    tap_key(ES_IEXL); // ¡
  }
  set_mods(temp_mod);
}
//
qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_N] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_enie_finished, dance_enie_tap),
  [TD_C] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_cedille_finished, NULL), 
  [M_SLSH] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_slash_finished, NULL),
  [TD_TAB] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_tab_finished, NULL),
  [TD_EXLM] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_exlm_finished, NULL), 
};

//MACROS
// Macro Declarations
enum {
    M_COMM = 0,
    M_DOT = 1,
    M_SCLN = 2,
    M_QUOT = 3,
    M_BSLS = 4,
};
// Macro Definitions
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
	uint8_t temp_mod = get_mods();
	clear_mods();
	switch(id) {
	case M_COMM: { // , <
	if (record->event.pressed) {       
		if (temp_mod & MODS_SHIFT_MASK) {
			tap_key(ES_LABK); // Shift: <
		}else{
			tap_key(KC_COMMA); //,
		}
	  }
	}
	break;
	case M_DOT: { // . >
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
			register_code(KC_LSFT);	
			tap_key(ES_LABK); // Shift: >
		}else{
			tap_key(KC_DOT); //.
		}
	  }
	}
	break;
	case M_SCLN: { // ; :
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
			register_code(KC_LSFT);	
			tap_key(KC_DOT); // Shift: :
		}else{
			register_code(KC_LSFT);	
			tap_key(KC_COMM); //;
		}
	  }
	}
	break;
  case M_QUOT: { // ' "  
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
			register_code(KC_LSFT);	
			tap_key(ES_DQUO); // Shift: "
		}else{
			tap_key(ES_QUOT); //'
		}
	  }
	}
	break;
  case M_BSLS: { // \ |
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
			register_code(KC_ALGR);	
			tap_key(ES_1); // Shift: |
		}else{
      register_code(KC_ALGR);	
			tap_key(ES_MORD); /* \ */
		}
	  }
	}
	break;
  }
  set_mods(temp_mod);
  return MACRO_NONE;
};
//

enum planck_layers {
  _QWERTY,
  _COLEMAK,
  _DVORAK,
  _LOWER,
  _RAISE,
  _PLOVER,
  _ADJUST
};

enum planck_keycodes {
  QWERTY = SAFE_RANGE,
  COLEMAK,
  DVORAK,
  PLOVER,
  BACKLIT,
  EXT_PLV
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Qwerty
 * ,-----------------------------------------------------------------------------------.
 * | Esc  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Tab  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Brite| Ctrl | Alt  | GUI  |Lower |    Space    |Raise | Left | Down |  Up  |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_QWERTY] = LAYOUT_planck_grid(
    KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
    KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    M(M_SCLN), M(M_QUOT),
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    M(M_COMM), M(M_DOT),  TD(M_SLSH), KC_ENT ,
    BACKLIT, KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
),

/* Colemak
 * ,-----------------------------------------------------------------------------------.
 * | Esc  |   Q  |   W  |   F  |   P  |   G  |   J  |   L  |   U  |   Y  |   ;  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |Tab BM|   A  |   R  |   S  |   T  |   D  |   H  |   N  |   E  |   I  |   O  |  ' " |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   K  |   M  |   ,  |   .  |   /  |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | RGB  | GUI  | Alt  |Lower |    Space    |Raise | Left | Down |  Up  |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_COLEMAK] = LAYOUT_planck_grid(
  KC_ESC,     KC_Q,     KC_W,     KC_F,     KC_P,     KC_G,     KC_J,     KC_L,     KC_U,     KC_Y,     M(M_SCLN),  KC_BSPC,
  TD(TD_TAB), GUI_T(KC_A),     ALT_T(KC_R),     SFT_T(KC_S),     CTL_T(KC_T),     KC_D,     KC_H,     TD(TD_N), SFT_T(KC_E),     ALT_T(KC_I),     GUI_T(KC_O),       M(M_QUOT),
  // TD(TD_TAB), KC_A,     KC_R,     KC_S,     KC_T,     KC_D,     KC_H,     TD(TD_N), KC_E,     KC_I,     KC_O,       M(M_QUOT),
  KC_LSFT,    KC_Z,     KC_X,     TD(TD_C), KC_V,     KC_B,     KC_K,     KC_M,     M(M_COMM),M(M_DOT), TD(M_SLSH), KC_ENT ,
  KC_LCTL,    RGB_TOG,  KC_LGUI,  KC_LALT,  LOWER,    KC_SPC,   KC_SPC,   RAISE,    KC_LEFT,  KC_DOWN,  KC_UP,      KC_RGHT
),

/* Dvorak
 * ,-----------------------------------------------------------------------------------.
 * | Tab  |   "  |   ,  |   .  |   P  |   Y  |   F  |   G  |   C  |   R  |   L  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Esc  |   A  |   O  |   E  |   U  |   I  |   D  |   H  |   T  |   N  |   S  |  /   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Shift|   ;  |   Q  |   J  |   K  |   X  |   B  |   M  |   W  |   V  |   Z  |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Brite| Ctrl | Alt  | GUI  |Lower |    Space    |Raise | Left | Down |  Up  |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_DVORAK] = LAYOUT_planck_grid(
    KC_TAB,  KC_QUOT, KC_COMM, KC_DOT,  KC_P,    KC_Y,    KC_F,    KC_G,    KC_C,    KC_R,    KC_L,    KC_BSPC,
    KC_ESC,  KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,    KC_T,    KC_N,    KC_S,    KC_SLSH,
    KC_LSFT, KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,    KC_ENT ,
    BACKLIT, KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
),

/* Lower
 * ,-----------------------------------------------------------------------------------.
 * |   ~  |   !  |   @  |   (  |   )  |   %  |   ^  |   7  |   8  |   9  |   -  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   &  |   #  |   €  |   [  |   ]  |   *  |   ´  |   4  |   5  |   6  |   +  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |   $  |   {  |   }  |   ¨  |   `  |   1  |   2  |   3  | \ |  |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | RGB  | GUI  | Alt  |Lower |             |Adjust|   0  |   .  | Vol+ | Play |
 * `-----------------------------------------------------------------------------------'
 */
[_LOWER] = LAYOUT_planck_grid(
    ES_TILD, TD(TD_EXLM), ES_AT,    ES_LPRN, ES_RPRN,  KC_PERC, ES_CIRC,  KC_7,    KC_8,   KC_9, ES_MINS, KC_BSPC,
    ES_AMPR, ES_HASH,     ES_EURO,  ES_LBRC, ES_RBRC,  ES_ASTR, ES_ACUT,  KC_4,    KC_5,   KC_6, ES_PLUS, ES_EQL,
    _______, _______,     ES_DLR,   ES_LCBR, ES_RCBR,  ES_DIAE, ES_GRV,  KC_1,    KC_2,   KC_3, M(M_BSLS),  _______,
    _______, _______,     _______,  _______, _______,  _______, _______,  _______, KC_0,   KC_DOT, KC_VOLU, KC_MPLY
),

/* Raise 
 * ,-----------------------------------------------------------------------------------.
 * |      |  F1  |  F2  |  F3  |  F4  |      |      | Home |  Up  |  End |Pg Up | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |  F5  |  F6  |  F7  |  F8  |      |      | Left | Down |Right |Pg Dn |  \ | |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |  F9  |  F10 |  F11 |  F12 |      |      |ISO # |ISO / |Pg Up |Pg Dn |Insert|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | RGB  | GUI  | Alt  |Adjust|             |Raise | Next | Vol- | Vol+ | Play |
 * `-----------------------------------------------------------------------------------'
 */
[_RAISE] = LAYOUT_planck_grid(
    _______,  KC_F1,   KC_F2,    KC_F3,    KC_F4,   _______,  _______,  KC_HOME, KC_UP,   KC_END,    KC_PGUP,  KC_BSPC,
    _______,  KC_F5,   KC_F6,    KC_F7,    KC_F8,   _______,  _______,  KC_LEFT, KC_DOWN, KC_RIGHT,  KC_PGDN,  M(M_BSLS),
    _______, KC_F9,   KC_F10,   KC_F11,   KC_F12,  _______,  _______,  KC_NUHS, KC_NUBS, KC_PGUP,   KC_PGDN,  KC_INS,
    _______,    _______,  _______,  _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
),

/* Plover layer (http://opensteno.org)
 * ,-----------------------------------------------------------------------------------.
 * |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |   S  |   T  |   P  |   H  |   *  |   *  |   F  |   P  |   L  |   T  |   D  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |   S  |   K  |   W  |   R  |   *  |   *  |   R  |   B  |   G  |   S  |   Z  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Exit |      |      |   A  |   O  |             |   E  |   U  |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_PLOVER] = LAYOUT_planck_grid(
    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1   ,
    XXXXXXX, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
    XXXXXXX, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    EXT_PLV, XXXXXXX, XXXXXXX, KC_C,    KC_V,    XXXXXXX, XXXXXXX, KC_N,    KC_M,    XXXXXXX, XXXXXXX, XXXXXXX
),

/* Adjust (Lower + Raise)
 *                      v------------------------RGB CONTROL--------------------v
 * ,-----------------------------------------------------------------------------------.
 * |      | Reset|Debug | RGB  |RGBMOD| HUE+ | HUE- | SAT+ | SAT- |BRGTH+|BRGTH-|  Del |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |MUSmod|Aud on|Audoff|AGnorm|AGswap|Qwerty|Colemk|Dvorak|Plover|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |Voice-|Voice+|Mus on|Musoff|MIDIon|MIDIof|TermOn|TermOf|      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_ADJUST] = LAYOUT_planck_grid(
    _______, RESET,   DEBUG,   RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD,  RGB_VAI, RGB_VAD, KC_DEL ,
    _______, _______, MU_MOD,  AU_ON,   AU_OFF,  AG_NORM, AG_SWAP, QWERTY,  COLEMAK,  DVORAK,  PLOVER,  _______,
    _______, MUV_DE,  MUV_IN,  MU_ON,   MU_OFF,  MI_ON,   MI_OFF,  TERM_ON, TERM_OFF, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______, _______
)

};

#ifdef AUDIO_ENABLE
  float plover_song[][2]     = SONG(PLOVER_SOUND);
  float plover_gb_song[][2]  = SONG(PLOVER_GOODBYE_SOUND);
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        print("mode just switched to qwerty and this is a huge string\n");
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case COLEMAK:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_COLEMAK);
      }
      return false;
      break;
    case DVORAK:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_DVORAK);
      }
      return false;
      break;
    case BACKLIT:
      if (record->event.pressed) {
        register_code(KC_RSFT);
        #ifdef BACKLIGHT_ENABLE
          backlight_step();
        #endif
        #ifdef KEYBOARD_planck_rev5
          writePinLow(E6);
        #endif
      } else {
        unregister_code(KC_RSFT);
        #ifdef KEYBOARD_planck_rev5
          writePinHigh(E6);
        #endif
      }
      return false;
      break;
    case PLOVER:
      if (record->event.pressed) {
        #ifdef AUDIO_ENABLE
          stop_all_notes();
          PLAY_SONG(plover_song);
        #endif
        layer_off(_RAISE);
        layer_off(_LOWER);
        layer_off(_ADJUST);
        layer_on(_PLOVER);
        if (!eeconfig_is_enabled()) {
            eeconfig_init();
        }
        keymap_config.raw = eeconfig_read_keymap();
        keymap_config.nkro = 1;
        eeconfig_update_keymap(keymap_config.raw);
      }
      return false;
      break;
    case EXT_PLV:
      if (record->event.pressed) {
        #ifdef AUDIO_ENABLE
          PLAY_SONG(plover_gb_song);
        #endif
        layer_off(_PLOVER);
      }
      return false;
      break;
  }
  return true;
}

bool muse_mode = false;
uint8_t last_muse_note = 0;
uint16_t muse_counter = 0;
uint8_t muse_offset = 70;
uint16_t muse_tempo = 50;

bool encoder_update_user(uint8_t index, bool clockwise) {
  if (muse_mode) {
    if (IS_LAYER_ON(_RAISE)) {
      if (clockwise) {
        muse_offset++;
      } else {
        muse_offset--;
      }
    } else {
      if (clockwise) {
        muse_tempo+=1;
      } else {
        muse_tempo-=1;
      }
    }
  } else {
    if (clockwise) {
      #ifdef MOUSEKEY_ENABLE
        tap_code(KC_MS_WH_DOWN);
      #else
        tap_code(KC_PGDN);
      #endif
    } else {
      #ifdef MOUSEKEY_ENABLE
        tap_code(KC_MS_WH_UP);
      #else
        tap_code(KC_PGUP);
      #endif
    }
  }
    return true;
}

bool dip_switch_update_user(uint8_t index, bool active) {
    switch (index) {
        case 0: {
			#ifdef AUDIO_ENABLE
				static bool play_sound = false;
			#endif
            if (active) {
				#ifdef AUDIO_ENABLE
					if (play_sound) { PLAY_SONG(plover_song); }
				#endif
				layer_on(_ADJUST);
            } else {
				#ifdef AUDIO_ENABLE
					if (play_sound) { PLAY_SONG(plover_gb_song); }
				#endif
				layer_off(_ADJUST);
            }
			#ifdef AUDIO_ENABLE
				play_sound = true;
			#endif
            break;
        }
        case 1:
            if (active) {
                muse_mode = true;
            } else {
                muse_mode = false;
            }
    }
    return true;
}

void matrix_scan_user(void) {
#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    } else {
        if (muse_counter) {
            stop_all_notes();
            muse_counter = 0;
        }
    }
#endif
}

bool music_mask_user(uint16_t keycode) {
  switch (keycode) {
    case RAISE:
    case LOWER:
      return false;
    default:
      return true;
  }
}
