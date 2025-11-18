WINDOW * shopHud;
void openWeaponShop();

extern WINDOW *debugHud, *playerhud, *mainScreen, *commandHud, *textHud, *debugMessageHud;
extern void center_box(WINDOW *parent, WINDOW *child, int y_offset, int styley, int stylex);
extern void clearCommandHud(); extern void drawMainScreen();
extern int usrInputChoices(char *strChoices[], WINDOW *win, int starty, int startx, void (*onHighlight)(int index));
extern void printCenteredSprite(WINDOW *win, int start_y, const char **lines, int lineCount);

void startShop()
{
  shopHud = newwin(9, 29, 0 ,0);
  center_box(mainScreen, shopHud, 10, 0, 0);
  mvwprintw(shopHud, 1, 1, shopSprite);
  box(shopHud, 0, 0);
  wrefresh(shopHud);

  char *shopChoices[] = {"Weapon", "Armor", "<< Back", NULL};
  while (1)
  {
    int choices = usrInputChoices(shopChoices, commandHud, 1, 1, NULL);
    if (choices == 0) openWeaponShop();
    if (choices == 2) break;
  }

  clearCommandHud();
  delwin(shopHud);
  drawMainScreen();
}

void openWeaponShop()
{
    int totalWeapons = sizeof(weapon) / sizeof(weapon[0]);
    int page = 0;

    while (1)
    {
        clearCommandHud();
        char *choices[32]; // enough space
        int indexStart = page * 16;
        int indexEnd = indexStart + 16;
        if (indexEnd > totalWeapons) indexEnd = totalWeapons;

        int count = 0;

        // Add weapons
        for (int i = indexStart; i < indexEnd; i++)
        {
            choices[count] = weapon[i].name;
            count++;
        }

        // Insert spacer line
        choices[count] = " ";
        int spacerIndex = count;
        count++;

        // Add "Next >>" if more pages
        bool hasNext = (indexEnd < totalWeapons);
        int nextIndex = -1;
        if (hasNext)
        {
            choices[count] = "Next >>";
            nextIndex = count;
            count++;
        }

        // Add "<< Back" if not on first page
        bool hasBack = (page > 0);
        int backIndex = -1;
        if (hasBack)
        {
            choices[count] = "<< Back";
            backIndex = count;
            count++;
        }

        // Add exit option
        int exitIndex = count;
        choices[count] = "Exit Shop";
        count++;

        // terminate for usrInputChoices()
        choices[count] = NULL;

        // ask user
        int selected = usrInputChoices(choices, commandHud, 1, 1, NULL);

        // ---------------------------------------------
        // interprets selection
        // ---------------------------------------------

        // If user clicked on a weapon
        if (selected < (indexEnd - indexStart))
        {
            // int weaponIndex = indexStart + selected;
            // showWeaponDetails(weaponIndex); // you implement
            // continue;
        }

        // Spacer line → ignore
        if (selected == spacerIndex)
            continue;

        // Next page
        if (hasNext && selected == nextIndex)
        {
            page++;
            continue;
        }

        // Back page
        if (hasBack && selected == backIndex)
        {
            page--;
            continue;
        }

        // Exit
        if (selected == exitIndex)
            clearCommandHud();
            return;
    }
}
