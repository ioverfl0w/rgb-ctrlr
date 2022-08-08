/*
  RGB Light controller
  Control a 4-wire RGB device, using a button to change colors and patterns

*/
int selectedColor = 0;
int selectedPattern = 2;

static int buttonPin = A8;
static int buttonHold = 500; // time duration in ms to declare button held
static int defaultDelay = 50;
static int groups[][3] = {
  {3, 6, 5}
};
static int patterns[] {
  500,  //Constant on
  60,   //Pulse
  60,   //Rainbow Fade
};
static int colors[][3] {
  {255, 0,   0},    //red
  {0,   255, 0},    //green
  {0,   0,   255},  //blue
  {255, 255, 255},  //white
  {255, 255, 0},    //yellow
  {255, 0,   255},  //purple
  {0,   255, 255},  //cyan
};
static int groupsLen = (sizeof(groups) / 3) / sizeof(int);
static int patternsLen = sizeof(patterns) / sizeof(int);
static int colorsLen = (sizeof(colors) / 3) / sizeof(int);

void setup() {
  for (int i = 0; i < groupsLen; i++) {
    for (int x = 0; x < 3; x++) {
      pinMode(groups[i][x], OUTPUT);
    }
  }
  Serial.begin(9600);
}

void loop() {
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
    case 2: // Rainbow Fade
      rainbow();
      break;
  }
}

static int pulseIncr = 12;
boolean pulseAscend = true;
int pulseBlend[16][3];
int pulsePosition = 0;
int pulseColor = -1;
void pulse() {
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
      pulseAscend = false;
    } else {
      pulsePosition++;
    }
  } else {
    if (pulsePosition == 1) { // set to 1, so light never goes completely off
      pulseAscend = true;
    } else {
      pulsePosition--;
    }
  }
}

static boolean rainbowRandom = true;
boolean rainbowSwitch = false;
void rainbow() {
  if (!pulseAscend && !rainbowSwitch) {
    rainbowSwitch = true;
  }
  if (pulseAscend && rainbowSwitch) {
    if (rainbowRandom) {
      randomColor();
    } else {
      nextColor();
    }
    rainbowSwitch = false;
  }
  pulse();
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
  for (int i = 0; i < groupsLen; i++) {
    activateColor(i);
  }
}

void activateColor(int color[]) {
  for (int i = 0; i < groupsLen; i++) {
    activateColor(i, color);
  }
}

void activateColor(int grp) {
  activateColor(grp, { colors[selectedColor] });
}

void activateColor(int grp, int color[]) {
  for (int i = 0; i < 3; i++) {
    analogWrite(groups[grp][i], color[i]);
  }
}

void debug() {
  Serial.print("color: ");
  Serial.print(selectedColor);
  Serial.print(" - pattern: ");
  Serial.println(selectedPattern);
}
