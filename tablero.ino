#include "BluetoothSerial.h"

String device_name = "ESP32-BT-Slave";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int gameTeamA = 0;
int gameTeamB = 0;
int pointsTeamA = 0;
int pointsTeamB = 0;
int setTeamA=0;
int setTeamB=0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name); // Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
}

void updateSet(char team){
    if (team == 'A') {
      setTeamA++;
    }
    else if (team == 'B') {
      setTeamB++;
  }

  Serial.printf("Sets: P1(%d) - P2(%d)\n", setTeamA, setTeamB);

}
  

void updateGame(char team) {
  if (team == 'A') {
    gameTeamA++;
    if(gameTeamA == 6){
        updateSet(team);
        resetPoints();
        resetScore();
      }
  } else if (team == 'B') {
    gameTeamB++;
    if(gameTeamB == 6){
        updateSet(team);
        resetPoints();
        resetScore();
      }
  }

  Serial.printf("Marcador: P1(%d) - P2(%d)\n", gameTeamA, gameTeamB);
}

void updatePoints(char team) {
  if (pointsTeamA == 40 && pointsTeamB == 40) {
    // Punto de oro, el próximo que anota gana
    if (team == 'A') {
      updateGame('A');
    } else if (team == 'B') {
      updateGame('B');
    }

    resetPoints();
  } else {
    // Manejar los puntos normales
    if (team == 'A') {
      if (pointsTeamA == 0) {
        pointsTeamA = 15;
      } else if (pointsTeamA == 15) {
        pointsTeamA = 30;
      } else if (pointsTeamA == 30) {
        pointsTeamA = 40;
      } else if (pointsTeamA == 40 && pointsTeamB < 40) {
        // Controlar el estado de "ventaja"
        updateGame('A');
        resetPoints();
      }
    } else if (team == 'B') {
      if (pointsTeamB == 0) {
        pointsTeamB = 15;
      } else if (pointsTeamB == 15) {
        pointsTeamB = 30;
      } else if (pointsTeamB == 30) {
        pointsTeamB = 40;
      } else if (pointsTeamB == 40 && pointsTeamA < 40) {
        // Controlar el estado de "ventaja"
        updateGame('B');
        resetPoints();
      }
    }
  }

  Serial.printf("Puntos: P1(%d) - P2(%d)\n", pointsTeamA, pointsTeamB);
}

void resetPoints() {
  pointsTeamA = 0;
  pointsTeamB = 0;
}

void resetScore() {
  gameTeamA = 0;
  gameTeamB = 0;
}

void loop() {
  if (SerialBT.available()) {
    char dato = SerialBT.read();

    switch (dato) {
      case 'R':
        Serial.println("Resetear marcador");
        resetScore();
        resetPoints();
        break;
      case 'A':
        Serial.println("Punto de pareja 1");
        updatePoints('A');
        break;
      case 'B':
        Serial.println("Punto de pareja 2");
        updatePoints('B');
        break;
      // Agrega más casos según tus necesidades
    }

    // Asegúrate de esperar un poco después de procesar cada carácter
    delay(20);
  }
}
