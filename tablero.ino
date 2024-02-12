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
bool inTieBreak = false;
int tieBreakPointsTeamA = 0;
int tieBreakPointsTeamB = 0;

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

}
  

void updateGame(char team) {
  if (team == 'A') {
    gameTeamA++;
    if (gameTeamA >= 6 && (gameTeamA - gameTeamB >= 2 || (gameTeamA == 6 && gameTeamB == 6))) {
      if (gameTeamA == 6 && gameTeamB == 6) {
        // Iniciar tie-break
        inTieBreak = true;
      } else {
        updateSet(team);
        resetPoints();
        resetScore();
      }
    }
  } else if (team == 'B') {
    gameTeamB++;
    if (gameTeamB >= 6 && (gameTeamB - gameTeamA >= 2 || (gameTeamB == 6 && gameTeamA == 6))) {
      if (gameTeamB == 6 && gameTeamA == 6) {
        // Iniciar tie-break
        inTieBreak = true;
      } else {
        updateSet(team);
        resetPoints();
        resetScore();
      }
    }
  }
}

void updatePoints(char team) {
  if (inTieBreak) {
    if (team == 'A') {
      tieBreakPointsTeamA++;
    } else if (team == 'B') {
      tieBreakPointsTeamB++;
    }

    // Verificar condiciones de victoria en el tie-break
    if ((tieBreakPointsTeamA >= 7 && tieBreakPointsTeamA - tieBreakPointsTeamB >= 2) ||
        (tieBreakPointsTeamB >= 7 && tieBreakPointsTeamB - tieBreakPointsTeamA >= 2)) {
      updateSet(team);
      resetPoints();
      resetScore();
      inTieBreak = false;
    }
  } else{
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
}
}

void resetPoints() {
  pointsTeamA = 0;
  pointsTeamB = 0;
  if (inTieBreak) {
    tieBreakPointsTeamA = 0;
    tieBreakPointsTeamB = 0;
  }
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
        actualizarMarcador();
        break;
      case 'A':
        Serial.println("Punto de pareja 1");
        updatePoints('A');
        actualizarMarcador();
        break;
      case 'B':
        Serial.println("Punto de pareja 2");
        updatePoints('B');
        actualizarMarcador();
        break;
      // Agrega más casos según tus necesidades
    }

    // Asegúrate de esperar un poco después de procesar cada carácter
    delay(20);
  }
}

void actualizarMarcador(void){
        Serial.println("--------------");
        Serial.printf("Sets: P1(%d) - P2(%d)\n", setTeamA, setTeamB);
        Serial.println("--------------");
        Serial.printf("Games: P1(%d) - P2(%d)\n", gameTeamA, gameTeamB);
        
        

    if (inTieBreak) {
    Serial.println("--------------");
    Serial.printf("Tie-Break: P1(%d) - P2(%d)\n", tieBreakPointsTeamA, tieBreakPointsTeamB);
  } else {
    Serial.println("--------------");
    Serial.printf("Puntos: P1(%d) - P2(%d)\n", pointsTeamA, pointsTeamB);
  }
  Serial.println("");
  
  }
