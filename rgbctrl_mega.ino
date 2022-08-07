/*
  RGB Light controller
  Control a 4-wire RGB device, using a button to change colors and patterns

*/
int selectedColor = 7;
int selectedPattern = 0;

static int buttonPin = A8;
static int buttonHold = 500; // time duration in ms to declare button held
static int defaultDelay = 50;
static int groups[][3] = {
  {3, 6, 5}
};
static int patterns[][1] {
  800 //Constant on
};
static int colors[][3] {
  {255, 0,   0}, //red
  {0,   255, 0}, //green
  {0,   0,   255}, //blue
  {255, 255, 255}, //white
  {255, 255, 0}, //yellow
  {255, 0,   255}, //purple
  {0,   255, 255}, //cyan
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
  debug();
  delay(patterns[selectedPattern]);
}

void performPattern() {
  switch (selectedPattern) {
    case 0: // Constant on
      activateColor();
      break;
  }
}

void buttonWork() {
  int btnStatus = digitalRead(buttonPin);

  if (btnStatus == HIGH) {
    Serial.println("button triggered .. ");
    delay(buttonHold - defaultDelay);

    // button was held
    if (btnStatus == digitalRead(buttonPin)) {
      Serial.println("button was held .. change color");
      //change color
      selectedColor = ((selectedColor + 1) == colorsLen) ? 0 : selectedColor + 1;
      activateColor();
      delay(500); // time to let go of button

      //button was not held for the set duration
    } else {
      Serial.println("button was NOT held .. change pattern");
      //change pattern
      selectedPattern = ((selectedPattern + 1) == patternsLen) ? 0 : selectedPattern + 1;
    }

    debug();
  }
}

void activateColor() {
  for (int i = 0; i < groupsLen; i++) {
    activateColor(i);
  }
}

void activateColor(int grp) {
  activateColor(grp, { colors[selectedColor] });
}

void activateColor(int grp, int color[]) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(groups[grp][i], color[i]);
    Serial.print(" - pin: ");
    Serial.print(groups[grp][i]);
    Serial.print(" - color: ");
    Serial.println(color[i]);
    Serial.println();
  }
  Serial.println("-----");
}

void debug() {
  Serial.print("color: ");
  Serial.print(selectedColor);
  Serial.print(" - pattern: ");
  Serial.println(selectedPattern);
}
