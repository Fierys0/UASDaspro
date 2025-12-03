#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include "armorTable.c"
#include "weaponTable.c"
#include "enemyTable.c"
#include "playerData.c"
#ifdef __unix__
  #include <ncurses.h>
#else
  #include <curses.h>
#endif
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #define MKDIR(path) mkdir(path, 0777)
#endif

// i want it to make delay different oses but it cant use float so 1 is the lowest :(
// windows delay is too slow
#ifdef __unix__
    #define NORMAL_DELAY 1
    #define SLOW_DELAY 10
#else
    #define NORMAL_DELAY 1
    #define SLOW_DELAY 3
#endif

#define AC_BLACK "\x1b[30m"
#define AC_RED "\x1b[31m"
#define AC_GREEN "\x1b[32m"
#define AC_YELLOW "\x1b[33m"
#define AC_BLUE "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN "\x1b[36m"
#define AC_WHITE "\x1b[37m"
#define AC_NORMAL "\x1b[m"
#define STUN 1

extern void matrixAnimationNcurses(int startX, unsigned int characterDelay, unsigned int textDelay, const char* stringData, ...);
extern WINDOW * enemySprite, * enemyHealthHud;
extern WINDOW * mainScreen, * textHud, *commandHud, *playerhud;
extern void drawHealthUI(struct entityData enemy);
extern void drawPlayerHud();
extern void flashWindow(WINDOW *win, int flashes, int delay, int borderstyle);
extern void drawEnemySprite(struct entityData enemy);
extern char* enemyHealthBar;
extern char* playerHealthBar;
extern char* expBar;
extern void inputDebugMessage(const char *messageString, ...);

int baseEXPUP = 65;
int levelUP;
int effectList[] = {STUN};
int effectLength = sizeof(effectList) / sizeof(effectList[0]);

int borderWidth = 40;
int borderHeight = 5;

void matrixAnimation(const char* stringData, unsigned int characterDelay, unsigned int textDelay);
void drawHealth(int health, int maxHealth);
void battleUI(struct Player player, struct entityData enemy);
struct Player startBattle(struct Player player, struct entityData enemy);
void battleAttack(struct Player* player, struct entityData* enemy);
struct entityData randomBattle();
void battleEnd(struct Player* player, struct entityData* enemy);
void mainMenu(struct Player player);
void gameOver();
extern void draw_all();

// menggambar bar [###########]
char* drawBar(int health, int maxHealth)
{
    inputDebugMessage("Drawing bar (%d)(%d)", health, maxHealth);
    int totalBars = 10;

    if (maxHealth <= 0)
        maxHealth = 1;

    if (health < 0)
        health = 0;
    if (health > maxHealth)
        health = maxHealth;

    int filledBars = (health * totalBars) / maxHealth;

    char *result = malloc(13);
    if (!result) return NULL;

    result[0] = '[';

    for (int i = 0; i < totalBars; i++)
        result[i + 1] = (i < filledBars) ? '#' : '-';

    result[11] = ']';
    result[12] = '\0';

    return result;
}

// fungsi musuh menyerang
bool enemyAttackFunc(struct Player* player, struct entityData* enemy)
{
    int playerDamage = player->baseDamage + player->weapon.damage;
    int enemyDamage = enemy->baseDamage + enemy->weapon.damage;
    inputDebugMessage("base enm dmg: %d", enemyDamage);

    for (int i = 0; i <= 3; i++)
    {
        for (int j = 0; j <= effectLength; j++)
        {
            if (enemy->statusEffect[i] != 0)
            {
                enemy->statusEffect[i] == effectList[j];
            }
        }
    }

    bool isPlayerCrit = (1 + rand() % 100) < (player->weapon.critRate * 100);
    bool isEnemyCrit = (1 + rand() % 100) < (enemy->weapon.critRate* 100);
    inputDebugMessage("enemy cr: %d", isEnemyCrit);

    if (isPlayerCrit)
    {
        playerDamage = (int)(playerDamage * player->weapon.critDamage) + 5;
        inputDebugMessage("Player Crit");
    }
    if (isEnemyCrit)
    {
        enemyDamage = (int)(enemyDamage * enemy->weapon.critDamage) + 5;
        inputDebugMessage("crit: %d", enemyDamage);
        inputDebugMessage("Enemy Crit");
    }

    int finalDamage = enemyDamage - player->armor.baseDefense;
    inputDebugMessage("final dmg: %d", finalDamage);
    if (finalDamage < 0) finalDamage = 0;

    player->health -= finalDamage;
    if (player->health < 0) player->health = 0;

    flashWindow(playerhud, 3, 150, 0);

    int playerHealth = player->health;
    int playerMaxHealth = player->maxHealth;

    playerHealthBar = drawBar(playerHealth, playerMaxHealth);
    matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY,
        "%s menyerang %s (DMG: %d)", enemy->name, player->name, finalDamage);

    drawPlayerHud();
    drawHealthUI(*enemy);
    napms(1000);

    if (player->health == 0) return true;
    return false;
}

// Fungsi player menyerang
bool playerAttackFunc(struct Player* player, struct entityData* enemy)
{
    int playerDamage = player->baseDamage * player->level + player->weapon.damage;
    inputDebugMessage("base plyr dmg: %d", playerDamage);
    int enemyDamage = enemy->baseDamage + enemy->weapon.damage;

    for (int i = 0; i <= 3; i++)
    {
        for (int j = 0; j <= effectLength; j++)
        {
            if (enemy->statusEffect[i] != 0)
            {
                enemy->statusEffect[i] == effectList[j];
            }
        }
    }

    bool isPlayerCrit = (1 + rand() % 100) < (player->weapon.critRate * 100);
    bool isEnemyCrit = (1 + rand() % 100) < (enemy->weapon.critRate* 100);

    inputDebugMessage("plyr cr: %d", isPlayerCrit);

    if (isPlayerCrit)
    {
        playerDamage = (int)(playerDamage * player->weapon.critDamage) + 5;
        inputDebugMessage("Player Crit");
        inputDebugMessage("crit dmg: %s", playerDamage);
    }
    if (isEnemyCrit)
    {
        enemyDamage = (int)(enemyDamage * enemy->weapon.critDamage) + 5;
        inputDebugMessage("Enemy Crit");
    }

    int finalDamage = playerDamage - enemy->defense;
    inputDebugMessage("final dmg: %d", finalDamage);
    if (finalDamage < 0) finalDamage = 0;

    enemy->health -= finalDamage;
    if (enemy->health < 0) enemy->health = 0;

    flashWindow(enemySprite, 3, 150, 32);
    drawEnemySprite(*enemy);
    int enemyHealth = enemy->health;
    int enemyMaxHealth = enemy->maxHealth;
    enemyHealthBar = drawBar(enemyHealth, enemyMaxHealth);
    wrefresh(enemyHealthHud);
    drawHealthUI(*enemy);

    matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY,
        "%s menyerang %s (DMG: %d)", player->name, enemy->name, finalDamage);

    drawPlayerHud();
    napms(1000);

    if (enemy->health == 0) {
        matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY, "%s defeated!", enemy->name);
        napms(1000);
        battleEnd(player, enemy);
        napms(1000);
        return true;
    }

    return false;
}

// Fungsi skill dengan damage yang sudah ditentukan
bool playerAttackSkill(struct Player* player, struct entityData* enemy, int skillDamage)
{
    int playerDamage = skillDamage;
    int finalDamage = playerDamage - enemy->defense;
    if (finalDamage < 0) finalDamage = 0;

    enemy->health -= finalDamage;
    if (enemy->health < 0) enemy->health = 0;

    flashWindow(enemySprite, 3, 150, 32);
    drawEnemySprite(*enemy);
    int enemyHealth = enemy->health;
    int enemyMaxHealth = enemy->maxHealth;
    enemyHealthBar = drawBar(enemyHealth, enemyMaxHealth);
    wrefresh(enemyHealthHud);
    drawHealthUI(*enemy);

    matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY,
        "%s menyerang %s (DMG: %d)", player->name, enemy->name, finalDamage);

    drawPlayerHud();
    napms(1000);

    if (enemy->health == 0) {
        matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY, "%s defeated!", enemy->name);
        napms(1000);
        battleEnd(player, enemy);
        napms(1000);
        return true;
    }

    return false;
}

// Fungsi fork untuk battle
void battleAttack(struct Player* player, struct entityData* enemy) 
{
    srand(time(NULL));  
    bool playerFirst = player->weapon.speed >= enemy->weapon.speed;

    if (playerFirst) {
        if (playerAttackFunc(player, enemy)) return;
        if (enemyAttackFunc(player, enemy)) gameOver();
    } else {
        if (enemyAttackFunc(player, enemy)) gameOver();
        if (playerAttackFunc(player, enemy)) return;   
    }
}

// does this even work
void addStatus(struct entityData *enemy, int Status)
{
    for (int i=0; i <= 3; i++)
    {
        if (enemy->statusEffect[i] != 0)
        {
            enemy->statusEffect[i] == Status;
        }
    }
}

// tidur zzzz
void playerRest(struct Player *player)
{
  char *playerName = player->name;
  if (player->money < 5)
  {
    matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY, "Duit kamu kurang!");
    napms(300);
    return;
  }
  player->money -= 5;
  player->health = player->maxHealth;
  matrixAnimationNcurses(1, SLOW_DELAY, SLOW_DELAY, "%s beristirahat!", playerName);
  napms(300);
}

// Fungsi menangkis dengan cara mengurangi damage
void battleDefend(struct Player *player, struct entityData *enemy)
{
    int playerDefense = player->defensePoint + player->armor.baseDefense;
    int enemyAttack = enemy->baseDamage + enemy->weapon.damage;
    if (enemyAttack <= playerDefense)
    {
        addStatus(enemy, STUN);
        return;
    }
    flashWindow(playerhud, 3, 150, 0);
    int parriedAttack = enemyAttack * 0.6;
    inputDebugMessage("base atk: %d", enemyAttack);
    inputDebugMessage("dec atk: %d", parriedAttack);
    player->health = player->health - parriedAttack;
    int playerHealth = player->health;
    int playerMaxHealth = player->maxHealth;
    playerHealthBar = drawBar(playerHealth, playerMaxHealth);
    drawPlayerHud();
    matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY, "berhasil menangkis (damage ditangkis: %d)", enemyAttack - parriedAttack);
}

// Rekursi
int skillChargeAttack(struct Player* player, struct entityData* enemy, int baseDamage, int turn)
{
  napms(150);
  srand(time(NULL));
  int randomMultiply = 2 + rand() % 3;
  int totalDamage = baseDamage * randomMultiply;
  inputDebugMessage("sdmg (x%d) %d t:%d", randomMultiply, totalDamage, turn);
  if (turn){
    turn--;
    enemyAttackFunc(player, enemy);
    if (player->health <= 0) gameOver();
    return skillChargeAttack(player, enemy, totalDamage, turn);
  }
  else {
    return totalDamage;
  }
}

// Fungsi ketika battle selesai
void battleEnd(struct Player* player, struct entityData* enemy) 
{
    srand(time(NULL));

    // Menghitung exp dan uang
    int expGain = enemy->baseExpDrop + rand() % enemy->baseExpDrop;
    int moneyGain = enemy->baseMoneyDrop + rand() % enemy->baseMoneyDrop;

    player->exp += expGain;
    player->money += moneyGain;
    levelUP = (int)((float)(baseEXPUP * player->level * 1.50));

    if (player->exp >= levelUP )
    {
      player->level += 1;
      player->exp -= levelUP;
      player->health = player->maxHealth;
    }

    matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY, "%s mengalahkan %s!", player->name, enemy->name);
    matrixAnimationNcurses(1, NORMAL_DELAY, NORMAL_DELAY, "Mendapat %d EXP dan %d Gold!", expGain, moneyGain);
    enemyHealthBar = "[##########]";
    expBar = drawBar(player->exp, levelUP);
    
    drawPlayerHud();
}

struct entityData randomBattle() 
{
    extern struct entityData enemies[];
    srand(time(NULL));

    int randomIndex = rand() % 2;

    return enemies[randomIndex];
}

void gameOver() {
    matrixAnimationNcurses(1, SLOW_DELAY, SLOW_DELAY, "GAME OVER!");
    usleep(100000);
    getch();
    endwin();
    exit(0);
}

void savePlayer(struct Player *player)
{
    const char *saveFolder = "save";
    const char *saveLocation = "save/save.dat";

    MKDIR(saveFolder);

    FILE *f = fopen(saveLocation, "wb");
    if (!f) {
        inputDebugMessage("Failed to save %s", saveLocation);
        return;
    }

    inputDebugMessage("save %s", saveLocation);
    fwrite(player, sizeof(struct Player), 1, f);
    fclose(f);

    matrixAnimationNcurses(1, SLOW_DELAY, SLOW_DELAY, "Game saved!");
    napms(300);
}

void loadPlayer(struct Player *player)
{
    char *loadLocation = "save/save.dat";
    FILE *f = fopen(loadLocation, "rb");
    if (!f) return;
    inputDebugMessage("load %s", loadLocation);

    fread(player, sizeof(struct Player), 1, f);

    fclose(f);

    matrixAnimationNcurses(1, SLOW_DELAY, SLOW_DELAY, "Game loaded!");
    draw_all();
}

