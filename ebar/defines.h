#define SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define LED                A1
#define LOADCELL_DOUT_PIN  A2
#define LOADCELL_SCK_PIN   A3
#define I2C_ADDR 4

#define INIT_WEIGHT_WARNING 10
int EMPTY_GLASS_WEIGHT = 312;
int FULL_GLASS_WEIGHT = 600;
#define MAX_LIQUID_WEIGHT (FULL_GLASS_WEIGHT-EMPTY_GLASS_WEIGHT)
#define OVERSHOOT 0//14
#define PETINA (MAX_LIQUID_WEIGHT/5)

#define DEVMODE


int RELAY[6] = {9, 10, 11, 12, 13, A0}; //ventily (4), pumpy (2)
int BUTTON[7] = {2, 3, 4, 5, 6, 7, 8}; //predni (4), cernej, klicek (2)
String JMENA[4] = {"Rothbauer", "Szetei", "Jirik", "Kocev"};
String empty = "                ";

//String PITI[4] = {"DRINK1", "DRINK2", "DRINK3", "DRINK4"};
String PITI[4] = {"DZUS POMERANC", "REDENY POMERANC", "DZUS JABLKO", "REDENE JABLKO"};
String REZIM[2] = {"Manualni rezim", "Autom. rezim"};

int percent[4][4] = {
  {100, 0, 0, 0},
  {50, 50, 0, 0},
  {0, 0, 0, 100},
  {0, 0, 50, 50}
};

int grams[4][4];
