#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

// Set LCD address to 0x27 for 16x2 display (Try 0x3F if 0x27 doesn't display text)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Custom Characters for smooth pixel-by-pixel RAM progress bar
byte bar1[8] = {B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000};
byte bar2[8] = {B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000};
byte bar3[8] = {B11100, B11100, B11100, B11100, B11100, B11100, B11100, B11100};
byte bar4[8] = {B11110, B11110, B11110, B11110, B11110, B11110, B11110, B11110};
byte bar5[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};

// Custom Characters for Network Arrows
byte upArrow[8]   = {B00100, B01110, B11111, B00100, B00100, B00100, B00100, B00100};
byte downArrow[8] = {B00100, B00100, B00100, B00100, B00100, B11111, B01110, B00100};

// Hardware Data Variables
int gpuT = 0; int gpuU = 0;
int cpuT = 0; int cpuU = 0;
int ramU = 0;
float netU = 0.0; float netD = 0.0;
int ssdR = 0; int ssdW = 0;
String osVer = "Windows 11";

// System State
int displayMode = 0; // 0: Auto Rotate, 1: GPU, 2: CPU, 3: RAM, 4: NET, 5: SSD, 6: OS
int currentPage = 1;
unsigned long lastPageChange = 0;
unsigned long lastDataUpdate = 0;

char currentScreen[2][17] = {"                ", "                "};
char nextScreen[2][17]    = {"                ", "                "};

void setup() {
  Serial.begin(115200);
  
  lcd.init();
  lcd.backlight();
  
  // Register custom pixel characters (Started from 1 to avoid String null-terminator bugs)
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4);
  lcd.createChar(5, bar5);
  lcd.createChar(6, upArrow);
  lcd.createChar(7, downArrow);

  lcd.setCursor(0, 0);
  lcd.print("System Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Waiting PC Py..");
  delay(2000);
}

void loop() {
  readSerialData();

  unsigned long currentMillis = millis();

  // Mode 0: Auto Rotate Mode (4 seconds per page)
  if (displayMode == 0) {
    if (currentMillis - lastPageChange >= 4000) {
      currentPage++;
      if (currentPage > 6) currentPage = 1;
      
      prepareScreenBuffer(currentPage);
      applyTransition();
      lastPageChange = currentMillis;
    } else if (currentMillis - lastDataUpdate >= 1000) {
      prepareScreenBuffer(currentPage);
      refreshScreenQuietly();
      lastDataUpdate = currentMillis;
    }
  } 
  // Modes 1 to 6: Individual Locked Page Mode
  else {
    if (currentPage != displayMode) {
      currentPage = displayMode;
      prepareScreenBuffer(currentPage);
      applyTransition();
    } else if (currentMillis - lastDataUpdate >= 1000) {
      prepareScreenBuffer(currentPage);
      refreshScreenQuietly();
      lastDataUpdate = currentMillis;
    }
  }
}

void readSerialData() {
  if (Serial.available() > 0) {
    String jsonStr = Serial.readStringUntil('\n');
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (!error) {
      int newMode = doc["m"];
      
      // If display mode requested from PC changed
      if (newMode != displayMode) {
        displayMode = newMode;
        if (displayMode == 0) currentPage = 1;
        else currentPage = displayMode;
        
        prepareScreenBuffer(currentPage);
        applyTransition();
        lastPageChange = millis();
      }

      gpuT = doc["gT"]; gpuU = doc["gU"];
      cpuT = doc["cT"]; cpuU = doc["cU"];
      ramU = doc["rU"];
      netU = doc["nU"]; netD = doc["nD"];
      ssdR = doc["sR"]; ssdW = doc["sW"];
    }
  }
}

// Generates the RAM bar safely inside a String
String getRamBarString(int usage) {
  String bar = "[";
  int totalPixels = 9 * 5; 
  int filledPixels = map(usage, 0, 100, 0, totalPixels);
  int fullBlocks = filledPixels / 5;
  int partialBlock = filledPixels % 5;

  for (int i = 0; i < 9; i++) {
    if (i < fullBlocks) {
      bar += (char)5; // Full block custom char
    } else if (i == fullBlocks && partialBlock > 0) {
      bar += (char)partialBlock; // Partial block custom char (1..4)
    } else {
      bar += " "; // Empty space
    }
  }
  bar += "]";
  
  // Format the percentage so it stays cleanly at the right edge
  String usgStr = String(usage) + "%";
  while (bar.length() + usgStr.length() < 16) {
    bar += " ";
  }
  bar += usgStr;
  
  return bar;
}

// Prepare screen contents
void prepareScreenBuffer(int page) {
  String row1 = ""; String row2 = "";
  
  if (page == 1) {
    row1 = "Geforce RTX 3060";
    row2 = "Temp:" + String(gpuT) + "C Usg:" + String(gpuU) + "%";
  } 
  else if (page == 2) {
    row1 = "Ryzen 7 5700x";
    row2 = "Temp:" + String(cpuT) + "C Usg:" + String(cpuU) + "%";
  } 
  else if (page == 3) {
    row1 = "Kingston HyperX";
    row2 = getRamBarString(ramU); // Uses the new pre-generated string
  } 
  else if (page == 4) {
    row1 = "Gigabyte LAN";
    // \x06 is Up Arrow, \x07 is Down Arrow
    row2 = "\x06" + String(netU, 1) + "M \x07" + String(netD, 1) + "M";
  } 
  else if (page == 5) {
    row1 = "Samsung 980 SSD";
    row2 = "R:" + String(ssdR) + "MB W:" + String(ssdW) + "MB";
  } 
  else if (page == 6) {
    row1 = "OS: " + osVer;
    row2 = "Made By Ridwan:)";
  }

  // Pad strings to 16 characters for alignment
  while(row1.length() < 16) row1 += " ";
  while(row2.length() < 16) row2 += " ";

  row1.toCharArray(nextScreen[0], 17);
  row2.toCharArray(nextScreen[1], 17);
}

// --- SMOOTH & UNCOMMON TRANSITIONS ---
void applyTransition() {
  if (currentPage == 6) {
    animateCreditPage();
    return;
  }

  int effect = random(0, 4); 

  switch (effect) {
    case 0: // Wipe Right
      for (int i = 0; i < 16; i++) {
        lcd.setCursor(i, 0); lcd.print(nextScreen[0][i]);
        lcd.setCursor(i, 1); lcd.print(nextScreen[1][i]);
        delay(25);
      }
      break;
    case 1: // Wipe Left
      for (int i = 15; i >= 0; i--) {
        lcd.setCursor(i, 0); lcd.print(nextScreen[0][i]);
        lcd.setCursor(i, 1); lcd.print(nextScreen[1][i]);
        delay(25);
      }
      break;
    case 2: // Vertical Slide
      lcd.setCursor(0, 1); lcd.print(currentScreen[0]);
      delay(120);
      lcd.setCursor(0, 0); lcd.print(nextScreen[0]);
      delay(120);
      lcd.setCursor(0, 1); lcd.print(nextScreen[1]);
      break;
    case 3: // Expand from Center Outward
      for (int i = 0; i <= 7; i++) {
        lcd.setCursor(7 - i, 0); lcd.print(nextScreen[0][7 - i]);
        lcd.setCursor(8 + i, 0); lcd.print(nextScreen[0][8 + i]);
        lcd.setCursor(7 - i, 1); lcd.print(nextScreen[1][7 - i]);
        lcd.setCursor(8 + i, 1); lcd.print(nextScreen[1][8 + i]);
        delay(35);
      }
      break;
  }
  refreshScreenQuietly();
}

// Random transition effect specifically for "Made By Ridwan :)"
void animateCreditPage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(nextScreen[0]); 

  int creditEffect = random(0, 3);
  String creditStr = "Made By Ridwan:)";
  
  if (creditEffect == 0) {
    // Typewriter
    lcd.setCursor(0, 1);
    for (unsigned int i = 0; i < creditStr.length(); i++) {
      lcd.print(creditStr[i]);
      delay(50);
    }
  } else if (creditEffect == 1) {
    // Slide in from right
    for (int i = 16; i >= 0; i--) {
      lcd.setCursor(i, 1);
      lcd.print(creditStr.substring(0, 16 - i));
      delay(30);
      lcd.print(" ");
    }
  } else {
    // Flash Reveal
    for (int i = 0; i < 2; i++) {
      lcd.setCursor(0, 1); lcd.print("                ");
      delay(100);
      lcd.setCursor(0, 1); lcd.print(creditStr);
      delay(150);
    }
  }
  memcpy(currentScreen, nextScreen, sizeof(currentScreen));
}

// Now much simpler since the RAM bar is already a formatted string!
void refreshScreenQuietly() {
  lcd.setCursor(0, 0); lcd.print(nextScreen[0]);
  lcd.setCursor(0, 1); lcd.print(nextScreen[1]);
  memcpy(currentScreen, nextScreen, sizeof(currentScreen));
}