/*
  RGB Light controller - Arduino Nano
  2 channel RGB led light control, using a button to change patterns and colors
  and a dip switch to enable channel outputs.
  Control a 4-wire RGB device, using a button to change colors and patterns

*/
int selectedColor = 0;
int selectedPattern = 1;
bool channelStatus[2];

static int buttonPin = A0;
static int dipSwitch[2] = {A1, A2}; // channel control
static int buttonHold = 500; // time duration in ms to declare button held
static int defaultDelay = 50;
static int channel[][3] = {
  {3, 6, 5},
  {9, 10, 11},
};
static int patterns[] {
  100,  //Constant on
  60,   //Pulse
  40,   //Pulse (drop)
  60,   //Rainbow Pulse
  60,   //Rainbow Pulse (random colors)
  40,   //Rainbow Pulse (drop)
  95,   //Crazy Rainbow
};
static int OFF[3] {0, 0, 0};
static int colors[][3] {
  {255, 0,   0},    //red
  {0,   255, 0},    //green
  {0,   0,   255},  //blue
  {255, 255, 255},  //white
  {255, 255, 0},    //yellow
  {255, 0,   255},  //purple
  {0,   255, 255},  //cyan
};
static int channelLen = (sizeof(channel) / 3) / sizeof(int);
static int patternsLen = sizeof(patterns) / sizeof(int);
static int colorsLen = (sizeof(colors) / 3) / sizeof(int);

void setup() {
  for (int i = 0; i < channelLen; i++) {
    for (int x = 0; x < 3; x++) {
      pinMode(channel[i][x], OUTPUT);
    }
  }
  Serial.begin(9600);
}

void loop() {
  checkChannels();
  performPattern();
  buttonWork();
  if (debug) {
    debug();
  }
  delay(patterns[selectedPattern]);
}

void performPattern() {
  switch (selectedPattern) {
    case 0: // Constant on
      activateColor();
      break;
    case 1: // Pulse
      pulse();
      break;
    case 2: // Pulse (drop)
      pulse(1);
      break;
    case 3: // Rainbow Pulse
      rainbow();
      break;
    case 4: // Rainbow Pulse (random color order)
      rainbow(3);
      break;
    case 5: // Rainbow Pulse (drop)
      rainbow(1);
      break;
    case 6: // Crazy Rainbow
      rainbow(2);
      break;
  }
}

static int pulseIncr = 16;
boolean pulseAscend = true;
int pulseBlend[16][3];
int pulsePosition = 0;
int pulseColor = -1;

/*
 * modes
 * 
 * 0 = normal
 * 1 = drop only
 * 2 = rise only
 * 
 */

void pulse(int mode) {
  //setup
  if (pulseColor != selectedColor) {
    pulseColor = selectedColor;
    for (int i = 0; i < pulseIncr; i++) {
      for (int x = 0; x < 3; x++) {
        pulseBlend[i][x] = (colors[selectedColor][x] / pulseIncr) * i;
      }
    }
  }

  activateColor(pulseBlend[pulsePosition]);
  if (pulseAscend) {
    if ((pulsePosition + 1) == pulseIncr) {
      if (mode == 2) { // rise only
        pulsePosition = 1;
      } else {
        pulseAscend = false;
      }
    } else {
      pulsePosition++;
    }
  } else {
    if (pulsePosition == 1) { // set to 1, so light never goes completely off
      if (mode == 1) { // drop only
        pulsePosition = pulseIncr - 1;
      } else {
        pulseAscend = true;
      }
    } else {
      pulsePosition--;
    }
  }
}

void pulse() {
  pulse(0);
}

boolean rainbowRandom = false;
boolean rainbowSwitch = false;
/*
 * modes
 * 0 = normal
 * 1 = drop only
 * 2 = crazy mode
 */
void rainbow(int mode) {
  rainbowRandom = mode > 0;
  
  if (!pulseAscend && !rainbowSwitch) {
    rainbowSwitch = true;
  }
  
  if (pulseAscend && (mode == 2 || rainbowSwitch)) {
    rainbowSwitch = false;
    switchColor();
  }
  
  pulse(mode);

  if (mode == 1 && pulsePosition == 1) {
    switchColor();
  }
}

void rainbow() {
  rainbow(0);
}

void switchColor() {
  if (rainbowRandom) {
    randomColor();
  } else {
    nextColor();
  }
}

void buttonWork() {
  int btnStatus = digitalRead(buttonPin);

  if (btnStatus == HIGH) {
    if (debug) {
      Serial.println("button triggered .. ");
    }
    delay(buttonHold - defaultDelay);

    // button was held
    if (btnStatus == digitalRead(buttonPin)) {
      Serial.println("button was held .. change color"); // DEBUG
      //change color
      nextColor();
      activateColor();
      delay(500); // time to let go of button

      //button was not held for the set duration
    } else {
      Serial.println("button was NOT held .. change pattern"); // DEBUG
      //change pattern
      selectedPattern = ((selectedPattern + 1) == patternsLen) ? 0 : selectedPattern + 1;
    }

    debug();
  }
}

void checkChannels() {
  for (int i = 0; i < channelLen; i++) {
    channelStatus[i] = updateChannel(i);
  }
}

void randomColor() {
  int p = selectedColor;
  selectedColor = random(colorsLen - 1);
  if (selectedColor == p) { // prevent two in a rows
    return randomColor();
  }
}

void nextColor() {
  selectedColor = ((selectedColor + 1) == colorsLen) ? 0 : selectedColor + 1;
}

void prevColor() {
  selectedColor = ((selectedColor - 1) == -1) ? colorsLen - 1 : selectedColor - 1;
}

void activateColor() {
  for (int i = 0; i < channelLen; i++) {
    if (channelStatus[i]) {
      activateColor(i);
    } else {
      activateColor(i, OFF);
    }
  }
}

void activateColor(int color[]) {
  for (int i = 0; i < channelLen; i++) {
    if (channelStatus[i]) {
      activateColor(i, color);
    } else {
      activateColor(i, OFF);
    }
  }
}

void activateColor(int grp) {
  activateColor(grp, { colors[selectedColor] });
}

void activateColor(int grp, int color[]) {
  for (int i = 0; i < 3; i++) {
    analogWrite(channel[grp][i], color[i]);
  }
}

bool updateChannel(int ch) {
  return digitalRead(dipSwitch[ch]) == HIGH;
}

void debug() {
  Serial.print("color: ");
  Serial.print(selectedColor);
  Serial.print(" - pattern: ");
  Serial.println(selectedPattern);
}
