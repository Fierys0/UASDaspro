#include "entity.h"

// nama armor, base defense, resistance, deskripsi armor, harga
struct armorData armor[] = {
    {"Maid's Grace", 2, 1, "Elegant maid uniform with light protection", 0},
    {"Urban Stride", 4, 2, "Modern city fashion with comfort", 100},
    {"Leather Embrace", 5, 3, "Supple leather for agile movement", 150},
    {"Summer Breeze", 3, 6, "Light fabrics for warm seasons", 180},
    {"Sorcerer's Veil", 3, 8, "Mystical robes woven with arcane threads", 200},
    {"Festival Bloom", 5, 8, "Colorful attire for celebrations", 300},
    {"Witch's Whisper", 4, 12, "Dark elegance with magical resonance", 350},
    {"Knight's Vow", 10, 5, "Steel plates sworn to protection", 400},
    {"Winter's Kiss", 8, 12, "Fur-lined armor against the cold", 450},
    {"Starlight Weave", 6, 15, "Garments blessed by celestial bodies", 500},
    {"Eastern Harmony", 8, 14, "Traditional attire from eastern lands", 550},
    {"Royal Guard's Pride", 12, 6, "Gilded armor of the elite guard", 600},
    {"Masquerade Mystery", 7, 15, "Enigmatic costume for hidden identities", 650},
    {"Evening Elegance", 4, 18, "Formal wear for grand balls", 700},
    {"Northern Fury", 14, 8, "Heavy furs of northern warriors", 750},
    {"Starlight Festival", 12, 18, "Sparkling attire for celestial events", 800},
    {"Tempest Howl", 9, 20, "Gear that rides the raging winds", 820},
    {"Flame Dancer", 12, 22, "Clothing that dances with embers", 850},
    {"Ocean's Depth", 10, 25, "Armor with abyssal pressure resistance", 880},
    {"Dragon Scale", 15, 10, "Armor forged from fallen drakes", 900},
    {"Thunder Pulse", 11, 28, "Robes charged with electric energy", 920},
    {"Moonlight Sonata", 10, 24, "Elegant dress that glows in moonlight", 950},
    {"Bridal Promise", 6, 20, "Radiant outfit for sacred vows", 950},
    {"Sunrise Promise", 15, 20, "Golden armor that welcomes the dawn", 1100},
    {"Beast King's Mantle", 19, 18, "Fur and claws of alpha predators", 1100},
    {"Mechanical Heart", 22, 12, "Steam-powered defensive systems", 1200},
    {"Dream Weaver", 8, 35, "Armor spun from hopes and dreams", 1400},
    {"Phoenix Rebirth", 18, 25, "Ashes-to-flame protective magic", 1500},
    {"Abyssal Night", 20, 22, "Darkness given tangible form", 1600},
    {"Celestial Chorus", 16, 30, "Armor blessed by heavenly voices", 1800},
    {"Eternal Moment", 25, 28, "Frozen time given physical form", 2200},
    {"Genesis Spark", 28, 32, "The first protection ever conceived", 2500},
    {"Infinity Loop", 30, 35, "Endless cycle of defense and resistance", 3000}
};

void sortArmorsByPrice() {
    int size = sizeof(armor) / sizeof(armor[0]);

    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (armor[i].price > armor[j].price) {
                struct armorData temp = armor[i];
                armor[i] = armor[j];
                armor[j] = temp;
            }
        }
    }
}

