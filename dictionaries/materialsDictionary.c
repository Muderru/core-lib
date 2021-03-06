//*****************************************************************************
// Class: materialsDictionary
// File Name: materialsDictionary.c
//
// Copyright (c) 2018 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//
//*****************************************************************************
#include "/lib/include/inventory.h"
#include "/lib/include/itemFormatters.h"
#include "materials/materials.h"
#include "materials/weapons.h"
#include "materials/armor.h"
#include "materials/instruments.h"
#include "materials/drinks.h"
#include "materials/food.h"
#include "materials/potions.h"

private nosave string EquipmentBlueprint = "lib/items/equipment.c";
private nosave string DetailsText = "\t\x1b[0;36m%s: \x1b[0m\x1b[0;33m%d to %d\x1b[0m\n";
private nosave string SingleDetailText = "\t\x1b[0;36m%s: \x1b[0m\x1b[0;33m%d\x1b[0m\n";
private nosave string DetailString = "\t\x1b[0;36m%s: \x1b[0m\x1b[0;33m%s\x1b[0m\n";

private nosave string *validBonuses = ({ "strength", "intelligence", "dexterity",
    "wisdom", "constitution", "charisma",  });
    
private nosave string *validArmorLocations = ({ "armor", "gloves", "helmet", 
    "boots", "ring", "cloak", "amulet", "belt", "arm greaves", "leg greaves",
    "bracers" });
private nosave string *validArmorTypes = ({ "plate armor", "splint armor", "chainmail",
    "scalemail", "hard leather", "soft leather", "no armor" });
private nosave string *validWeaponTypes = ({ "shield", "axe", "long sword",
    "hand and a half sword", "short sword", "dagger", "bow", "crossbow", "sling",
    "thrown", "two-handed sword", "pole arm", "mace", "hammer", "flail", "staff" });
private nosave string *validDamageTypes = ({ "slash", "bludgeon", "thrust" });

/////////////////////////////////////////////////////////////////////////////
private nomask int isValidItem(object item)
{
    return (item && objectp(item) && 
            (member(inherit_list(item), EquipmentBlueprint) > -1));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidMaterial(string material)
{
    return (materials && mappingp(materials) &&
            member(materials, material) &&
            member(materials[material], "crafting skill required") &&
            member(materials[material], "class"));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidMaterialType(string materialType)
{
    return (member(({ "wood", "textile", "stone", "leather", "metal", 
        "crystal", "exotic" }), materialType) > -1);
}

/////////////////////////////////////////////////////////////////////////////
public nomask string *slotsUsedByItemMask(int mask)
{
    string *items = ({});

    if (mask & Armor)
    {
        items += ({ "armor" });
    }
    if (mask & Cloak)
    {
        items += ({ "cloak" });
    }
    if (mask & Helmet)
    {
        items += ({ "helmet" });
    }
    if (mask & Amulet)
    {
        items += ({ "amulet" });
    }
    if (mask & Belt)
    {
        items += ({ "belt" });
    }
    if (mask & ArmGreaves)
    {
        items += ({ "arm greaves" });
    }
    if (mask & LegGreaves)
    {
        items += ({ "leg greaves" });
    }
    if (mask & Gloves)
    {
        items += ({ "gloves" });
    }
    if (mask & Bracers)
    {
        items += ({ "bracers" });
    }
    if (mask & Boots)
    {
        items += ({ "boots" });
    }
    if (mask & Ring)
    {
        items += ({ "ring 1", "ring 2" });
    }
    if (mask & PrimaryWeapon)
    {
        items += ({ "wielded primary" });
    }
    if (mask & OffhandWeapon)
    {
        items += ({ "wielded offhand" });
    }

    return items + ({});
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getMaterialTypeForMaterial(string material)
{
    string type = 0;

    if(isValidMaterial(material) && 
        isValidMaterialType(materials[material]["class"]))
    {
        type = materials[material]["class"];
    }
    return type;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidWeaponBlueprint(string type)
{
    return (weaponBlueprints && mappingp(weaponBlueprints) &&
            member(weaponBlueprints, type) &&
            member(weaponBlueprints[type], "skill to craft") &&
            member(weaponBlueprints[type], "type") &&
            member(weaponBlueprints[type], "default encumberance"));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidArmorBlueprint(string type)
{
    return (armorBlueprints && mappingp(armorBlueprints) &&
        member(armorBlueprints, type) &&
        member(armorBlueprints[type], "skill to craft") &&
        member(armorBlueprints[type], "type") &&
        member(armorBlueprints[type], "default encumberance"));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidInstrumentBlueprint(string type)
{
    return (instrumentBlueprints && mappingp(instrumentBlueprints) &&
        member(instrumentBlueprints, type) &&
        member(instrumentBlueprints[type], "skill to craft") &&
        member(instrumentBlueprints[type], "type") &&
        member(instrumentBlueprints[type], "default encumberance"));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidFoodBlueprint(string type)
{
    return (food && mappingp(food) && member(food, type) &&
        member(food[type], "crafting prerequisites") && 
        member(food[type], "type") &&
        member(food[type], "crafting materials"));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidDrinkBlueprint(string type)
{
    return (drinks && mappingp(drinks) && member(drinks, type) &&
        member(drinks[type], "crafting prerequisites") && 
        member(drinks[type], "type") &&
        member(drinks[type], "crafting materials"));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidPotionBlueprint(string type)
{
    return (potions && mappingp(potions) && member(potions, type) &&
        member(potions[type], "crafting prerequisites") &&
        member(potions[type], "type") &&
        member(potions[type], "effects") &&
        member(potions[type], "crafting materials"));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidArmorLocation(string type)
{
    return (member(validArmorLocations, type) > -1);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidArmorType(string type)
{
    return (member(validArmorTypes, type) > -1);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidWeaponType(string type)
{
    return (member(validWeaponTypes, type) > -1);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidDamageType(string type)
{
    return (member(validDamageTypes, type) > -1);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidBonus(string bonus)
{
    return (member(validBonuses, bonus) > -1);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getBlueprintModifier(object item, string type)
{
    int ret = 0;
    string itemType = item->query("blueprint");
    if (isValidWeaponBlueprint(itemType))
    {
        ret = weaponBlueprints[itemType][type];
    }
    else if (isValidArmorBlueprint(itemType))
    {
        ret = armorBlueprints[itemType][type];
    }
    else if (isValidMaterial(itemType))
    {
        ret = materials[itemType][type];
    }
    else if (isValidInstrumentBlueprint(itemType))
    {
        ret = instrumentBlueprints[itemType][type];
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getMaterialCraftsmanshipBonus(object item)
{
    int retVal = 0;
    
    if(isValidItem(item))
    {
        retVal = item->query("craftsmanship");
        
        string material = item->query("material");
        if(isValidMaterial(material))
        {
            retVal -= materials[material]["crafting skill required"];
        }
        
        retVal -= getBlueprintModifier(item, "skill to craft");
    }

    return (retVal > 0) ? retVal / 5 : 0;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getDefaultValue(object item)
{
    int ret = getBlueprintModifier(item, "default value");

    string material = item->query("material");

    if (member(materials, material) && member(materials[material],
        "value multiplier"))
    {
        ret = to_int(ret * materials[material]["value multiplier"]);
    }
    return to_int(ret + (250 * (1 + 0.35 * (getMaterialCraftsmanshipBonus(item))) *
        (1 + 10 * item->query("enchanted"))));
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getMaterialEncumberance(object item)
{
    int retVal = 0;
    
    if(isValidItem(item))
    {
        string material = item->query("material");
        retVal -= getMaterialCraftsmanshipBonus(item);
        retVal = (retVal > 0) ? retVal : 0;

        if(isValidMaterial(material))
        {
            if(member(materials[material], "encumberance"))
            {
                retVal = materials[material]["encumberance"];
            }
        }
    }

    return  retVal;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string *hasExtraAttackTypes(object item)
{
    string *retVal = 0;

    if(isValidItem(item) && item->query("weapon type"))
    {
        string material = item->query("material");
        if(isValidMaterial(material) && member(materials[material], "attack"))
        {    
            string *listOfAttackTypes = m_indices(materials[material]["attack"]);
            
            if(member(listOfAttackTypes, "physical") > -1)
            {
                listOfAttackTypes -= ({ "physical" });
            }
            
            if (sizeof(listOfAttackTypes))
            {
                retVal = listOfAttackTypes;
            }
        }
    }
    return retVal;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string *getMaterialDamageType(object weapon)
{
    string *ret;
    if (isValidItem(weapon))
    {
        string weaponType = weapon->query("blueprint");
        if (isValidWeaponBlueprint(weaponType))
        {
            ret = ({ weaponBlueprints[weaponType]["damage type"] });
        }
        else
        {
            ret = ({ "physical" });
        }

        string material = weapon->query("material");
        if (isValidMaterial(material) && member(materials[material], "attack"))
        {
            string *materialDmgs = m_indices(materials[material]["attack"]);
            materialDmgs -= ({ "physical" });

            ret += materialDmgs;
        }
    }
    return ret + ({});
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getMaterialDamage(object item, string type)
{
    int retVal = 0;

    if(isValidItem(item))
    {
        string material = item->query("material");
        if(isValidMaterial(material))
        {
            if(member(materials[material], "attack") &&
               member(materials[material]["attack"], type))
            {
                retVal = materials[material]["attack"][type];
            }
        }
                  
        retVal += getMaterialCraftsmanshipBonus(item);
    }
    return retVal;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getMaterialDefense(object item, string type)
{
    int retVal = 0;
    if(isValidItem(item))
    {
        string material = item->query("material");
        if(isValidMaterial(material))
        {
            if(member(materials[material], "defense") &&
               member(materials[material]["defense"], type))
            {
                retVal = materials[material]["defense"][type];
            }
        }
                  
        retVal += getMaterialCraftsmanshipBonus(item);
    }
    return retVal;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getMaterialDefendAttack(object item)
{
    int retVal = 0;

    if(isValidItem(item))
    {
        retVal += getMaterialCraftsmanshipBonus(item);
    }
    return retVal;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getMaterialAttack(object item)
{
    int retVal = 0;

    if (isValidItem(item))
    {
        string material = item->query("material");
        if (isValidMaterial(material))
        {
            if (member(materials[material], "attack rating"))
            {
                retVal += materials[material]["attack rating"];
            }
        }

        retVal += getMaterialCraftsmanshipBonus(item);
    }
    return retVal;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getMaterialDetails(object item)
{
    string retVal = "";

    return retVal;
}

/////////////////////////////////////////////////////////////////////////////
public varargs string getMaterialQualityFormatter(object equipment)
{
    string ret = Cyan;

    if (getMaterialCraftsmanshipBonus(equipment) > 4)
    {
        ret = "\x1b[0;32;1m%-20s\x1b[0m";
    }
    else if (equipment->query("enchanted") > 4)
    {
        ret = "\x1b[0;35;1m%-20s\x1b[0m";
    }
    else if (getMaterialCraftsmanshipBonus(equipment))
    {
        ret = WellCrafted;
    }
    else if (equipment->query("enchanted"))
    {
        ret = Enchanted;
    }

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public varargs string applyMaterialQualityToText(object equipment, string text)
{
    string ret = NormalEquipment;
    string qualityText = "typical for its type";

    if (getMaterialCraftsmanshipBonus(equipment) > 4)
    {
        ret = Masterwork;
        if(!text)
        {
            qualityText = "a masterwork item";
        }
    }
    else if (equipment->query("enchanted") > 4)
    {
        ret = StrongEnchantment;
        if (!text)
        {
            qualityText = "enchanted with a powerful aura";
        }
    }
    else if (equipment->query("enchanted"))
    {
        ret = Enchanted;
        if (!text)
        {
            qualityText = "enchanted";
        }
    }
    else if (getMaterialCraftsmanshipBonus(equipment))
    {
        ret = WellCrafted;
        if (!text)
        {
            qualityText = "a well-crafted item";
        }
    }

    if(!text)
    {
        text = sprintf("This %s is %s.\n", 
            equipment->query("blueprint") || "item",
            qualityText);
    }
    return sprintf(ret, text);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int canCraftBlueprintWithMaterial(object initiator, 
                                                string blueprint,
                                                string material)
{
    int ret = 0;

    if (initiator && member(materials, material))
    {
        ret = 1;
        int skillLevel;

        if (member(weaponBlueprints, blueprint))
        {
            if (member(({ "bow", "crossbow" }), weaponBlueprints[blueprint]["type"]) > -1)
            {
                skillLevel = initiator->getSkill("bowyer and fletcher");
            }
            else
            {
                skillLevel = initiator->getSkill("weapon smithing");
            }
            ret &&= skillLevel >= weaponBlueprints[blueprint]["skill to craft"];
        }
        else if (member(armorBlueprints, blueprint))
        {
            ret &&= initiator->getSkill("armorer") >=
                armorBlueprints[blueprint]["skill to craft"];
        }

        switch(materials[material]["class"])
        {
            case "wood":
            {
                skillLevel = initiator->getSkill("wood crafting");
                break;
            }
            case "stone":
            {
                skillLevel = initiator->getSkill("stonemasonry");
                break;
            }
            case "crystal":
            {
                skillLevel = initiator->getSkill("gem crafting");
                break;
            }
            case "metal":
            {
                skillLevel = initiator->getSkill("metal crafting");
                break;
            }
            case "leather":
            {
                skillLevel = initiator->getSkill("leatherworking");
                break;
            }
            case "textile":
            {
                skillLevel = initiator->getSkill("sewing");
                break;
            }
            case "exotic":
            {
                skillLevel = initiator->getSkill("spellcraft");
                break;
            }
        }
        ret &&= skillLevel >= materials[material]["crafting skill required"];
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int calculateServiceBonuses(string methodToCheck, object initiator)
{
    int ret = 0;

    string *servicesToCheck = ({ "races", "guilds", "research", "traits",
        "biological", "background" });

    foreach(string serviceToCheck in servicesToCheck)
    {
        if (initiator->has(serviceToCheck))
        {
            ret += call_other(initiator,
                sprintf("%sBonusTo", serviceToCheck), methodToCheck);
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int getAttackData(object weapon, object initiator)
{
    int baseAttack = weapon->query("weapon attack") +
        getMaterialAttack(weapon);

    if (initiator)
    {
        baseAttack += initiator->magicalAttackBonus() +
            calculateServiceBonuses("AttackBonus", initiator) +
            (initiator->dexterityBonus() / 2) +
            (initiator->intelligenceBonus() / 2);

        string skillToUse = weapon->query("weapon type");

        if (skillToUse && stringp(skillToUse))
        {
            baseAttack += call_other(initiator, "getSkillModifier",
                skillToUse);
        }
        baseAttack -= weapon->query("skill penalty");
    }
    return baseAttack;
}

/////////////////////////////////////////////////////////////////////////////
private nomask string getWeaponAttackInformation(object weapon, object initiator)
{
    int baseAttack = getAttackData(weapon, initiator);

    return sprintf(DetailsText, "Attack", baseAttack, baseAttack + 100);
}

/////////////////////////////////////////////////////////////////////////////
private nomask string convertDamageMappingToString(mapping damages)
{
    string ret = 0;

    if (damages && sizeof(damages))
    {
        ret = "";
        string *damagetKeys = sort_array(m_indices(damages),
            (: return $1 > $2; :));

        foreach(string damage in damagetKeys)
        {
            ret += sprintf(SpecialAttack, sprintf(" [+%d %s]",
                damages[damage], damage));
        }
        ret += "\n";
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask string applyEnchantments(object weapon)
{
    string ret = 0;

    mapping enchantments = weapon->query("enchantments");
    if (!enchantments)
    {
        enchantments = ([]);
    }

    string *extraDamages = hasExtraAttackTypes(weapon);
    if(sizeof(extraDamages))
    {
        foreach(string extraDamage in extraDamages)
        {
            enchantments[extraDamage] = member(enchantments, extraDamage) ?
                (enchantments[extraDamage] + getMaterialDamage(weapon, extraDamage)) :
                getMaterialDamage(weapon, extraDamage);
        }
    }

    return convertDamageMappingToString(enchantments);
}

/////////////////////////////////////////////////////////////////////////////
private nomask string applyResistances(object item)
{
    string ret = 0;

    mapping resistances = item->query("resistances");
    if (!resistances)
    {
        resistances = ([]);
    }

    string material = item->query("material");
    if (material && member(materials, material) &&
        member(materials[material], "defense") &&
        item->query("armor type"))
    {
        string *materialResistances = m_indices(materials[material]["defense"]);
        if (sizeof(materialResistances))
        {
            foreach(string materialResistance in materialResistances)
            {
                resistances[materialResistance] = member(resistances, materialResistance) ?
                    (resistances[materialResistance] + getMaterialDefense(item, materialResistance)) :
                    getMaterialDefense(item, materialResistance);
            }

            if (item->query("armor type") && member(resistances, "physical"))
            {
                m_delete(resistances, "physical");
            }
        }
    }

    return convertDamageMappingToString(resistances);
}

/////////////////////////////////////////////////////////////////////////////
private nomask int getDamageData(object weapon, object initiator)
{
    int baseDamage = weapon->query("weapon class") +
        getMaterialDamage(weapon, "physical");

    if (initiator)
    {
        baseDamage += initiator->magicalDamageBonus() +
            calculateServiceBonuses("DamageBonus", initiator) +
            (initiator->strengthBonus() / 2) +
            (initiator->wisdomBonus() / 4) +
            (initiator->intelligenceBonus() / 4);

        string skillToUse = weapon->query("weapon type");
        if (skillToUse && stringp(skillToUse))
        {
            baseDamage += call_other(initiator, "getSkillModifier",
                skillToUse) / 2;
        }
        baseDamage -= weapon->query("skill penalty");
    }
    return baseDamage;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getWeaponDamageInformation(object weapon, object initiator)
{
    string ret = "";
    int baseDamage = getDamageData(weapon, initiator);

    float modifier = baseDamage / 8.0;
    ret = sprintf(DetailsText, "Damage", 
        to_int(baseDamage - modifier), to_int(baseDamage + modifier));

    string enchantments = applyEnchantments(weapon);
    if (enchantments)
    {
        ret -= "\n";
        ret += enchantments;
    }

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int getWeaponDefenseData(object weapon, object initiator)
{
    int baseDefense = weapon->query("defense class") +
        getMaterialDefendAttack(weapon);

    if (initiator)
    {
        baseDefense += initiator->magicalDefendAttackBonus() +
            calculateServiceBonuses("DefendAttackBonus", initiator) +
            (initiator->dexterityBonus() / 2) +
            (initiator->wisdomBonus() / 2);

        string skillToUse = weapon->query("weapon type");
        if (skillToUse && stringp(skillToUse))
        {
            baseDefense += call_other(initiator, "getSkillModifier",
                skillToUse) / 2;
        }
        baseDefense -= weapon->query("skill penalty");
    }
    return baseDefense;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getWeaponDefenseInformation(object weapon, object initiator)
{
    int baseDefense = getWeaponDefenseData(weapon, initiator);

    float modifier = baseDefense / 8.0;
    return sprintf(DetailsText, "Defense",
        to_int(baseDefense - modifier), to_int(baseDefense + modifier));
}

/////////////////////////////////////////////////////////////////////////////
private nomask int getEncumberanceData(object item, object initiator)
{
    int encumberance = item->query("encumberance") +
        getMaterialEncumberance(item);

    if (initiator)
    {
        string skillToUse = item->query("weapon type") ||
            item->query("armor type");
        if (skillToUse && stringp(skillToUse))
        {
            encumberance -= call_other(initiator, "getSkillModifier",
                skillToUse);
        }
        encumberance += item->query("skill penalty");
    }
    return encumberance;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getEncumberance(object item, object initiator)
{
    int encumberance = getEncumberanceData(item, initiator);

    return sprintf(SingleDetailText, "Encumberance", encumberance);
}

/////////////////////////////////////////////////////////////////////////////
private nomask string applyBonusDetails(object item)
{
    string ret = "";
    string *bonuses = sort_array(item->query("bonuses"),
                                (: return $1 > $2; :));

    if (sizeof(bonuses))
    {
        foreach(string bonus in bonuses)
        {
            ret += sprintf(Value,
                sprintf("\t%s: %d\n", capitalize(bonus), item->query(bonus)));
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask string applyMaterialDetails(object item)
{
    string ret = "";
    string material = item->query("material");
    if (material)
    {
        ret += sprintf(DetailString, "Material", capitalize(material));
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask string applyWeaponDetails(object weapon, object initiator)
{
    return applyMaterialDetails(weapon) +
        getWeaponAttackInformation(weapon, initiator) +
        getWeaponDamageInformation(weapon, initiator) +
        getWeaponDefenseInformation(weapon, initiator) +
        getEncumberance(weapon, initiator);
}

/////////////////////////////////////////////////////////////////////////////
private nomask int getDamageProtectionData(object armor, object initiator)
{
    int baseAC = armor->query("armor class") +
        getMaterialDefense(armor, "physical");

    if (initiator)
    {
        baseAC += initiator->magicalDefenseBonus() +
            calculateServiceBonuses("DefenseBonus", initiator) +
            (initiator->constitutionBonus() / 2) +
            (initiator->strengthBonus() / 2);
    }
    return baseAC;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getDamageProtection(object armor, object initiator)
{
    int baseAC = getDamageProtectionData(armor, initiator);

    return sprintf(SingleDetailText, "Damage Protection", baseAC);
}

/////////////////////////////////////////////////////////////////////////////
private nomask string applyArmorDetails(object armor, object initiator)
{
    string ret = getDamageProtection(armor, initiator);
    
    string resistances = applyResistances(armor);
    if (resistances)
    {
        ret -= "\n";
        ret += resistances;
    }
    return applyMaterialDetails(armor) + 
        ret + 
        getEncumberance(armor, initiator);
}

/////////////////////////////////////////////////////////////////////////////
private nomask string applyNonEquipmentDetails(object item)
{
    string ret = applyMaterialDetails(item);

    string enchantments = applyEnchantments(item);
    if (enchantments)
    {
        ret += sprintf(Value, sprintf("\tEnchantments: %s", enchantments));
    }
    string resistances = applyResistances(item);
    if (resistances)
    {
        ret += sprintf(Value, sprintf("\tResistances: %s", resistances));
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private int spellcraftCanIdentifyItem(object item, object initiator)
{
    return item && initiator && ((item->query("enchanted") * 5) <=
        (initiator->getSkillModifier("spellcraft")));
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getEquipmentStatistics(object equipment, object initiator)
{
    string ret = applyMaterialQualityToText(equipment);

    if(equipment->query("identified") || canCraftBlueprintWithMaterial(initiator,
        equipment->query("blueprint"), equipment->query("material")))
    {
        if (equipment->query("weapon type"))
        {
            ret += applyWeaponDetails(equipment, initiator);
        }
        else if (equipment->query("armor type"))
        {
            ret += applyArmorDetails(equipment, initiator);
        }
        else
        {
            ret += applyNonEquipmentDetails(equipment);
        }
    }

    if (equipment->query("identified") ||
        spellcraftCanIdentifyItem(equipment, initiator))
    {
        ret += applyBonusDetails(equipment);
    }

    if (equipment->query("weight"))
    {
        ret += sprintf(SingleDetailText, "Weight", equipment->query("weight"));
    }

    if (spellcraftCanIdentifyItem(equipment, initiator) &&
        canCraftBlueprintWithMaterial(initiator,
            equipment->query("blueprint"), equipment->query("material")))
    {
        equipment->identify();
    }

    if (equipment->query("identified") && equipment->query("cursed"))
    {
        ret += sprintf(BoldBlack, "This item is cursed!\n");
    }

    if (!equipment->query("identified"))
    {
        ret += sprintf(Unidentified, "This item has not been identified.\n");
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getItemSummary(object equipment)
{
    string ret = "\x1b[0;30;1mThis item has not been identified\x1b[0m";

    if (equipment->query("identified") || canCraftBlueprintWithMaterial(0,
        equipment->query("blueprint"), equipment->query("material")))
    {
        if (equipment->query("weapon type"))
        {
            ret = sprintf("    \x1b[0;36mAttack: \x1b[0;32m%2d\x1b[0;36m, Damage: \x1b[0;32m%2d\x1b[0;36m,"
                " Defense: \x1b[0;32m%2d\x1b[0m", 
                getAttackData(equipment, 0),
                getDamageData(equipment, 0), 
                getWeaponDefenseData(equipment, 0));
        }
        else if (equipment->query("armor type"))
        {
            ret = sprintf("    \x1b[0;36mSoak: \x1b[0;32m%2d\x1b[0;36m, Encumberance: \x1b[0;32m%2d\x1b[0m",
                getDamageProtectionData(equipment, 0),
                getEncumberanceData(equipment, 0));
        }
        else
        {
            ret = "    \x1b[0;36mView description for summary\x1b[0m";
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask string *getWeightedMaterial(string material)
{
    string *weightedMaterials = ({});
    int numToAdd = 0;
    switch (materials[material]["scarcity"])
    {
        case "common":
        {
            numToAdd = 8;
            break;
        }
        case "uncommon":
        {
            numToAdd = 3;
            break;
        }
        case "rare":
        {
            numToAdd = 1;
            break;
        }
        case "very rare":
        {
            numToAdd = !random(3);
            break;
        }
    }

    for (int i = 0; i < numToAdd; i++)
    {
        weightedMaterials += ({ material });
    }
    return weightedMaterials;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs string getRandomMaterialFromClass(string materialClass,
    string defaultMaterial)
{
    int useCrystals = materialClass == "metal";

    string *options = filter(m_indices(materials),
        (: (($2[$1]["class"] == $3) ||
        ($4 && !random(10) && ($2[$1]["class"] == "crystal"))) :),
        materials, materialClass, useCrystals);

    string *weightedMaterials = ({});

    if (defaultMaterial)
    {
        for (int i = 0; i < 100; i++)
        {
            weightedMaterials += ({ defaultMaterial });
        }
    }

    if (sizeof(options))
    {
        foreach(string material in options)
        {
            weightedMaterials += getWeightedMaterial(material);
        }
    }
    return weightedMaterials[random(sizeof(weightedMaterials))];
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getRandomMaterial(object item)
{
    string defaultMaterial = item->query("material");
    string ret = defaultMaterial;
    if (member(materials, defaultMaterial))
    {
        string materialClass = materials[defaultMaterial]["class"];
        ret = getRandomMaterialFromClass(materialClass, defaultMaterial);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int getRandomCraftsmanshipBonus(object item)
{
    int ret = 5 + random(15);

    string itemBlueprint = item->query("blueprint");
    if (member(weaponBlueprints, itemBlueprint))
    {
        ret += weaponBlueprints[itemBlueprint]["skill to craft"];
    }
    else if (member(armorBlueprints, itemBlueprint))
    {
        ret += armorBlueprints[itemBlueprint]["skill to craft"];
    }

    if (!random(5))
    {
        ret += 20 + random(25);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask string *bonusList(object item)
{
    string *bonuses = ({ "bonus strength", "bonus intelligence", "bonus dexterity",
        "bonus wisdom", "bonus constitution", "bonus charisma", "bonus armor class",
        "bonus defense class", "bonus attack", "bonus soak", "bonus defense",
        "bonus spell points", "bonus stamina points", "bonus heal hit points rate",
        "bonus heal hit points", "bonus heal spell points", "bonus hit points",
        "bonus heal spell points rate", "bonus heal stamina",
        "bonus heal stamina rate", "damage reflection",
        "bonus defense class", "bonus damage", "bonus weapon attack" });

    if (item->query("weapon type"))
    {
        bonuses += ({ item->query("weapon type") });
    }
    else if (item->query("armor type"))
    {
        bonuses += ({ item->query("armor type") });
    }
    else
    {
        bonuses +=
            load_object("/lib/dictionaries/skillsDictionary.c")->validBonusSkills();
    }
    return bonuses;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int addEnchantment(object item)
{
    int ret = 0;

    string *enchantments = ({ "acid", "air", "chaos", "cold", "earth", "electricity",
        "energy", "fire", "magical", "poison", "water", "undead", "good", "evil" });

    string enchantType = "enchantments";
    if (item->query("armor type") ||
        (!item->query("weapon type") && random(2)))
    {
        enchantType = "resistances";
    }

    if (!item->query(enchantType))
    {
        string enchantment = enchantments[random(sizeof(enchantments))];
        item->set(enchantType, ([
            enchantment:(1 + random(5))
        ]));
        item->set("name", sprintf("%s of %s", item->query("name"), capitalize(enchantment)));
        item->set("short", sprintf("%s of %s", item->query("short"), capitalize(enchantment)));
        ret = 1;
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask varargs string *getListOfBlueprints(string type, string subtype,
    string *listOfPotentialItems)
{
    string *itemBlueprints = ({});
    if (listOfPotentialItems)
    {
        foreach(string file in listOfPotentialItems)
        {
            itemBlueprints += ({ sprintf("/lib/instances/items/%s%s/%s.c",
                type, (subtype && (subtype != "")) ? "/" + subtype : "",
                file) });
        }
    }
    else
    {
        string directory = sprintf("/lib/instances/items/%s%s/", type,
            ((subtype && (subtype != "")) ? "/" + subtype : ""));

        directory = regreplace(directory, " ", "-", 1);
        directory = regreplace(directory, "'", "", 1);

        itemBlueprints = get_dir(directory, 0x10);
    }
    return itemBlueprints;
}

/////////////////////////////////////////////////////////////////////////////
private nomask varargs object getRandomItemOfType(string type, string subtype,
    string *listOfPotentialItems)
{
    object item = 0;

    string *itemBlueprints = getListOfBlueprints(type, subtype, 
        listOfPotentialItems);

    if (sizeof(itemBlueprints))
    {
        string blueprint = itemBlueprints[random(sizeof(itemBlueprints))];

        if (file_size(blueprint) == -2)
        {
            string *subList = get_dir(blueprint + "/");
            blueprint += "/" + subList[random(sizeof(subList))];
        }

        item = clone_object(blueprint);
    }
    return item;
}

/////////////////////////////////////////////////////////////////////////////
private nomask void addEnchantments(object item, int numEnchantments)
{
    for (int j = 0; j < numEnchantments; j++)
    {
        switch (random(3))
        {
            case 0:
            case 1:
            {
                if (addEnchantment(item))
                {
                    break;
                }
            }
            default:
            {
                string *bonuses = bonusList(item);

                item->set(bonuses[random(sizeof(bonuses))],
                    (1 + random(3)));
            }
        }
        if (!random(4))
        {
            item->set("craftsmanship", getRandomCraftsmanshipBonus(item));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs object generateRandomItem(string type, string subtype, 
    int chanceForMagicalItems, string *listOfPotentialItems)
{
    object item = 0;

    object craftingDictionary = load_object("/lib/dictionaries/craftingDictionary.c");

    item = getRandomItemOfType(type, subtype, listOfPotentialItems);
    if (objectp(item))
    {
        craftingDictionary->getRandomCraftingMaterial(item);

        string *aliases = ({ lower_case(item->query("name")) });
        if (item->query("aliases"))
        {
            aliases += item->query("aliases");
        }
        item->set("aliases", aliases);

        if (random(100) < chanceForMagicalItems)
        {
            addEnchantments(item, 1 + random(5));
        }
    }
    return item;
}
