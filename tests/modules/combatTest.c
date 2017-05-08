//*****************************************************************************
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
inherit "/lib/tests/framework/testFixture.c";
#include "/lib/include/inventory.h"

object Attacker;
object Target;

/////////////////////////////////////////////////////////////////////////////
void SetUpAttacker()
{
    Attacker = clone_object("/lib/tests/support/services/combatWithMockServices");
    Attacker->Name("Bob");
    Attacker->Str(20);
    Attacker->Dex(20);
    Attacker->Con(20);
    Attacker->Int(20);
    Attacker->Wis(20);
}

/////////////////////////////////////////////////////////////////////////////
void SetUpTarget()
{
    Target = clone_object("/lib/realizations/monster");
    Target->Name("Nukulevee");
    Target->Race("undead horse");
    Target->effectiveLevel(20);
    Target->Str(20);
    Target->Dex(20);
    Target->Con(20);
    Target->Int(20);
    Target->Wis(20);
}

/////////////////////////////////////////////////////////////////////////////
object CreateWeapon(string name)
{
    object weapon = clone_object("/lib/items/weapon");
    weapon->set("name", name);
    weapon->set("defense class", 2);
    weapon->set("weapon class", 10);
    weapon->set("bonus hit points", 2);
    weapon->set("material", "galvorn");
    weapon->set("weapon type", "long sword");
    weapon->set("equipment locations", OnehandedWeapon);
    move_object(weapon, Attacker);

    return weapon;
}

/////////////////////////////////////////////////////////////////////////////
object CreateShield(string name)
{
    object shield = clone_object("/lib/items/weapon");
    shield->set("name", name);
    shield->set("defense class", 1);
    shield->set("bonus constitution", 3);
    shield->set("material", "steel");
    shield->set("craftsmanship", 20);
    shield->set("equipment locations", Shield);
    shield->set("weapon type", "shield");
    move_object(shield, Attacker);

    return shield;
}

/////////////////////////////////////////////////////////////////////////////
object CreateArmor(string name)
{
    object armor = clone_object("/lib/items/armor");
    armor->set("name", name);
    armor->set("bonus hit points", 4);
    armor->set("armor class", 5);
    armor->set("armor type", "chainmail");
    armor->set("equipment locations", Armor);
    move_object(armor, Attacker);

    return armor;
}

/////////////////////////////////////////////////////////////////////////////
void Init()
{
    ignoreList += ({ "SetUpAttacker", "SetUpTarget", "CreateWeapon", "CreateArmor",
        "CreateShield" });
}

/////////////////////////////////////////////////////////////////////////////
void Setup()
{
    SetUpAttacker();
    SetUpTarget();
}

/////////////////////////////////////////////////////////////////////////////
void CleanUp()
{
    destruct(Target);
    destruct(Attacker);
}

/////////////////////////////////////////////////////////////////////////////
void MaxHitPointsWithConOf20Returns120()
{
    ExpectEq(120, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void InventoryCorrectlyAffectsMaxHitPoints()
{
    object weapon = CreateWeapon("blah");
    object shield = CreateShield("shield");
    object armor = CreateArmor("stuff");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus hit points", 6);

    ExpectEq(120, Attacker->maxHitPoints(), "hit points still 120 until equipment equipped");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectTrue(shield->equip("shield offhand"), "shield equip called");
    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(150, Attacker->maxHitPoints(), "hit points 132 with equipment equipped");
    ExpectEq(12, Attacker->inventoryGetModifier("combatModifiers", "bonus hit points"));
}

/////////////////////////////////////////////////////////////////////////////
void MaxHitPointsCorrectlyAppliesRacialModifiers()
{
    Attacker->Race("dwarf");
    ExpectEq(142, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void MaxHitPointsCorrectlyAppliesGuildsModifiers()
{
    ExpectEq(120, Attacker->maxHitPoints());
    Attacker->ToggleMockGuilds();
    ExpectEq(125, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void MaxHitPointsCorrectlyAppliesResearchModifiers()
{
    ExpectEq(120, Attacker->maxHitPoints());
    Attacker->ToggleMockResearch();
    ExpectEq(134, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void MaxHitPointsCorrectlyAppliesTraitsModifiers()
{
    ExpectEq(120, Attacker->maxHitPoints());
    Attacker->ToggleMockTrait();

    // This should apply a +6 for a consititution attribute bonus and a +2 for the trait
    ExpectEq(128, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void MaxHitPointsCorrectlyAppliesBiologicalModifiers()
{
    ExpectEq(120, Attacker->maxHitPoints());
    Attacker->ToggleMockBiological();
    ExpectEq(118, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void MaxHitPointsCorrectlyAppliesBackgroundModifiers()
{
    ExpectEq(120, Attacker->maxHitPoints());
    Attacker->ToggleMockBackground();
    ExpectEq(122, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void MaxHitPointsCorrectlyAppliesAllModifiers()
{
    object weapon = CreateWeapon("blah");
    object shield = CreateShield("shield");
    object armor = CreateArmor("stuff");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus hit points", 6);

    ExpectEq(120, Attacker->maxHitPoints(), "hit points still 120 until equipment equipped");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectTrue(shield->equip("shield offhand"), "shield equip called");
    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    Attacker->Race("dwarf");
    Attacker->ToggleMockGuilds();
    Attacker->ToggleMockResearch();
    Attacker->ToggleMockTrait();
    Attacker->ToggleMockBiological();
    Attacker->ToggleMockBackground();
    ExpectEq(199, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void HitPointsIncrementsSetValue()
{
    ExpectEq(0, Attacker->hitPoints());
    ExpectEq(50, Attacker->hitPoints(50));
    ExpectEq(50, Attacker->hitPoints());
    ExpectEq(100, Attacker->hitPoints(50));
    ExpectEq(100, Attacker->hitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void HitPointsCapsAtMaxHitPoints()
{
    Attacker->Race("dwarf");
    Attacker->ToggleMockTrait();

    ExpectEq(0, Attacker->hitPoints());
    ExpectEq(150, Attacker->hitPoints(200));
    ExpectEq(150, Attacker->hitPoints());
    ExpectEq(150, Attacker->maxHitPoints());
}

/////////////////////////////////////////////////////////////////////////////
void HitPointsDoesNotDecreaseValue()
{
    ExpectEq(100, Attacker->hitPoints(100));
    ExpectEq(100, Attacker->hitPoints(-50));
}

/////////////////////////////////////////////////////////////////////////////
void HitPointsFiresOnHitPointsChangedEvent()
{
    ExpectEq(100, Attacker->hitPoints(100));

    object handler = clone_object("/lib/tests/support/events/mockEventSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    ExpectEq(100, Attacker->hitPoints());
    ExpectEq(100, Attacker->hitPoints(-50));

    string err = catch (ExpectEq(Attacker->hitPoints(100), "hit points increased"));
    ExpectEq("*event handler: onHitPointsChanged called", err, "onHitPointsChanged event fired");
}

/////////////////////////////////////////////////////////////////////////////
void MaxSpellPointsCorrectlyAppliesAllModifiers()
{
    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus intelligence", 2);
    modifier->set("bonus spell points", 10);

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(136, Attacker->maxSpellPoints());
    modifier->set("bonus wisdom", 1);
    ExpectEq(139, Attacker->maxSpellPoints());
    Attacker->Race("elf");
    // 25 for elf plus 3 for int
    ExpectEq(167, Attacker->maxSpellPoints());
    Attacker->ToggleMockGuilds();
    ExpectEq(172, Attacker->maxSpellPoints());
    Attacker->ToggleMockResearch();
    ExpectEq(186, Attacker->maxSpellPoints());
    Attacker->ToggleMockTrait();
    ExpectEq(194, Attacker->maxSpellPoints());
    Attacker->ToggleMockBiological();
    ExpectEq(192, Attacker->maxSpellPoints());
    Attacker->ToggleMockBackground();
    ExpectEq(194, Attacker->maxSpellPoints());
}

/////////////////////////////////////////////////////////////////////////////
void MaxStaminaPointsCorrectlyAppliesAllModifiers()
{
    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus strength", 2);
    modifier->set("bonus stamina points", 5);

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(131, Attacker->maxStaminaPoints());
    modifier->set("bonus constitution", 1);
    ExpectEq(134, Attacker->maxStaminaPoints());
    Attacker->Race("dwarf");
    // 25 for dwarf plus 3 for strength plus 6 for con
    ExpectEq(168, Attacker->maxStaminaPoints());
    Attacker->ToggleMockGuilds();
    ExpectEq(173, Attacker->maxStaminaPoints());
    Attacker->ToggleMockResearch();
    ExpectEq(187, Attacker->maxStaminaPoints());
    Attacker->ToggleMockTrait();
    ExpectEq(195, Attacker->maxStaminaPoints());
    Attacker->ToggleMockBiological();
    ExpectEq(193, Attacker->maxStaminaPoints());
    Attacker->ToggleMockBackground();
    ExpectEq(195, Attacker->maxStaminaPoints());
}

/////////////////////////////////////////////////////////////////////////////
void SpellPointsIncrementsSetValue()
{
    ExpectEq(0, Attacker->spellPoints());
    ExpectEq(50, Attacker->spellPoints(50));
    ExpectEq(50, Attacker->spellPoints());
    ExpectEq(100, Attacker->spellPoints(50));
    ExpectEq(100, Attacker->spellPoints());
}

/////////////////////////////////////////////////////////////////////////////
void SpellPointsCapsAtMaxSpellPoints()
{
    Attacker->Race("elf");
    Attacker->ToggleMockTrait();

    ExpectEq(0, Attacker->spellPoints());
    ExpectEq(156, Attacker->spellPoints(200));
    ExpectEq(156, Attacker->spellPoints());
    ExpectEq(156, Attacker->maxSpellPoints());
}

/////////////////////////////////////////////////////////////////////////////
void SpellPointsIncreaseIncludesBonusRecoverSpellPoints()
{
    Attacker->Race("elf");
    Attacker->ToggleMockGuilds();
    Attacker->ToggleMockResearch();
    Attacker->ToggleMockTrait();
    Attacker->ToggleMockBiological();
    Attacker->ToggleMockBackground();

    object armor = CreateArmor("stuff");
    armor->set("bonus recover spell points", 4);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus recover spell points", 1);

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");

    ExpectEq(64, Attacker->spellPoints(50));
}

/////////////////////////////////////////////////////////////////////////////
void SpellPointsDecreaseIncludesBonusReduceSpellPoints()
{
    Attacker->spellPoints(100);

    Attacker->Race("elf");
    Attacker->ToggleMockGuilds();
    Attacker->ToggleMockResearch();
    Attacker->ToggleMockTrait();
    Attacker->ToggleMockBiological();
    Attacker->ToggleMockBackground();

    object armor = CreateArmor("stuff");
    armor->set("bonus reduce spell points", 2);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus reduce spell points", 4);

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");

    ExpectEq(95, Attacker->spellPoints(-20));
}

/////////////////////////////////////////////////////////////////////////////
void SpellPointsFiresOnSpellPointsChangedEvent()
{
    ExpectEq(100, Attacker->spellPoints(100));

    object handler = clone_object("/lib/tests/support/events/mockEventSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    ExpectEq(100, Attacker->spellPoints());

    string err = catch (ExpectEq(Attacker->spellPoints(-50), "spell points increased"));
    ExpectEq("*event handler: onSpellPointsChanged called", err, "onSpellPointsChanged event fired");

    err = catch (ExpectEq(Attacker->spellPoints(100), "spell points increased"));
    ExpectEq("*event handler: onSpellPointsChanged called", err, "onSpellPointsChanged event fired");
}

/////////////////////////////////////////////////////////////////////////////
void StaminaPointsIncrementsSetValue()
{
    ExpectEq(0, Attacker->staminaPoints());
    ExpectEq(50, Attacker->staminaPoints(50));
    ExpectEq(50, Attacker->staminaPoints());
    ExpectEq(100, Attacker->staminaPoints(50));
    ExpectEq(100, Attacker->staminaPoints());
}

/////////////////////////////////////////////////////////////////////////////
void StaminaPointsCapsAtMaxStaminaPoints()
{
    Attacker->Race("dwarf");
    Attacker->ToggleMockTrait();

    ExpectEq(0, Attacker->staminaPoints(), "initial stamina points is zero");
    ExpectEq(162, Attacker->staminaPoints(200), "incrementing by 200 caps at maxStaminaPoints");
    ExpectEq(162, Attacker->staminaPoints(), "querying stamina returns expected result");
    ExpectEq(162, Attacker->maxStaminaPoints(), "max stamina matches");
}

/////////////////////////////////////////////////////////////////////////////
void StaminaPointsIncreaseIncludesBonusRecoverStaminaPoints()
{
    Attacker->Race("elf");
    Attacker->ToggleMockGuilds();
    Attacker->ToggleMockResearch();
    Attacker->ToggleMockTrait();
    Attacker->ToggleMockBiological();
    Attacker->ToggleMockBackground();

    object armor = CreateArmor("stuff");
    armor->set("bonus recover stamina points", 4);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus recover stamina points", 1);

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");

    ExpectEq(64, Attacker->staminaPoints(50));
}

/////////////////////////////////////////////////////////////////////////////
void StaminaPointsDecreaseIncludesBonusReduceStaminaPoints()
{
    Attacker->staminaPoints(100);

    Attacker->Race("elf");
    Attacker->ToggleMockGuilds();
    Attacker->ToggleMockResearch();
    Attacker->ToggleMockTrait();
    Attacker->ToggleMockBiological();
    Attacker->ToggleMockBackground();

    object armor = CreateArmor("stuff");
    armor->set("bonus reduce stamina points", 2);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus reduce stamina points", 4);

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");

    ExpectEq(95, Attacker->staminaPoints(-20));
}

/////////////////////////////////////////////////////////////////////////////
void StaminaPointsFiresOnStaminaPointsChangedEvent()
{
    ExpectEq(100, Attacker->staminaPoints(100));

    object handler = clone_object("/lib/tests/support/events/mockEventSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    ExpectEq(100, Attacker->staminaPoints());

    string err = catch (ExpectEq(Attacker->staminaPoints(-50), "stamina points increased"));
    ExpectEq("*event handler: onStaminaPointsChanged called", err, "onStaminaPointsChanged event fired");

    err = catch (ExpectEq(Attacker->staminaPoints(100), "stamina points increased"));
    ExpectEq("*event handler: onStaminaPointsChanged called", err, "onStaminaPointsChanged event fired");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackUsesCorrectAttributeBonuses()
{
    // 2 for dexterity of 20 and 2 for wisdom of 20
    ExpectEq(4, Attacker->calculateDefendAttack());

    Attacker->ToggleMockResearch();

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus dexterity", 4);

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(9, Attacker->calculateDefendAttack());
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackUsesCorrectPrimaryWeaponData()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);

    ExpectEq(4, Attacker->calculateDefendAttack(), "nothing is equipped");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectEq(-11, Attacker->calculateDefendAttack(), "weapon with dc of 2 is equipped");

    Attacker->advanceSkill("long sword", 1);
    ExpectEq(0, Attacker->calculateDefendAttack(), "weapon with dc of 2 is equipped");

    Attacker->advanceSkill("long sword", 8);
    ExpectEq(5, Attacker->calculateDefendAttack(), "weapon with dc of 2 is equipped and skill of 9");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackUsesCorrectOffhandWeaponData()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);

    ExpectEq(4, Attacker->calculateDefendAttack(), "nothing is equipped");

    ExpectTrue(weapon->equip("blah offhand"), "weapon equip called");
    ExpectEq(-11, Attacker->calculateDefendAttack(), "weapon with dc of 2 is equipped");

    Attacker->advanceSkill("long sword", 1);
    ExpectEq(0, Attacker->calculateDefendAttack(), "weapon with dc of 2 is equipped");

    Attacker->advanceSkill("long sword", 8);
    ExpectEq(5, Attacker->calculateDefendAttack(), "weapon with dc of 2 is equipped and skill of 9");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackUsesCorrectShieldData()
{
    object shield = CreateShield("shield");

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("shield", 1);

    ExpectEq(4, Attacker->calculateDefendAttack(), "nothing is equipped");

    ExpectTrue(shield->equip("shield offhand"), "shield equip called");
    ExpectEq(8, Attacker->calculateDefendAttack(), "shield of dc 1 with skill of 1 equipped");

    Attacker->advanceSkill("shield", 5);
    ExpectEq(14, Attacker->calculateDefendAttack(), "shield skill increased");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackUsesCorrectArmorData()
{
    object armor = CreateArmor("stuff");
    armor->set("material", "galvorn");
    armor->set("craftsmanship", 40);
    armor->set("encumberance", 3);

    Attacker->addSkillPoints(200);

    ExpectEq(4, Attacker->calculateDefendAttack(), "nothing is equipped");

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    // Wearing heavier armor untrained is kinda a bad thing... it'll block, but 
    // you'll get hit.The untrained penalty for chainmail is -20. There is also 
    // an encumberance penalty based on both the item type (-10) and material (-2) 
    // plus the custom-set encumberance field (-3)
    ExpectEq(-31, Attacker->calculateDefendAttack());

    // Now, there is no negative for not knowing "how to wear" the armor, but there's still
    // the other penalties.
    Attacker->advanceSkill("chainmail", 1);
    ExpectEq(-9, Attacker->calculateDefendAttack());

    Attacker->advanceSkill("chainmail", 14);
    ExpectEq(5, Attacker->calculateDefendAttack());
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackUsesCorrectModifierData()
{
    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus defense class", 3);
    modifier->set("bonus dexterity", 4);

    ExpectEq(4, Attacker->calculateDefendAttack(), "nothing is equipped");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(8, Attacker->calculateDefendAttack());
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackCorrectlyAppliesDisease()
{
    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus defense class", 10);

    ExpectEq(4, Attacker->calculateDefendAttack(), "nothing is equipped");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(14, Attacker->calculateDefendAttack(), "before disease is applied");
    modifier->set("disease", 1);
    ExpectEq(12, Attacker->calculateDefendAttack(), "disease applied");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackCorrectlyAppliesMagicalDefendAttackBonus()
{
    ExpectEq(4, Attacker->calculateDefendAttack(), "nothing is equipped");
    Attacker->ToggleMagicalDefendAttackBonus();
    ExpectEq(9, Attacker->calculateDefendAttack(), "magical defend method called");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackUsesCorrectServiceBonuses()
{
    ExpectEq(4, Attacker->calculateDefendAttack(), "base from attributes");
    Attacker->ToggleMockGuilds();
    ExpectEq(9, Attacker->calculateDefendAttack(), "guild modifier is active");
    Attacker->ToggleMockResearch();
    ExpectEq(13, Attacker->calculateDefendAttack(), "research modifier is active");
    Attacker->ToggleMockTrait();
    ExpectEq(15, Attacker->calculateDefendAttack(), "trait modifier is active");
    Attacker->ToggleMockBiological();
    ExpectEq(13, Attacker->calculateDefendAttack(), "biological modifier is active");
    Attacker->ToggleMockBackground();
    ExpectEq(15, Attacker->calculateDefendAttack(), "background modifier is active");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDefendAttackUsesCorrectInventoryData()
{
    object weapon = CreateWeapon("blah");
    object shield = CreateShield("shield");
    object armor = CreateArmor("stuff");
    armor->set("material", "steel");
    armor->set("craftsmanship", 20);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus defense class", 3);
    modifier->set("bonus chainmail", 4);
    modifier->set("bonus dexterity", 6);

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("shield", 1);
    Attacker->advanceSkill("long sword", 1);

    ExpectEq(4, Attacker->calculateDefendAttack(), "nothing is equipped");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectEq(0, Attacker->calculateDefendAttack(), "weapon with dc of 2 is equipped");

    Attacker->advanceSkill("long sword", 8);
    ExpectEq(5, Attacker->calculateDefendAttack(), "weapon with dc of 2 is equipped and skill of 9");

    ExpectTrue(shield->equip("shield offhand"), "shield equip called");
    ExpectEq(11, Attacker->calculateDefendAttack(), "shield of dc 1 with skill of 1 equipped");
    Attacker->advanceSkill("shield", 5);
    ExpectEq(17, Attacker->calculateDefendAttack(), "shield skill increased");

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(-9, Attacker->calculateDefendAttack(), "armor equipped");

    Attacker->advanceSkill("chainmail", 1);
    ExpectEq(13, Attacker->calculateDefendAttack(), "chainmail skill increased to 1");
    Attacker->advanceSkill("chainmail", 10);
    ExpectEq(23, Attacker->calculateDefendAttack(), "chainmail skill increased by 10");

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(30, Attacker->calculateDefendAttack(), "modifier object added");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackWithWeaponCorrectlyAppliesWeaponData()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);

    ExpectEq(0, Attacker->calculateAttack(Target, weapon, 1), "initial attack");
    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    // This includes: -4 for defender's defend attack, 2 dexterity bonus, 
    // 2 intelligence bonus, -20 for attacking untrained with longsword, 
    // -7 weapon encumberance (materials (-2) plus lack of skill (-5)),
    // and 5 for galvorn's attack bonus
    ExpectEq(-22, Attacker->calculateAttack(Target, weapon, 1), "untrained attack");
    
    Attacker->advanceSkill("long sword", 1);
    ExpectEq(0, Attacker->calculateAttack(Target, weapon, 1), "trained attack");

    Attacker->advanceSkill("long sword", 10);
    ExpectEq(10, Attacker->calculateAttack(Target, weapon, 1), "trained to 10 attack");

    weapon->set("bonus attack", 3);
    ExpectEq(13, Attacker->calculateAttack(Target, weapon, 1), "bonus attack applied");

    weapon->set("bonus dexterity", 2);
    ExpectEq(14, Attacker->calculateAttack(Target, weapon, 1), "bonus dexterity applied");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackWithDualWieldWeaponCorrectlyAppliesWeaponData()
{
    object weapon = CreateWeapon("blah");
    object offhand = CreateWeapon("blarg");

    Attacker->addSkillPoints(100);

    ExpectEq(0, Attacker->calculateAttack(Target, weapon, 1), "initial attack");
    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectTrue(offhand->equip("blarg offhand"), "offhand weapon equip called");

    // This includes: -4 for defender's defend attack, 2 dexterity bonus, 
    // 2 intelligence bonus, -20 for attacking untrained with longsword, 
    // -14 weapon encumberance (materials (-4) plus regular (-10)),
    // 5 for galvorn's attack bonus, -5 for untrained dual wield, and -5 for
    // base primary weapon dual wield penalty, 4 for attribute bonus for skill
    ExpectEq(-49, Attacker->calculateAttack(Target, weapon, 1), "untrained attack");
    ExpectEq(-54, Attacker->calculateAttack(Target, offhand, 1), "untrained offhand attack");

    Attacker->advanceSkill("dual wield", 1);
    ExpectEq(-43, Attacker->calculateAttack(Target, weapon, 1), "dual wield attack");
    ExpectEq(-48, Attacker->calculateAttack(Target, offhand, 1), "dual wield offhand attack");

    Attacker->advanceSkill("dual wield", 9);
    ExpectEq(-38, Attacker->calculateAttack(Target, weapon, 1), "dual wield attack");
    ExpectEq(-43, Attacker->calculateAttack(Target, offhand, 1), "dual wield offhand attack");

    Attacker->advanceSkill("long sword", 10);
    ExpectEq(10, Attacker->calculateAttack(Target, weapon, 1), "trained to 10 attack");
    ExpectEq(5, Attacker->calculateAttack(Target, offhand, 1), "trained to 10 attack - offhand");

    weapon->set("bonus attack", 3);
    ExpectEq(13, Attacker->calculateAttack(Target, weapon, 1), "bonus attack applied");
    ExpectEq(5, Attacker->calculateAttack(Target, offhand, 1), "bonus has no effect on offhand");

    weapon->set("bonus dexterity", 2);
    ExpectEq(14, Attacker->calculateAttack(Target, weapon, 1), "bonus dexterity applied");
    ExpectEq(6, Attacker->calculateAttack(Target, offhand, 1), "bonus dexterity applied offhand");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackWhileWearingArmorAppliesArmorModifiers()
{
    object weapon = CreateWeapon("blah");

    object armor = CreateArmor("stuff");
    armor->set("material", "steel");
    armor->set("craftsmanship", 20);

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 12);

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectEq(12, Attacker->calculateAttack(Target, weapon, 1), "weapon equipped");

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(-7, Attacker->calculateAttack(Target, weapon, 1), "armor equipped");

    Attacker->advanceSkill("chainmail", 1);
    ExpectEq(4, Attacker->calculateAttack(Target, weapon, 1), "armor equipped");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackUsesCorrectInventoryData()
{
    object weapon = CreateWeapon("blah");
    weapon->set("craftsmanship", 35);
    weapon->set("material", "iron");

    object shield = CreateShield("shield");

    object armor = CreateArmor("stuff");
    armor->set("material", "steel");
    armor->set("craftsmanship", 20);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus attack", 5);
    modifier->set("bonus dexterity", 2);
    modifier->set("bonus intelligence", 2);

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("shield", 1);
    Attacker->advanceSkill("long sword", 1);

    ExpectEq(0, Attacker->calculateAttack(Target, weapon, 1), "nothing is equipped");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    // The sword is "masterwork" because the skill greatly exceeded that required.
    // This accounts for the bonus as the attack would otherwise be -5.
    ExpectEq(7, Attacker->calculateAttack(Target, weapon, 1), "weapon is equipped");

    Attacker->advanceSkill("long sword", 10);
    ExpectEq(12, Attacker->calculateAttack(Target, weapon, 1), "weapon is equipped and skill of 11");

    ExpectTrue(shield->equip("shield offhand"), "shield equip called");
    ExpectEq(12, Attacker->calculateAttack(Target, weapon, 1), "shield with skill of 1 equipped");
    Attacker->advanceSkill("shield", 5);
    ExpectEq(12, Attacker->calculateAttack(Target, weapon, 1), "shield skill increased");

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(5, Attacker->calculateAttack(Target, weapon, 1), "armor equipped");

    Attacker->advanceSkill("chainmail", 1);
    ExpectEq(12, Attacker->calculateAttack(Target, weapon, 1), "chainmail skill increased to 1");
    Attacker->advanceSkill("chainmail", 10);
    ExpectEq(12, Attacker->calculateAttack(Target, weapon, 1), "chainmail skill increased by 10");

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(19, Attacker->calculateAttack(Target, weapon, 1), "modifier object added");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackUsesCorrectServiceBonuses()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 12);
    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    ExpectEq(12, Attacker->calculateAttack(Target, weapon, 1), "weapon is equipped");

    Attacker->ToggleMockGuilds();
    ExpectEq(19, Attacker->calculateAttack(Target, weapon, 1), "guild modifier is active");
    Attacker->ToggleMockResearch();
    ExpectEq(24, Attacker->calculateAttack(Target, weapon, 1), "research modifier is active");
    Attacker->ToggleMockTrait();
    ExpectEq(27, Attacker->calculateAttack(Target, weapon, 1), "trait modifier is active");
    Attacker->ToggleMockBiological();
    ExpectEq(24, Attacker->calculateAttack(Target, weapon, 1), "biological modifier is active");
    Attacker->ToggleMockBackground();
    ExpectEq(27, Attacker->calculateAttack(Target, weapon, 1), "background modifier is active");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackDecreasesAttackForDisease()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 12);
    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    ExpectEq(12, Attacker->calculateAttack(Target, weapon, 1), "weapon is equipped");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("disease", 1);

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(11, Attacker->calculateAttack(Target, weapon, 1), "disease object added");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackDecreasesAttackForEnfeebled()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 18);
    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    ExpectEq(15, Attacker->calculateAttack(Target, weapon, 1), "weapon is equipped");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("enfeebled", 1);

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(13, Attacker->calculateAttack(Target, weapon, 1), "enfeebled object added");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackIncreasesAttackForFortified()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 18);
    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    ExpectEq(15, Attacker->calculateAttack(Target, weapon, 1), "weapon is equipped");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("fortified", 1);

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(16, Attacker->calculateAttack(Target, weapon, 1), "fortified object added");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackCorrectlyAppliesMagicalAttackBonus()
{
    object weapon = CreateWeapon("blah");
    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 10);
    
    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    ExpectEq(10, Attacker->calculateAttack(Target, weapon, 1), "nothing is equipped");
    Attacker->ToggleMagicalAttackBonus();
    ExpectEq(14, Attacker->calculateAttack(Target, weapon, 1), "magical attack method called");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackCorrectlyAppliesAttackTypes()
{
    object weapon = clone_object("/lib/modules/combat/attacks/clawAttack.c");
    weapon->setAttackValues(10, 10);
    ExpectEq(10, Attacker->calculateAttack(Target, weapon, 1));
}

/////////////////////////////////////////////////////////////////////////////
void CalculateAttackCanBeRandomized()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 18);
    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    
    // There is a very small chance that this test will fail due to identical
    // random numbers, so this calls up to 5 times to almost remove that risk
    int firstEval = Attacker->calculateAttack(Target, weapon);
    int compareEval = firstEval;
    for(int i = 0; (i < 5) && (firstEval == compareEval); i++)
    {
        compareEval = Attacker->calculateAttack(Target, weapon);
    }
    ExpectTrue(firstEval != compareEval);
}

/////////////////////////////////////////////////////////////////////////////
void CalculateSoakDamageCorrectlyAppliesMagicalDefenseBonus()
{
    // This includes 2 from con and 2 from str
    ExpectEq(4, Attacker->calculateSoakDamage("physical"), "nothing is equipped");
    Attacker->ToggleMagicalDefenseBonus();
    ExpectEq(6, Attacker->calculateSoakDamage("physical"), "magical defend method called");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateSoakDamageCorrectlyAppliesWhenWearingArmor()
{
    object armor = CreateArmor("stuff");
    armor->set("material", "steel");

    object gloves = clone_object("/lib/items/armor");
    gloves->set("name", "gloves");
    gloves->set("armor class", 1);
    gloves->set("material", "steel");
    gloves->set("armor type", "chainmail");
    gloves->set("equipment locations", Gloves);
    move_object(gloves, Attacker);

    Attacker->addSkillPoints(100);
    ExpectEq(4, Attacker->calculateSoakDamage("physical"), "armor not equipped");

    // +5 for AC and +1 for being made out of steel
    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(10, Attacker->calculateSoakDamage("physical"), "armor equipped");

    armor->set("craftsmanship", 20);
    ExpectEq(11, Attacker->calculateSoakDamage("physical"), "craftsmanship bonus");

    ExpectTrue(gloves->equip("gloves"), "glove equip called");
    ExpectEq(12, Attacker->calculateSoakDamage("physical"), "gloves equipped");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateSoakDamageCorrectlyAppliesServiceBonuses()
{
    ExpectEq(4, Attacker->calculateSoakDamage("physical"), "initial case");

    Attacker->ToggleMockGuilds();
    ExpectEq(9, Attacker->calculateSoakDamage("physical"), "guild modifier is active");
    Attacker->ToggleMockResearch();
    ExpectEq(13, Attacker->calculateSoakDamage("physical"), "research modifier is active");
    Attacker->ToggleMockTrait();
    ExpectEq(15, Attacker->calculateSoakDamage("physical"), "trait modifier is active");
    Attacker->ToggleMockBiological();
    ExpectEq(13, Attacker->calculateSoakDamage("physical"), "biological modifier is active");
    Attacker->ToggleMockBackground();
    ExpectEq(15, Attacker->calculateSoakDamage("physical"), "background modifier is active");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateSoakDamageCorrectlyAppliesForInventory()
{
    object weapon = CreateWeapon("blah");
    weapon->set("bonus defense", 3);

    object armor = CreateArmor("stuff");
    armor->set("material", "steel");

    object gloves = clone_object("/lib/items/armor");
    gloves->set("name", "gloves");
    gloves->set("armor class", 1);
    gloves->set("material", "steel");
    gloves->set("armor type", "chainmail");
    gloves->set("equipment locations", Gloves);
    move_object(gloves, Attacker);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus defense", 2);
    modifier->set("bonus constitution", 2);
    modifier->set("bonus strength", 2);

    Attacker->addSkillPoints(100);
    ExpectEq(4, Attacker->calculateSoakDamage("physical"), "armor not equipped");

    // +5 for AC and +1 for being made out of steel
    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(10, Attacker->calculateSoakDamage("physical"), "armor equipped");

    armor->set("craftsmanship", 20);
    ExpectEq(11, Attacker->calculateSoakDamage("physical"), "craftsmanship bonus");

    ExpectTrue(gloves->equip("gloves"), "glove equip called");
    ExpectEq(12, Attacker->calculateSoakDamage("physical"), "gloves equipped");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectEq(15, Attacker->calculateSoakDamage("physical"), "weapon equipped");

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(19, Attacker->calculateSoakDamage("physical"), "modifier registered");
}

/////////////////////////////////////////////////////////////////////////////
void ParalysisAffectsSoakDamage()
{
    object armor = CreateArmor("stuff");
    armor->set("material", "steel");
    armor->set("armor class", 10);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("paralysis", 1);

    ExpectTrue(armor->equip("stuff"), "armor equip called");
    ExpectEq(15, Attacker->calculateSoakDamage("physical"), "armor equipped");

    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(13, Attacker->calculateSoakDamage("physical"), "modifier registered");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDamageCorrectlyAppliesAttackTypes()
{
    object weapon = clone_object("/lib/modules/combat/attacks/clawAttack.c");
    weapon->setAttackValues(10, 10);
    ExpectEq(14, Attacker->calculateDamage(weapon, weapon->getDamageType(), 1));
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDamageCorrectlyAppliesMagicalDamageBonus()
{
    object weapon = CreateWeapon("blah");

    // 2 from str, 1 from int, and 1 from wis
    ExpectEq(4, Attacker->calculateDamage(weapon, "physical", 1), "nothing is equipped");
    Attacker->ToggleMagicalDamageBonus();
    ExpectEq(7, Attacker->calculateDamage(weapon, "physical", 1), "magical damage method called");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDamageReturnsTheCorrectDamageForWeapons()
{
    object weapon = CreateWeapon("blah");

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 1);

    // 2 from str, 1 from int, and 1 from wis
    ExpectEq(4, Attacker->calculateDamage(weapon, "physical", 1), "No weapon equipped");
    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    // wc is 10 and the bonus from galvorn is 8.
    ExpectEq(23, Attacker->calculateDamage(weapon, "physical", 1, "physical damage"));

    Attacker->advanceSkill("long sword", 11);
    ExpectEq(29, Attacker->calculateDamage(weapon, "physical", 1, "physical damage after skill increased"));

    ExpectEq(7, Attacker->calculateDamage(weapon, "magical", 1, "Only attribute and magical damage from galvorn are counted"));
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDamageReturnsTheCorrectDamageForInventory()
{
    object weapon = CreateWeapon("blah");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus damage", 5);
    modifier->set("enchantments", (["magical":3, "fire" : 10, "acid" : 10]));

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 8);

    // 2 from str, 1 from int, and 1 from wis
    ExpectEq(4, Attacker->calculateDamage(weapon, "physical", 1), "No weapon equipped");
    ExpectEq(0, Attacker->calculateDamage(weapon, "magical", 1), "magical weapon equipped");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");

    ExpectEq(32, Attacker->calculateDamage(weapon, "physical", 1), "total physical damage");
    ExpectEq(10, Attacker->calculateDamage(weapon, "magical", 1), "total magical damage");
    ExpectEq(12, Attacker->calculateDamage(weapon, "fire", 1), "total fire damage");
    ExpectEq(12, Attacker->calculateDamage(weapon, "acid", 1), "total acid damage");
    ExpectEq(0, Attacker->calculateDamage(weapon, "cold", 1), "total cold damage");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDamageCorrectlyEnfeebles()
{
    object weapon = CreateWeapon("blah");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("enfeebled", 5);

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 8);

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectEq(27, Attacker->calculateDamage(weapon, "physical", 1), "weapon equipped");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(21, Attacker->calculateDamage(weapon, "physical", 1), "total physical damage");
}

/////////////////////////////////////////////////////////////////////////////
void CalculateDamageCorrectlyFortifies()
{
    object weapon = CreateWeapon("blah");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("fortified", 5);

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 8);

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectEq(27, Attacker->calculateDamage(weapon, "physical", 1), "weapon equipped");
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");
    ExpectEq(32, Attacker->calculateDamage(weapon, "physical", 1), "total physical damage");
}

/////////////////////////////////////////////////////////////////////////////
void AddAttackDoesNotAddInvalidAttacks()
{
    // With no attacks set, one unarmed attack is created
    ExpectEq(1, sizeof(Attacker->getAttacks()), "1 attack is in the list");
    ExpectFalse(Attacker->addAttack("blah", 10, 10), "blah attack is not added");
    ExpectEq(1, sizeof(Attacker->getAttacks()), "1 attack is in the list");
    ExpectEq("unarmed", Attacker->getAttacks()[0]["attack type"], "unarmed attack exists");
}

/////////////////////////////////////////////////////////////////////////////
void AddAttackAddsValidAttacks()
{
    ExpectTrue(Attacker->addAttack("claw", 10, 10), "claw attack added");
    ExpectEq(1, sizeof(Attacker->getAttacks()), "1 attack is in the list");
    ExpectEq("claw", Attacker->getAttacks()[0]["attack type"], "claw attack was added");
}

/////////////////////////////////////////////////////////////////////////////
void AddAttackAddsMultipleAttacks()
{
    // With no attacks set, one unarmed attack is created
    ExpectTrue(Attacker->addAttack("claw", 10, 10), "claw attack added");
    ExpectTrue(Attacker->addAttack("horn", 10, 10), "horn attack added");
    ExpectTrue(Attacker->addAttack("teeth", 10, 10), "teeth attack added");
    ExpectTrue(Attacker->addAttack("fire", 10, 10), "fire attack added");
    ExpectTrue(Attacker->addAttack("cold", 10, 10), "cold attack added");
    ExpectFalse(Attacker->addAttack("fake", 10, 10), "fake attack added");
    ExpectEq(5, sizeof(Attacker->getAttacks()), "5 attacks were added");
}

/////////////////////////////////////////////////////////////////////////////
void ClearAttacksClearsAllAttacks()
{
    ExpectTrue(Attacker->addAttack("claw", 10, 10), "claw attack added");
    ExpectTrue(Attacker->addAttack("horn", 10, 10), "horn attack added");
    ExpectEq(2, sizeof(Attacker->getAttacks()), "1 attack is in the list");
    ExpectEq("claw", Attacker->getAttacks()[0]["attack type"], "claw attack was added");
    Attacker->clearAttacks();
    ExpectEq(1, sizeof(Attacker->getAttacks()), "only 1 attack in the list");
    ExpectEq("unarmed", Attacker->getAttacks()[0]["attack type"], "unarmed attack in list");
}

/////////////////////////////////////////////////////////////////////////////
void GetAttacksReturnsWeaponAttack()
{
    object weapon = CreateWeapon("blah");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    ExpectEq(1, sizeof(Attacker->getAttacks()), "1 attack returned");
    ExpectEq("weapon", Attacker->getAttacks()[0]["attack type"], "weapon attack in list");
}

/////////////////////////////////////////////////////////////////////////////
void GetAttacksDoesNotReturnWieldedNonAttackShields()
{
    object weapon = CreateWeapon("blah");
    object shield = CreateShield("shield");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectTrue(shield->equip("shield offhand"), "shield equip called");

    ExpectEq(1, sizeof(Attacker->getAttacks()), "1 attack returned");
    ExpectEq("weapon", Attacker->getAttacks()[0]["attack type"], "weapon attack in list");
}

/////////////////////////////////////////////////////////////////////////////
void GetAttacksReturnsWieldedShieldsWithAttacks()
{
    object weapon = CreateWeapon("blah");
    object shield = CreateShield("shield");
    shield->set("weapon class", 1);

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectTrue(shield->equip("shield offhand"), "shield equip called");

    ExpectEq(2, sizeof(Attacker->getAttacks()), "2 attack returned");
    ExpectEq("weapon", Attacker->getAttacks()[0]["attack type"], "weapon attack in list");
    ExpectEq("offhand weapon", Attacker->getAttacks()[1]["attack type"], "weapon attack in list");
}

/////////////////////////////////////////////////////////////////////////////
void GetAttacksReturnDualWieldWeaponsWithAttacks()
{
    object weapon = CreateWeapon("blah");
    object weapon2 = CreateWeapon("stuff");

    ExpectTrue(weapon->equip("blah"), "weapon equip called");
    ExpectTrue(weapon2->equip("stuff offhand"), "second weapon equip called");

    ExpectEq(2, sizeof(Attacker->getAttacks()), "2 attacks returned");
    ExpectEq("weapon", Attacker->getAttacks()[0]["attack type"], "weapon attack in list");
    ExpectEq("offhand weapon", Attacker->getAttacks()[1]["attack type"], "weapon attack in list");
}

/////////////////////////////////////////////////////////////////////////////
void GetAttacksReturnsExtraAttacksFromServices()
{
    object weapon = CreateWeapon("blah");
    ExpectTrue(weapon->equip("blah"), "weapon equip called");

    Attacker->ToggleMockGuilds();
    Attacker->ToggleMockResearch();
    Attacker->ToggleMockTrait();
    Attacker->ToggleMockBiological();
    Attacker->ToggleMockBackground();
    ExpectEq(5, sizeof(Attacker->getAttacks()), "5 attacks returned");
}

/////////////////////////////////////////////////////////////////////////////
void PlayersCanToggleKillList()
{
    destruct(Attacker);
    Attacker = clone_object("/lib/realizations/player");

    ExpectFalse(Attacker->onKillList(), "initial value");
    Attacker->toggleKillList();
    ExpectTrue(Attacker->onKillList(), "first time toggled");
    Attacker->toggleKillList();
    ExpectFalse(Attacker->onKillList(), "second time toggled");
}

/////////////////////////////////////////////////////////////////////////////
void CombatStatisticsAreGenerated()
{
    ExpectEq(0, sizeof(Attacker->CombatStatistics()));
    Attacker->generateCombatStatistics(Target);

    // A key specific to the target is generated as well as one for "best kill",
    // "race", and "nemesis"
    mapping stats = Attacker->CombatStatistics();
    ExpectEq(4, sizeof(stats));

    string targetKey = program_name(Target) + "#" + Target->Name();
    ExpectTrue(member(stats, targetKey), "target exists in combat stats");
    ExpectEq("Nukulevee", stats[targetKey]["name"], "The target's name is 'Nukulevee'");
    ExpectEq(20, stats[targetKey]["level"], "The target's level is 20");
    ExpectEq(1, stats[targetKey]["times killed"], "The target's been killed once");

    ExpectTrue(member(stats, "best kill"));
    ExpectEq("Nukulevee", stats["best kill"]["name"], "The best kill's name is 'Nukulevee'");
    ExpectEq(20, stats["best kill"]["level"], "The best kill's level is 20");
    ExpectEq(targetKey, stats["best kill"]["key"], "The best kill's key is 'target'");

    ExpectTrue(member(stats, "race"));
    ExpectEq(1, stats["race"]["undead horse"], "The race was killed once");

    ExpectTrue(member(stats, "nemesis"));
    ExpectEq("Nukulevee", stats["nemesis"]["name"], "The nemesis's name is 'Nukulevee'");
    ExpectEq(20, stats["nemesis"]["level"], "The nemesis's level is 20");
    ExpectEq(1, stats["nemesis"]["times killed"], "The nemesis's been killed once");
    ExpectEq(targetKey, stats["nemesis"]["key"], "The nemesis's key is 'target'");
}

/////////////////////////////////////////////////////////////////////////////
void CombatStatisticsNemesisIsMostFrequestKill()
{
    ExpectEq(0, sizeof(Attacker->CombatStatistics()));

    object foe = clone_object("/lib/tests/support/services/combatWithMockServices");
    foe->Name("Earl");
    foe->Race("orc");
    foe->effectiveLevel(25);

    Attacker->generateCombatStatistics(foe);
    Attacker->generateCombatStatistics(Target);
    Attacker->generateCombatStatistics(Target);

    mapping stats = Attacker->CombatStatistics();
    ExpectEq(5, sizeof(stats));

    ExpectTrue(member(stats, "nemesis"));
    ExpectEq("Nukulevee", stats["nemesis"]["name"], "The nemesis's name is 'Nukulevee'");
    ExpectEq(20, stats["nemesis"]["level"], "The nemesis's level is 20");
    ExpectEq(2, stats["nemesis"]["times killed"], "The nemesis's been killed once");
    ExpectEq(program_name(Target) + "#" + Target->Name(), stats["nemesis"]["key"], "The nemesis's key is 'target'");
}

/////////////////////////////////////////////////////////////////////////////
void CombatStatisticsRaceUpdatesCorrectly()
{
    ExpectEq(0, sizeof(Attacker->CombatStatistics()));

    object foe = clone_object("/lib/realizations/monster");
    foe->Name("Earl");
    foe->Race("orc");
    foe->effectiveLevel(25);

    Attacker->generateCombatStatistics(foe);
    Attacker->generateCombatStatistics(Target);
    Attacker->generateCombatStatistics(Target);

    mapping stats = Attacker->CombatStatistics();
    ExpectEq(5, sizeof(stats));

    ExpectTrue(member(stats, "race"));
    ExpectEq(1, stats["race"]["orc"], "One orc has been killed");
    ExpectEq(2, stats["race"]["undead horse"], "Two undead horses have been killed");
}

/////////////////////////////////////////////////////////////////////////////
void CombatStatisticsBestKillUpdatesCorrectly()
{
    ExpectEq(0, sizeof(Attacker->CombatStatistics()));

    object foe = clone_object("/lib/realizations/monster");
    foe->Name("Earl");
    foe->Race("orc");
    foe->effectiveLevel(25);

    Attacker->generateCombatStatistics(Target);
    Attacker->generateCombatStatistics(foe);
    Attacker->generateCombatStatistics(Target);

    mapping stats = Attacker->CombatStatistics();

    ExpectTrue(member(stats, "best kill"));
    ExpectEq("Earl", stats["best kill"]["name"], "The best kill's name is 'Earl'");
    ExpectEq(25, stats["best kill"]["level"], "The best kill's level is 25");
    ExpectEq(program_name(foe) + "#" + foe->Name(), stats["best kill"]["key"], "The best kill's key is 'foe'");
}

/////////////////////////////////////////////////////////////////////////////
void AttackFiresOnAttackEvent()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);
    move_object(Target, room);

    object handler = clone_object("/lib/tests/support/events/mockEventSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    string err = catch (ExpectTrue(Attacker->attack(Target), "target attacked"));
    ExpectEq("*event handler: onAttack called", err, "onAttack event fired");
}

/////////////////////////////////////////////////////////////////////////////
void AttackFiresOnAttackedEvent()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);
    move_object(Target, room);

    object handler = clone_object("/lib/tests/support/events/mockEventSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    string err = catch (ExpectTrue(Target->attack(Attacker), "target attacks attacker"));
    ExpectEq("*event handler: onAttacked called", err, "onAttacked event fired");
}

/////////////////////////////////////////////////////////////////////////////
void AttackInvolvingDeathCreatesCorpseAndDestroysTarget()
{
    object room = clone_object("/lib/environment/room");

    object weapon = CreateWeapon("sword");
    Attacker->hitPoints(Attacker->maxHitPoints());
    weapon->equip("sword");
    move_object(Attacker, room);

    Target->hitPoints(1);
    move_object(Target, room);

    Attacker->addSkillPoints(100);
    Attacker->advanceSkill("long sword", 15);

    object *roomItems = all_inventory(room);
    ExpectEq(2, sizeof(roomItems), "two object are in the room");
    ExpectTrue(member(roomItems, Attacker) > -1, "attacker is in room");
    ExpectTrue(member(roomItems, Target) > -1, "target is in room");

    ExpectTrue(Attacker->attack(Target));

    roomItems = all_inventory(room);
    ExpectEq(2, sizeof(roomItems), "two object are in the room");
    ExpectTrue(member(roomItems, Attacker) > -1, "attacker is in room");
    ExpectFalse(member(roomItems, Target) > -1, "target is in room");
    ExpectEq("obj/corpse.c", program_name(roomItems[0]));
}

/////////////////////////////////////////////////////////////////////////////
void AttackInvolvingDeathOfPlayerCreatesCorpseButDoesNotDestroyPlayer()
{
    object room = clone_object("/lib/environment/room");

    object weapon = CreateWeapon("sword");
    Attacker->hitPoints(1);
    weapon->equip("sword");
    move_object(Attacker, room);

    Target->hitPoints(Target->maxHitPoints());
    move_object(Target, room);

    object *roomItems = all_inventory(room);
    ExpectEq(2, sizeof(roomItems), "two objects are in the room");
    ExpectTrue(member(roomItems, Attacker) > -1, "attacker is in room");
    ExpectTrue(member(roomItems, Target) > -1, "target is in room");
    ExpectFalse(Attacker->isDead(), "attacker is not dead");

    ExpectTrue(Target->attack(Attacker));

    roomItems = all_inventory(room);
    ExpectEq(3, sizeof(roomItems), "three objects are in the room");
    ExpectTrue(member(roomItems, Attacker) > -1, "attacker is in room");
    ExpectTrue(member(roomItems, Target) > -1, "target is in room");
    ExpectTrue(Attacker->isDead(), "attacker is now dead");
    ExpectEq("obj/corpse.c", program_name(roomItems[0]));
}

/////////////////////////////////////////////////////////////////////////////
void OnHitFiresWhenLegalHitIsDone()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);

    object handler = clone_object("/lib/tests/support/events/mockEventSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    string err = catch (ExpectTrue(Attacker->hit(5, "physical"), "attacker hit is called"));
    ExpectEq("*event handler: onHit called, data: physical 5, caller: lib/tests/support/services/combatWithMockServices.c", 
        err, "onHit event fired");
}

/////////////////////////////////////////////////////////////////////////////
void HitAddsCorrectExperience()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);
    move_object(Target, room);
    Target->hitPoints(Target->maxHitPoints());

    Target->hit(100, "physical", Attacker);
    ExpectEq(3375, Attacker->experience());
}

/////////////////////////////////////////////////////////////////////////////
void OnDeathFiresWhenKillingBlowLands()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);

    object handler = clone_object("/lib/tests/support/events/onDeathSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    string err = catch (ExpectTrue(Attacker->hit(500, "physical"), "attacker hit is called"));
    ExpectEq("*event handler: onDeath called: lib/tests/support/services/combatWithMockServices.c",
        err, "onDeath event fired");
}

/////////////////////////////////////////////////////////////////////////////
void SettingWimpyWorksCorrectly()
{
    ExpectEq(0, Attacker->Wimpy(), "Wimpy is initially 0");
    ExpectEq(70, Attacker->Wimpy("70"), "A wimpy of 70 can be set");
    ExpectEq(50, Attacker->Wimpy("50"), "A wimpy of 50 can be set");
    ExpectEq(0, Attacker->Wimpy("0"), "A wimpy of 0 can be set");
    ExpectEq(70, Attacker->Wimpy("80"), "A wimpy of 80 caps at 70%");
    ExpectEq(0, Attacker->Wimpy("-10"), "A wimpy of -10 floors at 0%");
}

/////////////////////////////////////////////////////////////////////////////
void WimpyIsNotTriggeredWhenHitPointsAboveThreshhold()
{
    object room = clone_object("/lib/environment/room");
    ExpectEq(50, Attacker->Wimpy("50"), "A wimpy of 50 can be set");
    move_object(Attacker, room);
    move_object(Target, room);

    Attacker->hitPoints(100);
    Target->hitPoints(Target->maxHitPoints());
    Attacker->registerAttacker(Target);

    object handler = clone_object("/lib/tests/support/events/onRunAwaySubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");
    
    string err = catch(Attacker->heart_beat());
    ExpectEq(0, err, "onRunAway event fired");
}

/////////////////////////////////////////////////////////////////////////////
void WimpyIsTriggeredWhenHitPointsBelowThreshhold()
{
    object room = clone_object("/lib/environment/room");
    ExpectEq(50, Attacker->Wimpy("50"), "A wimpy of 50 can be set");
    move_object(Attacker, room);
    move_object(Target, room);

    Attacker->hitPoints(50);
    Target->hitPoints(Target->maxHitPoints());
    Attacker->registerAttacker(Target);

    object handler = clone_object("/lib/tests/support/events/onRunAwaySubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    string err = catch (Attacker->heart_beat());
    ExpectEq("*event handler: onRunAway called: lib/tests/support/services/combatWithMockServices.c",
        err, "onRunAway event fired");
}

/////////////////////////////////////////////////////////////////////////////
void AttackerAttacksDuringHeartBeat()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);
    move_object(Target, room);

    Attacker->hitPoints(Attacker->maxHitPoints());
    Target->hitPoints(Target->maxHitPoints());

    Attacker->attack(Target);

    object handler = clone_object("/lib/tests/support/events/onAttackSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered for attacker");

    ExpectEq(0, handler->TimesOnAttackReceived(), "before heart_beat, no onAttack events fired");
    Attacker->heart_beat();
    ExpectEq(1, handler->TimesOnAttackReceived(), "after heart_beat, one onAttack event fired");
}

/////////////////////////////////////////////////////////////////////////////
void TargetAttackedDuringHeartBeat()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);
    move_object(Target, room);

    Attacker->hitPoints(Attacker->maxHitPoints());
    Target->hitPoints(Target->maxHitPoints());

    Attacker->attack(Target);

    object handler = clone_object("/lib/tests/support/events/onAttackedSubscriber");
    ExpectTrue(Target->registerEvent(handler), "event handler registered for attacker");

    string err = catch (Attacker->heart_beat());
    ExpectEq("*event handler: onAttacked called: lib/realizations/monster.c",
        err, "onAttacked event fired");
}

/////////////////////////////////////////////////////////////////////////////
void HeartBeatHealsHitPointsWhenBelowMaxEveryFifthTime()
{
    Attacker->hitPoints(20);
    Attacker->spellPoints(Attacker->maxSpellPoints());
    Attacker->staminaPoints(Attacker->maxStaminaPoints());

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "2 hit points healed");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus heal hit points", 2);
    modifier->set("registration list", ({ Attacker }));

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "0 hit points healed");

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "0 hit points healed");

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "0 hit points healed");

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "0 hit points healed");

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "0 hit points healed");

    Attacker->heart_beat();
    ExpectEq(26, Attacker->hitPoints(), "4 hit points healed");
}

/////////////////////////////////////////////////////////////////////////////
void HeartBeatHealsAtDifferentRatesWhenBonusApplied()
{
    Attacker->hitPoints(20);
    Attacker->spellPoints(Attacker->maxSpellPoints());
    Attacker->staminaPoints(Attacker->maxStaminaPoints());

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus heal hit points rate", 10);
    modifier->set("registration list", ({ Attacker }));

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "2 hit points healed");

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "0 hit points healed");

    Attacker->heart_beat();
    ExpectEq(22, Attacker->hitPoints(), "0 hit points healed");

    Attacker->heart_beat();
    ExpectEq(24, Attacker->hitPoints(), "2 hit points healed");

    Attacker->heart_beat();
    ExpectEq(24, Attacker->hitPoints(), "0 hit points healed");
}

/////////////////////////////////////////////////////////////////////////////
void HeartBeatHealsSpellPointsWhenBelowMaxEveryFifthTime()
{
    Attacker->hitPoints(Attacker->maxHitPoints());
    Attacker->spellPoints(20);
    Attacker->staminaPoints(Attacker->maxStaminaPoints());

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "2 spell points healed");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus heal spell points", 2);
    modifier->set("registration list", ({ Attacker }));

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "0 spell points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "0 spell points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "0 spell points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "0 spell points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "0 spell points healed");

    Attacker->heart_beat();
    ExpectEq(28, Attacker->spellPoints(), "4 spell points healed");
}

/////////////////////////////////////////////////////////////////////////////
void HeartBeatHealsSPAtDifferentRatesWhenBonusApplied()
{
    Attacker->hitPoints(Attacker->maxHitPoints());
    Attacker->spellPoints(20);
    Attacker->staminaPoints(Attacker->maxStaminaPoints());

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus heal spell points rate", 10);
    modifier->set("registration list", ({ Attacker }));

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "2 spell points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "0 spell points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->spellPoints(), "0 spell points healed");

    Attacker->heart_beat();
    ExpectEq(26, Attacker->spellPoints(), "2 spell points healed");

    Attacker->heart_beat();
    ExpectEq(26, Attacker->spellPoints(), "0 spell points healed");
}

/////////////////////////////////////////////////////////////////////////////
void HeartBeatHealsStaminaPointsWhenBelowMaxEveryFifthTime()
{
    Attacker->hitPoints(Attacker->maxHitPoints());
    Attacker->spellPoints(Attacker->maxSpellPoints());
    Attacker->staminaPoints(20);

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "2 stamina points healed");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus heal stamina", 2);
    modifier->set("registration list", ({ Attacker }));

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "0 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "0 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "0 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "0 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "0 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(28, Attacker->staminaPoints(), "4 stamina points healed");
}

/////////////////////////////////////////////////////////////////////////////
void HeartBeatHealsStaminaAtDifferentRatesWhenBonusApplied()
{
    Attacker->hitPoints(Attacker->maxHitPoints());
    Attacker->spellPoints(Attacker->maxSpellPoints());
    Attacker->staminaPoints(20);

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("bonus heal stamina rate", 10);
    modifier->set("registration list", ({ Attacker }));

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "2 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "0 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(23, Attacker->staminaPoints(), "0 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(26, Attacker->staminaPoints(), "2 stamina points healed");

    Attacker->heart_beat();
    ExpectEq(26, Attacker->staminaPoints(), "0 stamina points healed");
}

/////////////////////////////////////////////////////////////////////////////
void DamageReflectionIsTriggered()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);
    move_object(Target, room);

    Attacker->hitPoints(Attacker->maxHitPoints());
    Target->hitPoints(Target->maxHitPoints());
    Attacker->registerAttacker(Target);
    Target->registerAttacker(Attacker);

    object handler = clone_object("/lib/tests/support/events/mockEventSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered");

    // An onHit event would get triggered if Attacker is hit
    ExpectTrue(Target->hit(25, "physical"), "target hit is called");

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("damage reflection", 15);
    ExpectEq(1, modifier->set("registration list", ({ Target })), "registration list can be set");

    string err = catch (ExpectTrue(Target->hit(25, "physical"), "attack reflected on attacker"));
    ExpectEq("*event handler: onHit called, data: physical 3, caller: lib/tests/support/services/combatWithMockServices.c",
        err, "onHit event fired");
}

/////////////////////////////////////////////////////////////////////////////
void SlowDoesNotAttackEveryRound()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);
    move_object(Target, room);

    Attacker->hitPoints(Attacker->maxHitPoints());
    Target->hitPoints(Target->maxHitPoints());

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("slow", 1);
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");

    Attacker->attack(Target);

    object handler = clone_object("/lib/tests/support/events/onAttackSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered for attacker");

    // Expect that the first call to heart_beat does not initiate an attack
    ExpectEq(0, handler->TimesOnAttackReceived(), "before heart_beat, no onAttack events fired");
    Attacker->heart_beat();
    ExpectEq(0, handler->TimesOnAttackReceived(), "after heart_beat, no onAttack events fired");
    Attacker->heart_beat();
    ExpectEq(1, handler->TimesOnAttackReceived(), "after second heart_beat, one onAttack event fired");
    Attacker->heart_beat();
    ExpectEq(1, handler->TimesOnAttackReceived(), "after third heart_beat, one onAttack event fired");
    Attacker->heart_beat();
    ExpectEq(2, handler->TimesOnAttackReceived(), "after fourth heart_beat, two onAttack event fired");
}

/////////////////////////////////////////////////////////////////////////////
void HasteAddsAnExtraAttack()
{
    object room = clone_object("/lib/environment/room");
    move_object(Attacker, room);
    move_object(Target, room);

    Attacker->hitPoints(Attacker->maxHitPoints());
    Target->hitPoints(Target->maxHitPoints());

    object modifier = clone_object("/lib/items/modifierObject");
    modifier->set("fully qualified name", "blah");
    modifier->set("haste", 1);
    ExpectEq(1, modifier->set("registration list", ({ Attacker })), "registration list can be set");

    Attacker->attack(Target);

    object handler = clone_object("/lib/tests/support/events/onAttackSubscriber");
    ExpectTrue(Attacker->registerEvent(handler), "event handler registered for attacker");

    ExpectEq(0, handler->TimesOnAttackReceived(), "before heart_beat, no onAttack events fired");
    Attacker->heart_beat();
    ExpectEq(2, handler->TimesOnAttackReceived(), "after heart_beat, two onAttack events fired");
    Attacker->heart_beat();
    ExpectEq(4, handler->TimesOnAttackReceived(), "after second heart_beat, four onAttack event fired");
    Attacker->heart_beat();
}

