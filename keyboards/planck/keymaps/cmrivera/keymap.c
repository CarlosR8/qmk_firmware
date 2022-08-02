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
#include "keymap_french.h"
// Defining language macros
#define ES 1
#define EN 2
#define FR 3

int language = ES;

// Layers
enum planck_layers {
  _COLEMAK,
  _GAMING,
  _LOWER,
  _RAISE,
  _ADJUST,
  _MOUSE,
};

enum planck_keycodes {
  COLEMAK = SAFE_RANGE,
  GAMING,
  BACKLIT,
  EXT_PLV,
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define MOUSE MO(_MOUSE)
//

//TAP DANCING
#define MODS_SHIFT_MASK (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT))
#define MODS_CTRL_MASK (MOD_BIT(KC_LCTRL)|MOD_BIT(KC_RCTRL))
void tap_key(uint16_t keycode) {
  register_code16(keycode);
  unregister_code16(keycode);
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
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP; // interrution removed to combine tap dancing and home row mod
        // if (!state->pressed) return TD_SINGLE_TAP;
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

// Register key state
static td_tap_t keytap_state = {
    .is_press_action = true,
    .state = TD_NONE
};
void dance_tab_finished(qk_tap_dance_state_t *state, void *user_data) {
    keytap_state.state = cur_dance(state);
    switch (keytap_state.state) {
        case TD_SINGLE_HOLD: register_code(KC_LSHIFT); break;
        case TD_DOUBLE_HOLD: tap_key(KC_CAPS); break;
        default: break;
    }
}
void dance_tab_reset(qk_tap_dance_state_t *state, void *user_data) {
  unregister_code(KC_LSHIFT);
}
// Quad ends
enum {
  TD_N = 0,
  TD_C = 1,
  TD_SLSH = 2,
  TD_TAB = 3,
  TD_EXLM = 4, 
  TD_MOUSE = 5,
};

void matrix_init_user(void) {
    set_unicode_input_mode(UC_WIN);
};
//N and Ñ
// void dance_enie_reset(qk_tap_dance_state_t *state, void *user_data) {
//   //  keytap_state.state = cur_dance(state);
//   //   switch (keytap_state.state) {
//   //       case TD_SINGLE_TAP: tap_key(KC_N); break;
//   //       // case TD_SINGLE_HOLD: register_code(KC_LCTL); break;
//   //       case TD_DOUBLE_TAP: tap_key(KC_SEMICOLON); break;
//   //       default: break;
//   //   }
//   unregister_code(KC_LCTL);
// }
// void dance_enie_finished(qk_tap_dance_state_t *state, void *user_data) {
//    keytap_state.state = cur_dance(state);
//     switch (keytap_state.state) {
//         case TD_SINGLE_TAP: tap_key(KC_N); break;
//         case TD_SINGLE_HOLD: register_code(KC_LCTL); break; 
//         case TD_DOUBLE_TAP: tap_key(KC_SEMICOLON); break;
//         default: break;
//     }
//   // unregister_code(KC_LCTL);
//     // if (state->count == 1) {
//     //     tap_key(KC_N);
//     // } else {
//     //     tap_key(KC_SEMICOLON);
//     // }
// }
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
			// register_code(KC_LSFT);	
			// tap_key(ES_QUOT); // ?
      register_code16(S(ES_QUOT));
      unregister_code16(S(ES_QUOT));
		}else{
			register_code(KC_LSFT);	
			tap_key(ES_IEXL); // ¿
		}
	}else{ // Slash
		register_code(KC_LSFT);	
    tap_key(ES_7); // /
	}
  set_mods(temp_mod);

  // uint8_t temp_mod = get_mods(); // doesn't work better
  // keytap_state.state = cur_dance(state);
  // switch (keytap_state.state) {
  //     case TD_SINGLE_TAP: 
  //       if (temp_mod & MODS_SHIFT_MASK) {
  //         // register_code(KC_LSFT);	
  //         // tap_key(ES_QUOT); // ?
  //         register_code16(S(ES_QUOT));
  //         unregister_code16(S(ES_QUOT));
  //       }else{
  //         register_code(KC_LSFT);	
  //         tap_key(ES_7); // /
  //       }
  //       break;
  //     case TD_DOUBLE_TAP: 
  //       if (temp_mod & MODS_SHIFT_MASK) {
  //         register_code(KC_LSFT);	
  //         tap_key(ES_IEXL); // ¿
  //       }
  //       break;
  //     default: break;
  // }
  // set_mods(temp_mod);
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

// Left arrow or activate Mouse layer
void dance_mouse_tap(qk_tap_dance_state_t *state, void *user_data) {
  layer_on(_RAISE);
}

void dance_mouse_finish(qk_tap_dance_state_t *state, void *user_data) {
keytap_state.state = cur_dance(state);
    switch (keytap_state.state) {
        case TD_DOUBLE_HOLD: layer_on(_MOUSE); break;
        default: break;
    }
}

void dance_mouse_reset(qk_tap_dance_state_t *state, void *user_data) {
  layer_off(_RAISE);
  layer_off(_MOUSE);
}
//
qk_tap_dance_action_t tap_dance_actions[] = {
  // [TD_N] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dance_enie_finished, dance_enie_reset, 160),
  [TD_C] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dance_cedille_finished, NULL, 200), 
  [TD_SLSH] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dance_slash_finished, NULL, 170),
  [TD_TAB] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dance_tab_finished, dance_tab_reset, 150),
  [TD_EXLM] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dance_exlm_finished, NULL, 200), 
  [TD_MOUSE] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(dance_mouse_tap, dance_mouse_finish, dance_mouse_reset, 200), 
};

// Function to send key based on language
void tap_key_lang(uint16_t modifier_es, uint16_t key_es, uint16_t modifier_en, uint16_t key_en, uint16_t modifier_fr, uint16_t key_fr){
  switch (language)
  {
  case ES:
    register_code(modifier_es);
    tap_key(key_es);
    break;
  case EN:
    register_code(modifier_en);
    tap_key(key_en);
    break;
  case FR:
    register_code(modifier_fr);
    tap_key(key_fr);
    break;
  }
  return;
}

//MACROS
// Macro Declarations
enum {
    M_COMM = 0,
    M_DOT = 1,
    M_SCLN = 2,
    M_QUOT = 3,
    M_BSLS = 4,
    M_TEXT = 5,
    M_TEXT1 = 6,
    M_LSPOT = 7,
    M_ES=8,
    M_EN=9,
    M_FR=10,
    M_MINS=11,
    M_EQL=12,
    M_LBRC=13,
    M_PLUS=14,
    M_LCBR=15,
    M_RCBR=16,
    M_RBRC=17,
    M_LPRN=18,
    M_RPRN=19,
    M_TILD=20,
    M_AMPR=21,
    M_HASH=22,
    M_AT=23,
    M_DLR=24,
    M_EURO=25,
    M_PERC=26,
    M_ASTR=27,
    M_DIAE=28,
    M_CIRC=29,
    M_ACUT=30,
    M_GRV=31,
    M_Q=32,
    M_A=33,
    M_W=34,
    M_M=35,
    M_Z=36,
    M_0=37,
    M_1=38,
    M_2=39,
    M_3=40,
    M_4=41,
    M_5=42,
    M_6=43,
    M_7=44,
    M_8=45,
    M_9=46,
};
// Macro Definitions
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
	uint8_t temp_mod = get_mods();
	clear_mods();
	switch(id) {
  case M_MINS: { // - _
	if (record->event.pressed) {       
		if (temp_mod & MODS_SHIFT_MASK) {
			tap_key_lang(_______,ES_UNDS,KC_LSFT,KC_MINS,_______,FR_UNDS); // Shift: _
		}else{
			tap_key_lang(_______,ES_MINS,_______,KC_MINS,_______,FR_MINS); //-
		}
	}
	}
  break;
  case M_EQL: { // =
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_EQL,_______,KC_EQL,_______,FR_EQL); // =
	}
	}
	break;
  case M_PLUS: { // +
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_PLUS,KC_LSFT,KC_EQL,_______,FR_PLUS); // +
	}
	}
	break;
  case M_LBRC: { // [
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_LBRC,_______,KC_LBRC,_______,FR_LBRC); // [
	}
	}
	break;
  case M_LCBR: { // {
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_LCBR,KC_LSFT,KC_LBRC,_______,FR_LCBR); // {
	}
	}
	break;
  case M_RBRC: { // ]
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_RBRC,_______,KC_RBRC,_______,FR_RBRC); // ]
	}
	}
	break;
  case M_RCBR: { // }
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_RCBR,KC_LSFT,KC_RBRC,_______,FR_RCBR); // }
	}
	}
	break;
  case M_LPRN: { // (
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_LPRN,KC_LSFT,KC_9,_______,FR_LPRN); // (
	}
	}
	break;
  case M_RPRN: { // )
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_RPRN,KC_LSFT,KC_0,_______,FR_RPRN); // )
	}
	}
	break;
  case M_TILD: { // ~
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_TILD,KC_LSFT,KC_GRV,_______,FR_TILD); // ~
	}
	}
	break;
  case M_AMPR: { // &
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_AMPR,KC_LSFT,KC_7,_______,FR_AMPR); // &
	}
	}
	break;
  case M_HASH: { // #
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_HASH,KC_LSFT,KC_3,_______,FR_HASH); // #
	}
	}
	break;
  case M_AT: { // @
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_AT,KC_LSFT,KC_2,_______,FR_AT); // @
	}
	}
	break;
  case M_DLR: { // $
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_DLR,KC_LSFT,KC_4,_______,FR_DLR); // $
	}
	}
	break;
case M_EURO: { // €
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_EURO,_______,_______,_______,FR_EURO); // €
	}
	}
	break;
case M_PERC: { // %
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_PERC,KC_LSFT,KC_5,_______,FR_PERC); // %
	}
	}
	break;
case M_ASTR: { // *
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_ASTR,KC_LSFT,KC_8,_______,FR_ASTR); // *
	}
	}
	break;
case M_DIAE: { // ¨
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_DIAE,_______,_______,_______,FR_DIAE); // ¨
	}
	}
	break;
case M_CIRC: { // ^
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_CIRC,KC_LSFT,KC_6,_______,FR_CIRC); // ^
	}
	}
	break;
case M_ACUT: { // ´
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_ACUT,_______,_______,_______,_______); // ´
	}
	}
	break;
case M_GRV: { // `
	if (record->event.pressed) {       
		tap_key_lang(_______,ES_GRV,_______,KC_GRV,_______,FR_GRV); // `
	}
	}
	break;
case M_Q: { // Q
	if (record->event.pressed) {       
    set_mods(temp_mod);
    tap_key_lang(_______,KC_Q,_______,KC_Q,_______,KC_A); // q
	}
	}
	break;
case M_A: { // A
	if (record->event.pressed) {       
    set_mods(temp_mod);
	  tap_key_lang(_______,KC_A,_______,KC_A,_______,KC_Q);
	}
	}
	break;
  case M_W: { // W
	if (record->event.pressed) {       
    set_mods(temp_mod);
		tap_key_lang(_______,KC_W,_______,KC_W,_______,KC_Z);
	}
	}
	break;
  case M_M: { // M
	if (record->event.pressed) {       
    set_mods(temp_mod);
		tap_key_lang(_______,KC_M,_______,KC_M,_______,KC_SCLN);
	}
	}
	break;
  case M_Z: { // Z
	if (record->event.pressed) {       
    set_mods(temp_mod);
		tap_key_lang(_______,KC_Z,_______,KC_Z,_______,KC_W);
	}
	}
	break;
	case M_COMM: { // , <
	if (record->event.pressed) {       
		if (temp_mod & MODS_SHIFT_MASK) {
			tap_key_lang(_______,ES_LABK,KC_LSFT,KC_COMM,_______,FR_LABK); // Shift: <
		}else{
			tap_key_lang(_______,ES_COMM,_______,KC_COMM,_______,FR_COMM); //,
		}
	  }
	}
	break;
	case M_DOT: { // . >
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,ES_RABK,KC_LSFT,KC_DOT,_______,FR_RABK); // >
		}else{
			tap_key_lang(_______,ES_DOT,_______,KC_DOT,_______,FR_DOT); //.
		}
	  }
	}
	break;
	case M_SCLN: { // ; :
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,ES_COLN,KC_LSFT,KC_SCLN,_______,FR_COLN); // :
		}else{
      tap_key_lang(_______,ES_SCLN,_______,KC_SCLN,_______,FR_SCLN); // ;
		}
	  }
	}
	break;
  case M_QUOT: { // ' "  
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
			tap_key_lang(_______,ES_DQUO,KC_LSFT,KC_QUOT,_______,FR_DQUO); // Shift: "
		}else{
			tap_key_lang(_______,ES_QUOT,_______,KC_QUOT,_______,FR_QUOT); //'
		}
	  }
	}
	break;
  case M_BSLS: { // \ |
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,ES_PIPE,KC_LSFT,KC_BSLS,_______,FR_PIPE); // Shift: |
		}else{
      tap_key_lang(_______,ES_BSLS,_______,KC_BSLS,_______,FR_BSLS); /* \ */
		}
	  }
	}
	break;
  //Addind number support in french
  case M_0: { // 0
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_0,_______,KC_0,_______,FR_0); // 0
		}else{
      tap_key_lang(_______,KC_0,_______,KC_0,_______,FR_0); // 0
		}
	  }
	}    
  break;
  case M_1: { // 1
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_1,_______,KC_1,_______,FR_1); // 1
		}else{
      tap_key_lang(_______,KC_1,_______,KC_1,_______,FR_1); // 1
		}
	  }
	}    
  break;
  case M_2: { // 2
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_2,_______,KC_2,_______,FR_2); // 2
		}else{
      tap_key_lang(_______,KC_2,_______,KC_2,_______,FR_2); // 2
		}
	  }
	}    
  break;
  case M_3: { // 3
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_3,_______,KC_3,_______,FR_3); // 3
		}else{
      tap_key_lang(_______,KC_3,_______,KC_3,_______,FR_3); // 3
		}
	  }
	}    
  break;
  case M_4: { // 4
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_4,_______,KC_4,_______,FR_4); // 4
		}else{
      tap_key_lang(_______,KC_4,_______,KC_4,_______,FR_4); // 4
		}
	  }
	}    
  break;
  case M_5: { // 5
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_5,_______,KC_5,_______,FR_5); // 5
		}else{
      tap_key_lang(_______,KC_5,_______,KC_5,_______,FR_5); // 5
		}
	  }
	}    
  break;
  case M_6: { // 6
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_6,_______,KC_6,_______,FR_6); // 6
		}else{
      tap_key_lang(_______,KC_6,_______,KC_6,_______,FR_6); // 6
		}
	  }
	}    
  break;
  case M_7: { // 7
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_7,_______,KC_7,_______,FR_7); // 7
		}else{
      tap_key_lang(_______,KC_7,_______,KC_7,_______,FR_7); // 7
		}
	  }
	}    
  break;
  case M_8: { // 8
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_8,_______,KC_8,_______,FR_8); // 8
		}else{
      tap_key_lang(_______,KC_8,_______,KC_8,_______,FR_8); // 8
		}
	  }
	}    
  break;
  case M_9: { // 9
	  if (record->event.pressed) {     
		if (temp_mod & MODS_SHIFT_MASK) {
      tap_key_lang(_______,KC_9,_______,KC_9,_______,FR_9); // 9
		}else{
      tap_key_lang(_______,KC_9,_______,KC_9,_______,FR_9); // 9
		}
	  }
	}    
  break;

  //
  case M_TEXT: { // Sending text 1 
	  if (record->event.pressed) {
      switch (language)
    {
    case ES:
      SEND_STRING("setxkbmap es");
      break;
    case EN:
      SEND_STRING("setxkbmap es");
      break;
    case FR:
      SEND_STRING("setxb;qp es");
      break;
    }
	  }
	}
	break;
  case M_TEXT1: { // Sending text 2
  	if (record->event.pressed) {
      switch (language)
    {
    case ES:
      SEND_STRING("Carlos RIVERA");
      break;
    case EN:
      SEND_STRING("Carlos RIVERA");
      break;
    case FR:
      SEND_STRING("Cqrlos RIVERQ");
      break;
    }
	  }
	}
	break;
  case M_LSPOT: { // Send AltGr + L to trigger Spotify like
	  if (record->event.pressed) {     
      register_code(KC_ALGR);
      tap_key(KC_L);
	  }
	}
	break;
  // Set language
  case M_ES: {
	  if (record->event.pressed) {     
      set_single_persistent_default_layer(_COLEMAK);
      language=ES;
	  }
	}
	break;
  case M_EN: {
	  if (record->event.pressed) {     
      set_single_persistent_default_layer(_COLEMAK);
      language=EN;
	  }
	}
	break;
  case M_FR: {
	  if (record->event.pressed) {     
      set_single_persistent_default_layer(_COLEMAK);
      language=FR;
	  }
	}
	break;
  }
  set_mods(temp_mod);
  return MACRO_NONE;
};


// Tool used to align the columns: https://onlinetexttools.com/convert-text-to-nice-columns
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Colemak
 * ,-----------------------------------------------------------------------------------.
 * | Esc  |   Q  |   W  |   F  |   P  |   G  |   J  |   L  |   U  |   Y  |   ;  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Tab  |   A  |   R  |   S  |   T  |   D  |   H  |   N  |   E  |   I  |   O  |  ' " |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   K  |   M  |   ,  |   .  |   /  |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | RGB  | GUI  | Alt  |Lower |    Space    |Raise | MENU | Down |  Up  |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_COLEMAK] = LAYOUT_planck_grid(
KC_ESC      ,M(M_Q)         ,M(M_W)         ,KC_F         ,KC_P         ,KC_G    ,KC_J    ,KC_L          ,KC_U         ,KC_Y         ,M(M_SCLN)    ,KC_BSPC    , 
KC_TAB      ,M(M_A)         ,ALT_T(KC_R)    ,SFT_T(KC_S)  ,CTL_T(KC_T)  ,KC_D    ,KC_H    ,CTL_T(KC_N)   ,SFT_T(KC_E)  ,ALT_T(KC_I)  ,GUI_T(KC_O)  ,M(M_QUOT)  , 
KC_LSFT     ,M(M_Z)         ,KC_X           ,TD(TD_C)     ,KC_V         ,KC_B    ,KC_K    ,M(M_M)        ,M(M_COMM)    ,M(M_DOT)     ,TD(TD_SLSH)  ,KC_ENT     , 
KC_LCTL     ,RGB_TOG        ,KC_LGUI        ,KC_LALT      ,LOWER        ,KC_SPC  ,KC_SPC  ,TD(TD_MOUSE)  ,KC_APP       ,KC_DOWN      ,KC_UP        ,KC_RGHT    
),

/* Gaming (Colemak without tap dancing)
 * ,-----------------------------------------------------------------------------------.
 * | Esc  |   Q  |   W  |   F  |   P  |   G  |   J  |   L  |   U  |   Y  |   ;  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Tab  |   A  |   R  |   S  |   T  |   D  |   H  |   N  |   E  |   I  |   O  |  ' " |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   K  |   M  |   ,  |   .  |   /  |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | RGB  | GUI  | Alt  |Lower |    Space    |Raise | MENU | Down |  Up  |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_GAMING] = LAYOUT_planck_grid(
KC_ESC      ,KC_Q           ,KC_W           ,KC_F         ,KC_P         ,KC_G    ,KC_J    ,KC_L          ,KC_U         ,KC_Y         ,KC_SCLN      ,KC_BSPC, 
KC_TAB      ,KC_A           ,KC_R           ,KC_S         ,KC_T         ,KC_D    ,KC_H    ,KC_N          ,KC_E         ,KC_I         ,KC_O         ,KC_QUOT,
KC_LSFT     ,KC_Z           ,KC_X           ,KC_C         ,KC_V         ,KC_B    ,KC_K    ,KC_M          ,KC_COMM      ,KC_DOT       ,KC_SLSH      ,KC_ENT ,
KC_LCTL     ,RGB_TOG        ,KC_LGUI        ,KC_LALT      ,LOWER        ,KC_SPC  ,KC_SPC  ,TD(TD_MOUSE)  ,KC_APP       ,KC_DOWN      ,KC_UP        ,KC_RGHT    
),

/* Lower
 * ,-----------------------------------------------------------------------------------.
 * |   ~  |   !  |   @  |   (  |   )  |   %  |   ^  |   7  |   8  |   9  |   -  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   &  |   #  |   $  |   [  |   ]  |   *  |   ´  |   4  |   5  |   6  |   +  |  =   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |   €  |   {  |   }  |   ¨  |   `  |   1  |   2  |   3  | \ |  |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | RGB  | GUI  | Alt  |Lower |             |Adjust|   0  |   .  | Vol+ | Play |
 * `-----------------------------------------------------------------------------------'
 */
[_LOWER] = LAYOUT_planck_grid(
M(M_TILD)  ,TD(TD_EXLM)    ,M(M_AT)     ,M(M_LPRN)         ,M(M_RPRN)         ,M(M_PERC)         ,M(M_CIRC)  ,M(M_7)   ,M(M_8)       ,M(M_9)       ,M(M_MINS)      ,KC_BSPC           , 
M(M_AMPR)  ,M(M_HASH)      ,M(M_DLR)    ,M(M_LBRC)         ,M(M_RBRC)         ,M(M_ASTR)         ,M(M_ACUT)  ,M(M_4)   ,M(M_5)       ,M(M_6)       ,M(M_PLUS)      ,M(M_EQL)          , 
KC_CAPS    ,_______        ,M(M_EURO)   ,M(M_LCBR)         ,M(M_RCBR)         ,M(M_DIAE)         ,M(M_GRV)   ,M(M_1)   ,M(M_2)       ,M(M_3)       ,M(M_BSLS)      ,_______           , 
_______    ,_______        ,_______     ,_______           ,_______           ,_______           ,_______    ,_______  ,M(M_0)       ,KC_DOT       ,KC_VOLU        ,KC_MPLY                  
),

/* Raise
 * ,-----------------------------------------------------------------------------------.
 * |      |  F1  |  F2  |  F3  |  F4  |      |      | Home |  Up  |  End |Pg Up | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |  F5  |  F6  |  F7  |  F8  |      |      | Left | Down |Right |Pg Dn |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |  F9  |  F10 |  F11 |  F12 |      |      |ISO # |ISO / |Pg Up |Pg Dn |Insert|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | RGB  | GUI  | Alt  |Adjust|             |Raise | Next | Vol- | Vol+ | Play |
 * `-----------------------------------------------------------------------------------'
 */
[_RAISE] = LAYOUT_planck_grid(
_______ ,KC_F1        ,KC_F2        ,KC_F3        ,KC_F4        ,_______ ,_______ ,KC_HOME ,KC_UP   ,KC_END   ,KC_PGUP ,KC_BSPC   , 
_______ ,GUI_T(KC_F5) ,ALT_T(KC_F6) ,SFT_T(KC_F7) ,CTL_T(KC_F8) ,_______ ,_______ ,KC_LEFT ,KC_DOWN ,KC_RIGHT ,KC_PGDN ,_______   , 
_______ ,KC_F9        ,KC_F10       ,KC_F11       ,KC_F12       ,_______ ,_______ ,KC_NUHS ,KC_NUBS ,KC_PGUP  ,KC_PGDN ,KC_INS    , 
_______ ,_______      ,_______      ,_______      ,_______      ,_______ ,_______ ,_______ ,KC_MNXT ,KC_VOLD  ,KC_VOLU ,KC_MPLY   
),


/* Adjust (Lower + Raise)
 *                      v------------------------RGB CONTROL--------------------v
 * ,-----------------------------------------------------------------------------------.
 * |      | Reset|Debug | RGB  |RGBMOD| HUE+ | HUE- | SAT+ | SAT- |BRGTH+|BRGTH-|  Del |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |MUSmod|Aud on|Audoff|      |      |  ES  |  EN  |  FR  |Colema|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |Voice-|Voice+|Mus on|Musoff|MIDIon|MIDIof|TermOn|TermOf|      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_ADJUST] = LAYOUT_planck_grid(
    _______, RESET,   DEBUG,   RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD,  RGB_VAI, RGB_VAD, KC_DEL ,
    _______, _______, MU_MOD,  AU_ON,   AU_OFF,  _______, _______, M(M_ES),  M(M_EN),  M(M_FR), GAMING, _______,
    _______, MUV_DE,  MUV_IN,  MU_ON,   MU_OFF,  MI_ON,   MI_OFF,  TERM_ON, TERM_OFF, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______, _______
),

/* Mouse
 * ,-----------------------------------------------------------------------------------.
 * | set  | Like |      |      |      |      |      |LeftC | Up   |RightC|ScroUp|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Car  |      |      |      |      |      |      | Left | Down | Right|ScroDo|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |Speed1|Speed2|Speed3|      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      |MOUSEL|      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_MOUSE] = LAYOUT_planck_grid(
    M(M_TEXT) , M(M_LSPOT), _______, _______, _______, _______, _______, KC_BTN1, KC_MS_U,  KC_BTN2, KC_WH_U, _______,
    M(M_TEXT1),    _______, _______, _______, _______, _______, _______, KC_MS_L, KC_MS_D,  KC_MS_R, KC_WH_D, _______,
    _______   ,    _______, _______, _______, _______, _______, _______, KC_ACL0, KC_ACL1,  KC_ACL2, _______, _______,
    _______   ,    _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______, _______
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
  // set_single_persistent_default_layer(_COLEMAK);
  switch (keycode) {
    // case QWERTY:
    //   if (record->event.pressed) {
    //     print("mode just switched to qwerty and this is a huge string\n");
    //     // set_single_persistent_default_layer(_QWERTY);
    //   }
    //   return false;
    //   break;
    case COLEMAK:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_COLEMAK);
      }
      return false;
      break;
    // case COLEMAK_ES:
    // if (record->event.pressed) {
    //   set_single_persistent_default_layer(_COLEMAK_ES);
    // }
    // return false;
    // break;
    case GAMING:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_GAMING);
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
    // case PLOVER:
    //   if (record->event.pressed) {
    //     #ifdef AUDIO_ENABLE
    //       stop_all_notes();
    //       PLAY_SONG(plover_song);
    //     #endif
    //     layer_off(_RAISE);
    //     layer_off(_LOWER);
    //     layer_off(_ADJUST);
    //     // layer_on(_PLOVER);
    //     if (!eeconfig_is_enabled()) {
    //         eeconfig_init();
    //     }
    //     keymap_config.raw = eeconfig_read_keymap();
    //     keymap_config.nkro = 1;
    //     eeconfig_update_keymap(keymap_config.raw);
    //   }
    //   return false;
    //   break;
    case EXT_PLV:
      if (record->event.pressed) {
        #ifdef AUDIO_ENABLE
          PLAY_SONG(plover_gb_song);
        #endif
        // layer_off(_PLOVER);
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