#ifndef ENTITY_H
#define ENTITY_H

struct weaponData {
    char* weaponName;
    int damage;
    float critRate;
    float critDamage;
    int speed;
    int effect;
    char description[255];
    int price;
};

struct armorData {
    char* armorName;
    int baseDefense;
    int resistance;
    char description[255];
    int price;
};

struct entityData {
    char* name;
    int maxHealth;
    int health;
    struct weaponData weapon;
    int level;
    int defense;
    int baseDamage;
    int baseExpDrop;
    int baseMoneyDrop;
    float moneyMultiplier;
    float expMultiplier;
    int statusEffect[4];
    const char* enemySprite;
};

struct Player {
    char* name;
    int maxHealth;
    int health;
    struct weaponData weapon;
    struct armorData armor;
    int level;
    int baseDamage;
    int magicPoint;
    int strengthPoint;
    int agilityPoint;
    int defensePoint;
    int exp;
    int money;
    int statusEffect[4];
};

#endif
