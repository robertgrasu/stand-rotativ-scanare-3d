# Stand Rotativ pentru Scanare 3D
## Proiect Proiectare cu Microprocesoare

![poza ansamblu](Poze%20proiect/proiect-pm1.jpg)

## Descriere

Proiectul de stand rotativ pentru scanare 3D este realizat utilizând placa de dezvoltare Arduino Nano. Dispozitivul permite rotirea controlată a unui obiect și fotografierea sa automată din unghiuri precise, imaginile putând fi ulterior procesate în Autodesk Recap Photo pentru generarea modelelor 3D.

Construcția standului include un motor pas cu pas 28BYJ-48 conectat la un driver ULN2003, care asigură rotirea precisă a platformei. Interacțiunea cu utilizatorul este realizată prin intermediul unui modul joystick și a unui display LCD 16x2 I2C pentru afișarea informațiilor. Comunicarea cu telefonul mobil pentru declanșarea automată a camerei foto este implementată prin intermediul unui modul Bluetooth HC-05. Pentru alimentarea componentelor electronice a fost integrat un modul de alimentare multiplă.

## Componente hardware

- Arduino Nano
- Motor pas cu pas 28BYJ-48
- Driver ULN2003 pentru controlul motorului
- Display LCD 16x2 cu interfață I2C
- Modul joystick analogic cu buton
- Modul Bluetooth HC-05
- Modul de alimentare multiplă
- Carcasă fabricată prin imprimare 3D

## Functionarea proiectului

Proiectul implementează trei moduri principale de operare:

### Modul de fotografiere
Permite setarea numărului de fotografii pentru o rotație completă. Sistemul rotește platforma și declanșează camera telefonului prin Bluetooth la fiecare poziție, afișând progresul pe ecranul LCD.

### Modul cinematic
Controlează viteza de rotație (RPM) și numărul de rotații complete, util pentru filmarea obiectelor în mișcare continuă.

### Modul de control Manual
Permite rotirea platformei în ambele direcții utilizând joystick-ul, cu viteză ajustabilă pentru poziționarea precisă a obiectelor.

## Integrare Bluetooth

Modulul Bluetooth HC-05 asigură comunicarea wireless cu un smartphone Android. În modul de fotografiere, sistemul trimite automat comenzi către telefon atunci când platforma ajunge la unghiul dorit, declanșând camera foto. Această funcționalitate necesită instalarea unei aplicații compatibile pe telefon, care poate interpreta comenzile primite prin Bluetooth.

## Funcționarea sistemului

Navigarea prin meniuri se realizează folosind joystick-ul: mișcările stânga-dreapta pentru schimbarea meniului principal, mișcările sus-jos pentru ajustarea valorilor, iar apăsarea butonului pentru confirmarea selecției și intrarea în submeniuri.

Sistemul folosește un algoritm de control pentru divizarea precisă a unei rotații complete în numărul dorit de fotografii. Motorul pas cu pas, asociat cu sistemul de angrenaje din carcasă, realizează rotațiile la pozițiile calculate cu precizie.

În modul fotografiere, după fiecare rotație parțială, sistemul așteaptă 3 secunde pentru stabilizarea platformei, trimite comanda de declanșare către telefon, apoi continuă cu următoarea poziție.


## Implementare hardware

### Schema electrică

Schema electrică a proiectului prezintă interconexiunile între Arduino Nano și celelalte componente electronice.

![schema electrica](Poze%20proiect/schema-proiect-pm.png)


### Carcasa
Carcasa a fost proiectată în Autodesk Fusion 360 și imprimată 3D. Aceasta conține un sistem de angrenaje pentru transmisia mișcării de la motor la platoul rotativ.

![Carcasa](Poze%20proiect/carcasa-proiect.png)

Structura interioară a standului

![Interior stand](Poze%20proiect/proiect_pm4.png)
