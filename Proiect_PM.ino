// Universitatea "VASILE ALECSANDRI" din Bacau
// Facultatea de Inginerie
// Programarea cu microprocesoare
// Proiect - Stand rotativ pentru scanare 3D
// Creat de Grasu Robert-Nicolae

#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  
SoftwareSerial btSerial(3, 2);       

const int PIN_BUTON_JOYSTICK = 8;  // Pin digital pentru butonul joystick-ului
const int PIN_X = A0;              // Pin analog pentru axa X a joystick-ului
const int PIN_Y = A1;              // Pin analog pentru axa Y a joystick-ului

// Variabile de control pentru meniuri
int numar_meniu = 0;              // Numarul meniului curent (0-2)
int numar_fotografii = 2;         // Numarul de fotografii care trebuie realizate (implicit 2)

// Flaguri pentru evitarea repetitiilor necontrolate
bool steag_meniu = false;         // Steag pentru ajustarea meniului principal
bool steag_fotografii = false;    // Steag pentru ajustarea numarului de fotografii
bool steag_turatie1 = false;      // Steag pentru ajustarea turatiei in modul cinematic
bool steag_rotatii = false;       // Steag pentru ajustarea numarului de rotatii
bool steag_turatie2 = false;      // Steag pentru ajustarea turatiei in modul manual
bool steag_ecran = false;         // Steag pentru stergerea ecranului LCD
int submeniu = 0;                 // Indicator pentru submeniuri
bool steag_buton = false;         // Indicator pentru starea butonului
int contor = 1;                   // Contor pentru comanda Bluetooth

// Configurare motor pas cu pas
const int PASI_PER_REVOLUTIE = 2048;  
int rotatie_completa = 14336;         // Numarul de pasi pentru o rotatie completa a platformei
int turatie = 15;                     // Turatia motorului (0-17 rpm max)

// Variabile pentru controlul scanarii
int fotografii_realizate = 0;         // Numarul de fotografii realizate in sesiunea curenta
int pasi_per_fotografie;              // Numarul de pasi intre fotografii (calculat)
int numar_ture = 1;                   // Numarul de ture setat (implicit 1)
int tura_curenta = 0;                 // Tura curenta in executie
int pasi = 0;                         // Numarul de pasi pentru deplasare manuala

// Definire pini pentru motorul pas cu pas (IN1, IN3, IN2, IN4)
Stepper motor_pas_cu_pas(PASI_PER_REVOLUTIE, 9, 11, 10, 12);  

// Limitele pentru parametri
const int MIN_FOTOGRAFII = 2;         // Numarul minim de fotografii
const int MAX_FOTOGRAFII = 200;       // Numarul maxim de fotografii
const int MIN_TURATIE = 1;            // Turatia minima
const int MAX_TURATIE = 17;           // Turatia maxima
const int MIN_TURE = 1;               // Numarul minim de rotatii
const int MAX_TURE = 200;             // Numarul maxim de rotatii

// Valori de mijloc si margine pentru joystick
const int JOYSTICK_MIN = 400;         // Limita inferioara pentru pozitia neutra
const int JOYSTICK_MAX = 600;         // Limita superioara pentru pozitia neutra

void setup() {
  lcd.init();
  lcd.backlight();
  
  // Configurare pin pentru butonul joystick-ului (pullup intern activat)
  pinMode(PIN_BUTON_JOYSTICK, INPUT);
  digitalWrite(PIN_BUTON_JOYSTICK, HIGH);
  
  // Initializare comunicare Bluetooth
  btSerial.begin(9600);

  // Setare viteza motor pas cu pas
  motor_pas_cu_pas.setSpeed(turatie);
}

void loop() {
  // Citire valori joystick
  int valoare_X = analogRead(PIN_X);                // Valoarea analogica de pe axa X (0-1023)
  int valoare_Y = analogRead(PIN_Y);                // Valoarea analogica de pe axa Y (0-1023)
  int valoare_buton = digitalRead(PIN_BUTON_JOYSTICK);  // Valoarea digitala a butonului (0 = apasat, 1 = neapasat)

  // Limitare numar meniu intre 0 si 2
  if (numar_meniu < 0) {
    numar_meniu = 0;
  } else if (numar_meniu > 2) {
    numar_meniu = 2;
  }

  // Navigare intre meniurile principale (stanga/dreapta) cand suntem la nivelul principal
  if (valoare_X < JOYSTICK_MIN && steag_meniu == false && submeniu == 0) {
    numar_meniu = numar_meniu + 1;
    steag_meniu = true;
    lcd.clear();
  }

  if (valoare_X > JOYSTICK_MAX && steag_meniu == false && submeniu == 0) {
    numar_meniu = numar_meniu - 1;
    steag_meniu = true;
    lcd.clear();
  }

  // Resetare steag cand joystick-ul revine in pozitia neutra
  if (valoare_X >= JOYSTICK_MIN && valoare_X <= JOYSTICK_MAX && steag_meniu == true) {
    steag_meniu = false;
  }

  // Procesare apasare buton joystick (avansare in submeniu)
  if (valoare_buton == 0 && steag_buton == false) {
    submeniu = submeniu + 1;
    steag_buton = true;
    lcd.clear();
  }

  // Resetare steag buton cand butonul este eliberat
  if (valoare_buton == 1 && steag_buton == true) {
    steag_buton = false;
  }

  // =========================
  // MENIUL 0 - MOD FOTOGRAFIE
  // =========================
  if (numar_meniu == 0) {
    // Afisare titlu meniu
    if (submeniu == 0) {
      lcd.setCursor(0, 0);
      lcd.print(" Mod fotografie");
    }

    // Submeniu 1: Setare numar fotografii
    else if (submeniu == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Nr. fotografii");
      lcd.setCursor(7, 1);
      lcd.print(numar_fotografii);

      // Crestere numar fotografii (joystick sus)
      if (valoare_Y < JOYSTICK_MIN && steag_fotografii == false) {
        numar_fotografii = numar_fotografii + 2;
        steag_fotografii = true;
        lcd.clear();
      }
      
      // Scadere numar fotografii (joystick jos)
      if (valoare_Y > JOYSTICK_MAX && steag_fotografii == false) {
        numar_fotografii = numar_fotografii - 2;
        steag_fotografii = true;
        lcd.clear();
      }
      
      // Resetare steag cand joystick-ul revine in pozitia neutra
      if (valoare_Y >= JOYSTICK_MIN && valoare_Y <= JOYSTICK_MAX && steag_fotografii == true) {
        steag_fotografii = false;
      }

      // Limitare numar fotografii in intervalul permis
      if (numar_fotografii < MIN_FOTOGRAFII) {
        numar_fotografii = MIN_FOTOGRAFII;
      }
      if (numar_fotografii > MAX_FOTOGRAFII) {
        numar_fotografii = MAX_FOTOGRAFII;
      }
    }

    // Submeniu 2: Executie program fotografiere
    else if (submeniu == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Program in executie");
      lcd.setCursor(0, 1);
      lcd.print("Foto: ");
      lcd.print(fotografii_realizate);
      lcd.print("/");
      lcd.print(numar_fotografii);

      // Calculeaza numarul de pasi intre fotografii
      pasi_per_fotografie = rotatie_completa / numar_fotografii;

      // Daca mai sunt fotografii de realizat
      if (fotografii_realizate < numar_fotografii) {
        // Setare viteza si deplasare motor
        motor_pas_cu_pas.setSpeed(turatie);
        motor_pas_cu_pas.step(pasi_per_fotografie);
        
        // Incrementare contor fotografii
        fotografii_realizate = fotografii_realizate + 1;
        
        // Actualizare afisaj LCD
        lcd.setCursor(0, 1);
        lcd.print("Foto: ");
        lcd.print(fotografii_realizate);
        lcd.print("/");
        lcd.print(numar_fotografii);
        
        // Trimite comanda Bluetooth pentru declansare camera
        btSerial.print(contor);
        contor++;
        
        // Pauza pentru realizarea fotografiei
        delay(3000);
      }

      // Program terminat - resetare parametri
      if (fotografii_realizate == numar_fotografii) {
        lcd.setCursor(0, 0);
        lcd.print("Program terminat");
        delay(3000);
        lcd.clear();
        
        // Resetare variabile
        fotografii_realizate = 0;
        numar_fotografii = 2;
        submeniu = 0;
        pasi = 0;
      }
    }
  }

  // =========================
  // MENIUL 1 - MOD CINEMATIC
  // =========================
  else if (numar_meniu == 1) {
    // Afisare titlu meniu
    if (submeniu == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Mod Cinematic");
    }

    // Submeniu 1: Setare turatie motor
    else if (submeniu == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Viteza motor (RPM)");
      lcd.setCursor(7, 1);
      lcd.print(turatie);

      // Crestere turatie (joystick sus)
      if (valoare_Y < JOYSTICK_MIN && steag_turatie1 == false) {
        turatie = turatie + 1;
        steag_turatie1 = true;
        lcd.clear();
      }
      
      // Scadere turatie (joystick jos)
      if (valoare_Y > JOYSTICK_MAX && steag_turatie1 == false) {
        turatie = turatie - 1;
        steag_turatie1 = true;
        lcd.clear();
      }
      
      // Resetare steag cand joystick-ul revine in pozitia neutra
      if (valoare_Y >= JOYSTICK_MIN && valoare_Y <= JOYSTICK_MAX && steag_turatie1 == true) {
        steag_turatie1 = false;
      }

      // Limitare turatie in intervalul permis
      if (turatie < MIN_TURATIE) {
        turatie = MIN_TURATIE;
      }
      if (turatie > MAX_TURATIE) {
        turatie = MAX_TURATIE;
      }
    }

    // Submeniu 2: Setare numar rotatii
    else if (submeniu == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Numar rotatii");
      lcd.setCursor(7, 1);
      lcd.print(numar_ture);

      // Crestere numar rotatii (joystick sus)
      if (valoare_Y < JOYSTICK_MIN && steag_rotatii == false) {
        numar_ture = numar_ture + 1;
        steag_rotatii = true;
        lcd.clear();
      }
      
      // Scadere numar rotatii (joystick jos)
      if (valoare_Y > JOYSTICK_MAX && steag_rotatii == false) {
        numar_ture = numar_ture - 1;
        steag_rotatii = true;
        lcd.clear();
      }
      
      // Resetare steag cand joystick-ul revine in pozitia neutra
      if (valoare_Y >= JOYSTICK_MIN && valoare_Y <= JOYSTICK_MAX && steag_rotatii == true) {
        steag_rotatii = false;
      }

      // Limitare numar rotatii in intervalul permis
      if (numar_ture < MIN_TURE) {
        numar_ture = MIN_TURE;
      }
      if (numar_ture > MAX_TURE) {
        numar_ture = MAX_TURE;
      }
    }

    // Submeniu 3: Executie program rotatie continua
    else if (submeniu == 3) {
      lcd.setCursor(0, 0);
      lcd.print("Program in executie");
      lcd.setCursor(0, 1);
      lcd.print("Tura: ");
      lcd.print(tura_curenta);
      lcd.print("/");
      lcd.print(numar_ture);

      // Daca mai sunt ture de realizat
      if (tura_curenta < numar_ture) {
        // Setare viteza si deplasare motor pentru o rotatie completa
        motor_pas_cu_pas.setSpeed(turatie);
        motor_pas_cu_pas.step(rotatie_completa);
        
        // Incrementare contor ture
        tura_curenta = tura_curenta + 1;
        
        // Actualizare afisaj LCD
        lcd.setCursor(0, 1);
        lcd.print("Tura: ");
        lcd.print(tura_curenta);
        lcd.print("/");
        lcd.print(numar_ture);
      }

      // Program terminat - resetare parametri
      if (tura_curenta == numar_ture) {
        lcd.setCursor(0, 0);
        lcd.print("Program terminat");
        delay(3000);
        lcd.clear();
        
        // Resetare variabile
        tura_curenta = 0;
        numar_ture = 1;
        turatie = 15;
        submeniu = 0;
        pasi = 0;
      }
    }
  }

  // =========================
  // MENIUL 2 - CONTROL MANUAL
  // =========================
  else if (numar_meniu == 2) {
    // Afisare titlu meniu
    if (submeniu == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Control manual");
    }

    // Submeniu 1: Control manual rotatie + setare turatie
    else if (submeniu == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Viteza motor (RPM)");
      lcd.setCursor(7, 1);
      lcd.print(turatie);

      // Crestere turatie (joystick sus)
      if (valoare_Y < JOYSTICK_MIN && steag_turatie2 == false) {
        turatie = turatie + 1;
        steag_turatie2 = true;
        lcd.clear();
      }
      
      // Scadere turatie (joystick jos)
      if (valoare_Y > JOYSTICK_MAX && steag_turatie2 == false) {
        turatie = turatie - 1;
        steag_turatie2 = true;
        lcd.clear();
      }
      
      // Resetare steag cand joystick-ul revine in pozitia neutra
      if (valoare_Y >= JOYSTICK_MIN && valoare_Y <= JOYSTICK_MAX && steag_turatie2 == true) {
        steag_turatie2 = false;
      }

      // Limitare turatie in intervalul permis
      if (turatie < MIN_TURATIE) {
        turatie = MIN_TURATIE;
      }
      if (turatie > MAX_TURATIE) {
        turatie = MAX_TURATIE;
      }

      // Rotatie in sens orar (joystick dreapta)
      if (valoare_X < JOYSTICK_MIN) {
        pasi = pasi + 1;
        motor_pas_cu_pas.setSpeed(turatie);
        motor_pas_cu_pas.step(pasi);
        lcd.setCursor(14, 1);
        lcd.print("->");
        steag_ecran = true;
      }

      // Rotatie in sens antiorar (joystick stanga)
      if (valoare_X > JOYSTICK_MAX) {
        pasi = pasi + 1;
        motor_pas_cu_pas.setSpeed(turatie);
        motor_pas_cu_pas.step(-pasi);
        lcd.setCursor(0, 1);
        lcd.print("<-");
        steag_ecran = true;
      }

      // Resetare pasi si stergere indicatori de directie cand joystick-ul revine in pozitia neutra
      if (valoare_X >= JOYSTICK_MIN && valoare_X <= JOYSTICK_MAX) {
        pasi = 0;
        if (steag_ecran == true) {
          lcd.clear();
          steag_ecran = false;
        }
      }
    }

    // Submeniu 2: Iesire din modul manual si resetare parametri
    else if (submeniu == 2) {
      lcd.clear();
      
      // Resetare variabile
      tura_curenta = 0;
      numar_ture = 1;
      turatie = 15;
      submeniu = 0;
      pasi = 0;
    }
  }
}