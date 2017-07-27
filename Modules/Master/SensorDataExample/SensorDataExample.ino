
  char  activeSensors;
  // Petal LED counts
    unsigned int petalLevel1 = [150, 152, 149, 153, 143, 154]; // this will change as we calabrate eacah petal, one value for each of the 6 petals per level
    unsigned int petalLevel2 = [150, 152, 149, 153, 143, 154];
    unsigned int petalLevel3 = [150, 152, 149, 153, 143, 154];

  // Arm LED counts
    unsigned int armLEDCounts = [150, 152, 149, 153, 143, 154];

  // Sensors
    // Palse
    bool palseActive;
    unsigned int rate;

    // Chair Palse
    bool chairPalseActive;
    unsigned int highestRate;
    unsigned int currentRate;

    // Alcohol
    bool alcoholActive;
    unsigned int alcoholLevel; // Between lowLevel and High Level
    unsigned int alcoholLowLevel = 100;
    unsigned int alcoholHighLevel = 600;

    // Gear Rotation
    bool gearRotationActive;
    unsigned int currentTurn;
    unsigned int maxTurns = 10;

    // Gesture
    bool gestureActive;
    char gestureDirection; // Possible values:up, down, left, right, in, out.
    
    // Buttons
    bool buttonsActive;
    bool redButton;
    bool blueButton;
    bool greenButton;
    bool yellowButton;

    // Audio
    bool audioActive;
    unsigned int micValve; // Between 0 and 1023
    int frequencyValues = [345,1023,138,0,98,101,478]; // 7 Channels each between 0 and 1023

void setup() {
 
}

void loop() {
	if (availableSensorData(&activeSensors)) {
		// do something

		// Each sesnsor has a active variable that shoud be double checked before using values just in case the availableSensorData function was wrong.
		
	}
}

bool availableSensorData(char *fActiveSensors) {
	// function will look to see if there is available sensor data. If there is, it will 
	// list the active sensors in the activeSensors array and return true. Otherwise it'll return false.
  return false;
}
