#include <ctype.h>

WINDOW * shopHud, * shopDescription;
void openWeaponShop(struct Player *player);
void openArmorShop(struct Player *player);
void weaponDescription(int index);
void armorDescription(int index);
void clearShopDescription();
int searchItemByName(const char *query, const char **names, int totalItems);

extern WINDOW *debugHud, *playerhud, *mainScreen, *commandHud, *textHud, *debugMessageHud, *inputUser;
extern void center_box(WINDOW *parent, WINDOW *child, int y_offset, int styley, int stylex);
extern void clearCommandHud(); extern void drawMainScreen();
extern int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx, void (*onHighlight)(int index), bool isExtraKey);
extern void printCenteredSprite(WINDOW *win, int start_y, const char **lines, int lineCount);
extern char* userInput(WINDOW *win, const char *question);

int shopPageStart = 0;
int shopItemsOnPage = 0;
int shopForceHighlight = -1;

enum ShopType 
{
    SHOP_WEAPON,
    SHOP_ARMOR
};

enum ShopType currentShopType = SHOP_WEAPON;

int itemSearch(WINDOW *parent, WINDOW *child)
{
    child = newwin(5, 29, 0, 0);
    center_box(parent, child, 18, 0, 0);

    char *result = userInput(child, "Apa yang ingin kamu cari?");
    delwin(child);
    drawMainScreen();

    // Return jika kosong
    if (!result) return -1;

    int index = -1;

    if (currentShopType == SHOP_WEAPON)
    {
        const char *names[128];
        int totalItems = sizeof(weapon)/sizeof(weapon[0]);

        for (int i = 0; i < totalItems; i++)
            names[i] = weapon[i].name;

        index = searchItemByName(result, names, totalItems);
    } else if (currentShopType == SHOP_ARMOR)
    {
        const char *names[128];
        int totalItems = sizeof(armor)/sizeof(armor[0]);

        for (int i = 0; i < totalItems; i++)
            names[i] = armor[i].name;

        index = searchItemByName(result, names, totalItems);
    }

    free(result);
    drawPlayerHud();
    return index;
}


void shopDescriptionWrapper(int localIndex)
{
    // If user is highlighting spacer / next / back / exit -> ignore
    if (localIndex >= shopItemsOnPage)
    {
        clearShopDescription();
        return;
    }
    int globalIndex = shopPageStart + localIndex;

    if (currentShopType == SHOP_WEAPON)
        weaponDescription(globalIndex);
    else
        armorDescription(globalIndex);
}

int searchItemByName(const char *query, const char **names, int totalItems)
{
    if (!query || strlen(query) == 0)
        return -1;

    // Membuat teks menjadi lowercase
    char lowerQuery[64];
    int qlen = strlen(query);
    for (int i = 0; i < qlen; i++)
        lowerQuery[i] = tolower(query[i]);
    lowerQuery[qlen] = '\0';

    for (int i = 0; i < totalItems; i++)
    {
        char lowerName[64];
        int nlen = strlen(names[i]);

        for (int j = 0; j < nlen; j++)
            lowerName[j] = tolower(names[i][j]);
        lowerName[nlen] = '\0';

        if (strstr(lowerName, lowerQuery)) //Partial search
            return i;
    }

    return -1; // not found
}


void drawShopUI()
{
    shopHud = newwin(9, 29, 0 ,0);
    center_box(mainScreen, shopHud, 10, 0, 0);
    mvwprintw(shopHud, 1, 1, shopSprite);
    box(shopHud, 0, 0);
    wrefresh(shopHud);
}

void clearShopUI()
{
    delwin(shopHud);
    shopHud = newwin(9, 29, 0 ,0);
    center_box(mainScreen, shopHud, 10, 0, 0);
    wrefresh(shopHud);
}

void clearShopDescription()
{
    delwin(shopDescription);
    shopDescription = newwin(7, 27, 0, 0);
    center_box(mainScreen, shopDescription, 11, 32, 32);
    wrefresh(shopDescription);
}

void weaponDescription(int index)
{
    clearShopUI();
    delwin(shopDescription);
    shopDescription = newwin(7, 27, 0, 0);
    center_box(mainScreen, shopDescription, 11, 32, 32);
    wattron(shopDescription, A_BOLD);
    mvwprintw(shopDescription, 0, 0, "%s", weapon[index].name);
    wattroff(shopDescription, A_BOLD);
    mvwprintw(shopDescription, 1, 0, "%s", weapon[index].description);
    mvwprintw(shopDescription, 5, 0, "atk:%d", weapon[index].damage);
    mvwprintw(shopDescription, 5, 7, "spd:%d", weapon[index].speed);
    mvwprintw(shopDescription, 6, 0, "Price:%d", weapon[index].price);
    wrefresh(shopDescription);
}

void armorDescription(int index)
{
    clearShopUI();
    delwin(shopDescription);
    shopDescription = newwin(7, 27, 0, 0);
    center_box(mainScreen, shopDescription, 11, 32, 32);
    wattron(shopDescription, A_BOLD);
    mvwprintw(shopDescription, 0, 0, "%s", armor[index].name);
    wattroff(shopDescription, A_BOLD);
    mvwprintw(shopDescription, 1, 0, "%s", armor[index].description);
    mvwprintw(shopDescription, 5, 0, "def:%d", armor[index].baseDefense);
    mvwprintw(shopDescription, 5, 7, "res:%d", armor[index].resistance);
    mvwprintw(shopDescription, 6, 0, "Price:%d", armor[index].price);
    wrefresh(shopDescription);
}

void startShop()
{
  char *shopChoices[] = {"Weapon", "Armor", "<< Back", NULL};
  while (1)
  {
    drawShopUI();
    int choices = usrInputChoices(shopChoices, commandHud, 1, 1, NULL, false);
    if (choices == 0) openWeaponShop(&player);
    if (choices == 1) openArmorShop(&player);
    if (choices == 2) break;
  }

  clearCommandHud();
  delwin(shopHud);
  drawMainScreen();
}

void addWeaponToInventory(struct Player* player, int weaponIndex) 
{
    for (int i = 0; i < 255; i++) 
    {
        if (player->inventory[0][i] == -1) 
        {
            player->inventory[0][i] = weaponIndex;
            return;
        }
    }
}

void equipWeapon(struct Player* player, int weaponIndex) 
{
    char* playerWeaponBefore = player->weapon.name;
    char* playerWeaponAfter = weapon[weaponIndex].name;
    inputDebugMessage("%s -> %s", playerWeaponBefore, playerWeaponAfter);
    player->weapon = weapon[weaponIndex];
}

void addArmorToInventory(struct Player* player, int armorIndex) 
{
    for (int i = 0; i < 255; i++) 
    {
        if (player->inventory[1][i] == -1) 
        {
            player->inventory[1][i] = armorIndex;
            return;
        }
    }
}

void equipArmor(struct Player* player, int armorIndex) 
{
    char* playerArmorBefore = player->armor.name;
    char* playerArmorAfter = armor[armorIndex].name;
    inputDebugMessage("%s -> %s", playerArmorBefore, playerArmorAfter);
    player->armor = armor[armorIndex];
}

void initInventory(struct Player* player) 
{
    for (int i = 0; i < 255; i++) 
    {
        player->inventory[0][i] = -1; // weapons
        player->inventory[1][i] = -1; // armors
    }
}

void openWeaponShop(struct Player* player)
{
    currentShopType = SHOP_WEAPON;

    int totalItems = sizeof(weapon) / sizeof(weapon[0]);
    int page = 0;

    while (1)
    {
        clearCommandHud();

        char *choices[32];
        int indexStart = page * 16;
        int indexEnd   = indexStart + 16;
        if (indexEnd > totalItems) indexEnd = totalItems;

        int count = 0;

        // Weapon names
        for (int i = indexStart; i < indexEnd; i++)
            choices[count++] = weapon[i].name;

        // Store usable range for callback
        shopPageStart = indexStart;
        shopItemsOnPage = indexEnd - indexStart;

        // Spacer
        int spacerIndex = count;
        choices[count++] = "";

        // Next
        bool hasNext = (indexEnd < totalItems);
        int nextIndex = -1;
        if (hasNext) 
        {
            nextIndex = count;
            choices[count++] = "Next >>";
        }

        // Back
        bool hasBack = (page > 0);
        int backIndex = -1;
        if (hasBack) 
        {
            backIndex = count;
            choices[count++] = "<< Back";
        }

        // Exit
        int exitIndex = count;
        choices[count++] = "Exit Shop";

        choices[count] = NULL;

        int selected = usrInputChoices(choices, commandHud, 1, 1,shopDescriptionWrapper, true);

        if (selected == 47) // "/"
        {
            int found = itemSearch(mainScreen, inputUser);

            if (found >= 0)
            {
                // pergi ke halaman dimana item itu berada
                page = found / 16;
                // dan memberi highligh kepada itemnya
                shopForceHighlight = found % 16;
            }
            else
            {
                matrixAnimationNcurses(textHud, 1, 1, 1, "Item tidak ditemukan");
            }
        
            continue;
        }

        // Select weapon
        if (selected < shopItemsOnPage)
        {
            int itemIndex = indexStart + selected;
            int price = weapon[itemIndex].price;

            if (player->money >= price) 
            {
                player->money -= price;
                addWeaponToInventory(player, itemIndex);
                equipWeapon(player, itemIndex);
                matrixAnimationNcurses(textHud, 1, 1, 1, "Kamu membeli %s", weapon[itemIndex].name);
                drawPlayerHud();
            } else {
                matrixAnimationNcurses(textHud, 1, 1, 1, "Uangmu tidak cukup");
            }

            continue;
        }

        if (selected == spacerIndex) continue;
        if (hasNext && selected == nextIndex) { page++; continue; }
        if (hasBack && selected == backIndex) { page--; continue; }

        if (selected == exitIndex) 
        {
            clearCommandHud();
            return;
        }
    }
}

void openArmorShop(struct Player* player)
{
    currentShopType = SHOP_ARMOR;

    int totalItems = sizeof(armor) / sizeof(armor[0]);
    int page = 0;

    while (1)
    {
        clearCommandHud();

        char *choices[32];
        int indexStart = page * 16;
        int indexEnd   = indexStart + 16;
        if (indexEnd > totalItems) indexEnd = totalItems;

        int count = 0;

        // Armor names
        for (int i = indexStart; i < indexEnd; i++)
            choices[count++] = armor[i].name;

        // Store range for callback
        shopPageStart = indexStart;
        shopItemsOnPage = indexEnd - indexStart;

        // Spacer
        int spacerIndex = count;
        choices[count++] = "";

        // Next
        bool hasNext = (indexEnd < totalItems);
        int nextIndex = -1;
        if (hasNext) {
            nextIndex = count;
            choices[count++] = "Next >>";
        }

        // Back
        bool hasBack = (page > 0);
        int backIndex = -1;
        if (hasBack) 
        {
            backIndex = count;
            choices[count++] = "<< Back";
        }

        // Exit
        int exitIndex = count;
        choices[count++] = "Exit Shop";

        choices[count] = NULL;

        int selected = usrInputChoices(choices, commandHud, 1, 1, shopDescriptionWrapper, true);

        if (selected == 47)
        {
            int found = itemSearch(mainScreen, inputUser);

        if (found >= 0)
        {
            page = found / 16;
            shopForceHighlight = found % 16;
        }
        else
        {
            matrixAnimationNcurses(textHud, 1, 1, 1, "Item tidak ditemukan");
        }
        continue;
        }

        // ----- Selected armor -----
        if (selected < shopItemsOnPage)
        {
            int itemIndex = indexStart + selected;
            int price = armor[itemIndex].price;

            if (player->money >= price) {
                player->money -= price;
                addArmorToInventory(player, itemIndex);
                equipArmor(player, itemIndex);
                matrixAnimationNcurses(textHud, 1, 1, 1, "Kamu membeli %s", armor[itemIndex].name);
                drawPlayerHud();
            } else {
                matrixAnimationNcurses(textHud, 1, 1, 1, "Uangmu tidak cukup");
            }

            continue;
        }

        if (selected == spacerIndex) continue;
        if (hasNext && selected == nextIndex) { page++; continue; }
        if (hasBack && selected == backIndex) { page--; continue; }

        if (selected == exitIndex) 
        {
            clearCommandHud();
            return;
        }
    }
}
