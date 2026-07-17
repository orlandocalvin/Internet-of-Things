#include <EEPROM.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {3, 2, 1, 0};

LiquidCrystal_I2C lcd(0x27, 16, 2);
Keypad_I2C keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, 0x20);

const int relay = 2;
int dtTombol;
bool ulang;
bool simpan;
bool login = false;

void setup() {
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  lcd.begin();
	lcd.backlight();
  keypad.begin();
  
  EEPROM.write(0, 1);
  EEPROM.write(1, 123);
}
  
void loop() {
  if(!login) {
    cekKeamanan("User ID", EEPROM.read(0),0);
    cekKeamanan("Password", EEPROM.read(1),0);

    printStr(1,"Akses",6,0);
    printStr(0,"diijinkan",4,1);
    delay(1000);

    printStr(1,"Loading",0,0);
    tunggu(7,0);
    login = !login;
  } else {
    digitalWrite(relay, LOW);
    printStr(0,"Anda berada di",0,0);
    printStr(0,"dalam system...",0,1);

    char key = keypad.getKey();
    if(key) {
      if(key == 'A') {
        digitalWrite(relay, HIGH);

        cekKeamanan("Password lama :", EEPROM.read(1),0);
        cekKeamanan("Password baru :", EEPROM.read(1),1);
      } else if(key == '#') {
        digitalWrite(relay, HIGH);
        login = !login;
      }
    }
  }
}

void cekKeamanan(String str,byte data,bool s) {
  dtTombol = 0;
  ulang = true;
  if (s) {
    simpan = true;
  } else {
    simpan = false;
  }
  
  printStr(1,str,0,0);
  
  while(ulang) {
    char key = keypad.getKey();
    if(key) {
      if(key!='A' && key!='B' && key!='C' && key!='D' && key!='#') {
          // tombol hapus
          if(key == '*') {
            dtTombol = (dtTombol-dtTombol%10)/10;
          } else { // akan mengubah dtTombol
            dtTombol = (dtTombol*10) + (key-48);
          }
        printStr(1,str,0,0);
        printInt(dtTombol);
      } else if(key == 'D') { // eksekusi dataTombol
        if(simpan) {
          EEPROM.write(1, dtTombol);
          printStr(1,"Password",0,0);
          printStr(0,"diperbarui",0,1);
          tunggu(10,1);
          ulang = false;
        } else if(!simpan) {
          if(dtTombol == data) {
            ulang = false;
          } else {
            printStr(1,str,0,0);
            printStr(0,"invalid",4,1);
            delay(1000);
            cekKeamanan(str,data,0);
          }
        }
      }
    }
  }
}

void printStr(bool clear,String word,byte col,byte row) {
  if(clear) {
    lcd.clear();
  }
  lcd.setCursor(col,row);
  lcd.print(word);
}

void printInt(int angka) {
  lcd.setCursor(0,1);
  lcd.print(angka);
}

void tunggu(byte col, byte row) {
  for(int i=0;i<3;i++) {
    delay(500);
    lcd.setCursor(i+col,row);
    lcd.print(".");
    delay(500);
  }
}