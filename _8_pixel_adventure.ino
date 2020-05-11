const int button_pin = 13; // D13 for input
const int led_pin[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // D2-D9 for output
// button must be released to register next push
bool button_released = false;

// output a byte to 8 LEDs
void draw_byte(uint8_t b) {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(led_pin[i], b & 1 ? HIGH : LOW);
    b = b >> 1;
  }
}

// check button state. Return true if registers a NEW push
// need to call it fairly often to register button release first
bool check_button() {
  int button_state = digitalRead(button_pin);
  if (button_state == HIGH && button_released) {
    button_released = false;
    return true;
  } else if (button_state == LOW && !button_released) {
    button_released = true;
  }
  return false;
}

void level_begin_animation() {
  for (int i = 7; i >= 1; i--) {
    digitalWrite(led_pin[i], LOW);
    delay(100);
  }
}

void level_end_animation() {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(led_pin[i], HIGH);
    delay(100);
  }
}

void victory_animation() {
  delay(1000);
  // blink with increasing frequency
  for (int i = 1; i <= 16; i++) {
    for (int j = 0; j < i*2; j++) {
      draw_byte(0);
      delay(1024 >> i);
      draw_byte(0b11111111);
      delay(1024 >> i);
    }
  }
  // all bright, two dark pixels ride right to left
  draw_byte(0b11111111);
  delay(1000);
  for (int i = 0; i < 7; i++) {
    draw_byte(0b11111111 ^ (1 << i) ^ (1 << i+1));
    delay(500);
  }
  // fade bright to dark
  draw_byte(0b11111111);
  delay(1000);
  for (int i = 0; i < 300; i++) {
    draw_byte(0);
    delay(i/30);
    draw_byte(0b11111111);
    delay(10 - i/30);    
  }
  draw_byte(0);
  delay(1000);
}

void defeat_loop(int position) {
  int disp = 0b11111111 ^ (1 << position);
  int blink_freq = 50;
  int counter = 0;
  // blink all led except position
  while (true) {
    if (counter++ % blink_freq == 0) {
      draw_byte(disp);
      disp = disp ^ 0b11111111;
    }
    if (check_button()) {
      return;
    }
    delay(10);
  }

}

// show final score in binary
void final_score_loop(int score) {
  int disp = 0;
  int blink_freq = 5;
  int counter = 0;
  while (true) {
    if (counter++ % blink_freq == 0) {
      disp = disp ^ score;
      draw_byte(disp);
    }
    if (check_button()) {
      return;
    }
    delay(10);
  }
}

void fireworks_loop() {
  int counter = 0;
  int position = random(1, 7);
  const int magic = 64;
  while (true) {
    if (counter > 16) {
      draw_byte((1 << position) 
                | (1 << (position - 1))
                | (1 << (position + 1)));
      delay(magic - counter);
      draw_byte(0);
      delay(counter);
    } else {
      
      draw_byte(1 << position);
      delay(counter);
      draw_byte(0);
      delay(16-counter);
    }
    
    counter++;
    if (counter == magic) {
      counter = 0;
      position = random(1, 7);
      delay(1000);
    }
    if (check_button()) {
      return;
    }
  }
}

bool is_trap_active(unsigned int state) {
  // duty cycle 1/4
  return state < 4096;
}

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(led_pin[i], OUTPUT);
  }
  pinMode(button_pin, INPUT);
  pinMode(0, INPUT);
  randomSeed(analogRead(0));
}

struct Trap {
  uint16_t freq; // activation frequency
  uint16_t state; // open or closed will be calculated from this value
  bool planted; // if there's a trap
};

void loop(){
  // game loop
  draw_byte(0b11111111);
  delay(200);
  for (int level = 0; level <= 255; level++) {
    level_begin_animation();
    // init level
    int position = 0;
    // create traps
    struct Trap trap[8] = {{0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0},
                           {0, 255, 0}, {0, 255, 0}, {0, 255, 0}, {0, 255, 0}};
    // traps can only be in 6 middle cells
    // there can only be 1 to 3 traps (last level 2-4)
    // there's a chance that a trap will be overwritten with another, lucky you
    for (int i = random(1, 3) + (level/255); i >=0 ; i--) {
      int position = random(1, 7);
      // difficulty adjustment
      trap[position].freq = random(64, 256) + level;
      trap[position].state = random(16384);
      trap[position].planted = true;
    }
    // level loop
    while (true) {
      bool button_active = check_button();
      if (button_active) {
        position = position + 1;
      }
      // calculate world
      if (position == 8) {
        break;
      } else if (trap[position].planted) {
        if (!is_trap_active(trap[position].state)) {
          position += 1;
          continue;
        } else {
          defeat_loop(position);
          final_score_loop(level);
          return;
        }
      }
      //update traps state
      for (int i = 0; i < 8; i++) {
        trap[i].state = (trap[i].state + trap[i].freq) % 16384;
      }
      // world state to draw. Start with a player
      int world = 1 << position;
      // draw traps
      for (int i = 0; i < 8; i++) {
        if (trap[i].planted) {
          world = world | (is_trap_active(trap[i].state) ? 1 << i : 0);
        }
      }
      draw_byte(world);
      delay(10);
    }
    level_end_animation();
  }
  victory_animation();
  fireworks_loop();
}

