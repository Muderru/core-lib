//*****************************************************************************
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
inherit "/lib/tests/framework/testFixture.c";
#include "/lib/include/inventory.h"

object ResearchItem;
object User;
object Target;
object Room;

/////////////////////////////////////////////////////////////////////////////
void Setup()
{
    ResearchItem = clone_object("/lib/tests/support/research/testPersistedActiveResearchItem");
    ResearchItem->init();
    ResearchItem->testAddSpecification("command template", "throw turnip at ##Target##");
    ResearchItem->testAddSpecification("scope", "targetted");

    User = clone_object("/lib/tests/support/services/combatWithMockServices");
    User->Name("Bob");
    User->addAlias("bob");
    User->Str(20);
    User->Int(20);
    User->Dex(20);
    User->Con(20);
    User->Wis(20);
    User->Chr(20);
    User->hitPoints(User->maxHitPoints());
    User->spellPoints(User->maxSpellPoints());
    User->staminaPoints(User->maxStaminaPoints());
    User->addSkillPoints(200);
    User->advanceSkill("long sword", 5);
    User->toggleKillList();

    Target = clone_object("/lib/realizations/monster");
    Target->Name("Frank");
    Target->addAlias("frank");
    Target->Str(20);
    Target->Int(20);
    Target->Dex(20);
    Target->Con(20);
    Target->Wis(20);
    Target->Chr(20);
    Target->hitPoints(50);
    Target->spellPoints(50);
    Target->staminaPoints(50);
    Target->addSkillPoints(200);
    Target->advanceSkill("long sword", 10);

    Room = clone_object("/lib/environment/room");
    move_object(User, Room);
    move_object(Target, Room);
}

/////////////////////////////////////////////////////////////////////////////
void CleanUp()
{
    destruct(Target);
    destruct(User);
    destruct(ResearchItem);
    destruct(Room);
}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationAllowsValidBonusesToBeApplied()
{
    ExpectTrue(ResearchItem->testAddSpecification("bonus hit points", 10));
    ExpectTrue(ResearchItem->testAddSpecification("bonus long sword", 10));
    ExpectTrue(ResearchItem->testAddSpecification("bonus strength", 10));
    ExpectTrue(ResearchItem->testAddSpecification("bonus magical attack", 1));
}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationThrowsForInvalidBonuses()
{
    string err = catch (ResearchItem->testAddSpecification("bonus blarg", 10));
    string expectedError = "*ERROR - persistedActiveResearchItem: the 'bonus blarg' specification must be a valid modifier as defined in lib/dictionaries/bonusesDictionary.c\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid value");
}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationAllowsValidPenaltiesToBeApplied()
{
    ExpectTrue(ResearchItem->testAddSpecification("penalty to attack", 5));
    ExpectTrue(ResearchItem->testAddSpecification("penalty to heal spell points rate", 10));
    ExpectTrue(ResearchItem->testAddSpecification("penalty to strength", 10));
}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationThrowsForInvalidPenalties()
{
    string err = catch (ResearchItem->testAddSpecification("penalty to blarg", 10));
    string expectedError = "*ERROR - persistedActiveResearchItem: the 'penalty to blarg' specification must be a valid modifier as defined in lib/dictionaries/bonusesDictionary.c\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid value");
}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationAllowsDurationToBeApplied()
{
    ExpectTrue(ResearchItem->testAddSpecification("duration", 30));
}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationDoesNotAllowNegativeDuration()
{
    string err = catch (ResearchItem->testAddSpecification("duration", -10));
    string expectedError = "*ERROR - persistedActiveResearchItem: the duration specification must be a positive integer.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid value");
}

/////////////////////////////////////////////////////////////////////////////
void DurationMustBeAnInteger()
{
    string err = catch (ResearchItem->testAddSpecification("duration", "blah"));
    string expectedError = "*ERROR - persistedActiveResearchItem: the duration specification must be a positive integer.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid value");
}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationAllowsValidModifierToBeSet()
{
    mapping *modifiers = ({ ([
        "type":"skill",
        "name": "long sword",
        "formula": "additive",
        "rate": 1.25
    ]),
    ([
        "type":"attribute",
        "name": "strength",
        "formula": "subtractive",
        "rate": 0.25
    ]) });

    ExpectTrue(ResearchItem->testAddSpecification("modifier", modifiers));
}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationDoesNotAllowInvalidModifierToBeSet()
{
    mapping *modifiers = ({ ([
        "type":"skill",
        "name": "long sword",
        "formula": "additive",
        "rate": 1.25
    ]),
    ([
        "type":"modifier that will fail",
        "name": "strength",
        "rate": 0.25
    ]) });

    string err = catch (ResearchItem->testAddSpecification("modifier", modifiers));
    string expectedError = "*ERROR - persistedActiveResearchItem: the 'modifier' specification must be a properly formatted modifier.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid value");

}

/////////////////////////////////////////////////////////////////////////////
void AddSpecificationDoesNotAllowMalformedModifierToBeSet()
{
    string err = catch (ResearchItem->testAddSpecification("modifier", "blah"));
    string expectedError = "*ERROR - persistedActiveResearchItem: the 'modifier' specification must be a properly formatted modifier.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid value");
}

/////////////////////////////////////////////////////////////////////////////
void GetTargetReturnsFalseIfCommandDoesNotParse()
{
    ExpectFalse(ResearchItem->testGetTarget(User, "flumfrug blibblefro"));
}

/////////////////////////////////////////////////////////////////////////////
void GetTargetReturnsFalseIfTargetDoesNotExist()
{
    ExpectFalse(ResearchItem->testGetTarget(User, "throw turnip at gertrude"));
}

/////////////////////////////////////////////////////////////////////////////
void GetTargetReturnsFalseIfTargetNotPresent()
{
    object room = clone_object("/lib/environment/room");
    move_object(Target, room);

    ExpectFalse(ResearchItem->testGetTarget(User, "throw turnip at frank"));
}

/////////////////////////////////////////////////////////////////////////////
void GetTargetReturnsValidTarget()
{
    ExpectEq(Target, ResearchItem->testGetTarget(User, "throw turnip at frank"));
}

/////////////////////////////////////////////////////////////////////////////
void ExecuteOnSelfAppliesEffectOnSelf()
{
    object weapon = clone_object("/lib/items/weapon");
    weapon->set("name", "blah");
    weapon->set("defense class", 2);
    weapon->set("weapon class", 10);
    weapon->set("material", "galvorn");
    weapon->set("weapon type", "long sword");
    weapon->set("equipment locations", OnehandedWeapon);
    move_object(weapon, User);
    weapon->equip("blah");

    ExpectTrue(ResearchItem->testAddSpecification("bonus long sword", 10));
    ExpectTrue(ResearchItem->testAddSpecification("bonus magical attack", 10));
    ExpectTrue(ResearchItem->testAddSpecification("bonus weapon attack", 2));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectEq(5, User->getSkill("long sword"), "initial long sword skill");
    ExpectEq("({ ([ attack type: weapon, ]), })", User->getAttacks(), "only one attack initially");
    ExpectTrue(ResearchItem->testExecuteOnSelf(User, program_name(ResearchItem)), "can execute command");

    object modifier = User->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User));
    ExpectEq("lib/tests/support/research/testPersistedActiveResearchItem.c#lib/tests/support/services/combatWithMockServices.c", 
        modifier->query("fully qualified name"), "Modifier with FQN is registered");

    ExpectEq(15, User->getSkill("long sword"), "long sword skill after research used");

    mapping *expectedAttacks = ({ (["attack type": "magical", "damage": 10, "to hit": 35]), (["attack type":"weapon"]), (["attack type":"weapon"]), (["attack type":"weapon"]) });
    ExpectEq(expectedAttacks, User->getAttacks(), "Three weapon attacks and a magical attack are returned");
}

/////////////////////////////////////////////////////////////////////////////
void ExecuteOnSelfCanBeDoneMultipleTimes()
{
    ExpectTrue(ResearchItem->testAddSpecification("bonus long sword", 10));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectTrue(ResearchItem->testExecuteOnSelf(User, program_name(ResearchItem)), "can execute command");
    ExpectEq(15, User->getSkill("long sword"), "long sword skill after research used first time");

    ExpectTrue(ResearchItem->testAddSpecification("bonus long sword", 12));
    ExpectTrue(ResearchItem->testExecuteOnSelf(User, program_name(ResearchItem)), "can execute command second time");
    ExpectEq(17, User->getSkill("long sword"), "long sword skill after research used second time");
}

/////////////////////////////////////////////////////////////////////////////
void ExecuteOnTargetAppliesEffectOnTarget()
{
    ExpectTrue(ResearchItem->testAddSpecification("bonus long sword", 5));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectEq(10, Target->getSkill("long sword"), "initial long sword skill");
    ExpectTrue(ResearchItem->testExecuteOnTarget("throw turnip at frank", User, program_name(ResearchItem)), "can execute command");

    object modifier = Target->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User));
    ExpectEq("lib/tests/support/research/testPersistedActiveResearchItem.c#lib/tests/support/services/combatWithMockServices.c",
        modifier->query("fully qualified name"), "Modifier with FQN is registered");

    ExpectEq(15, Target->getSkill("long sword"), "long sword skill after research used");
}

/////////////////////////////////////////////////////////////////////////////
void ExecuteOnTargetAppliesNegativeEffect()
{
    ExpectTrue(ResearchItem->testAddSpecification("penalty to long sword", 5));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectEq(10, Target->getSkill("long sword"), "initial long sword skill");
    ExpectTrue(ResearchItem->testExecuteOnTarget("throw turnip at frank", User, program_name(ResearchItem)), "can execute command");

    ExpectTrue(Target->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectEq(5, Target->getSkill("long sword"), "long sword skill after research used");
}

/////////////////////////////////////////////////////////////////////////////
void ExecuteOnTargetFailsIfEffectNegativeAndTargetNotOnKillList()
{
    destruct(Target);
    object Target = clone_object("/lib/tests/support/services/combatWithMockServices");
    Target->Name("Frank");
    Target->addAlias("frank");
    move_object(Target, Room);

    ExpectTrue(ResearchItem->testAddSpecification("penalty to long sword", 5));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectFalse(ResearchItem->testExecuteOnTarget("throw turnip at frank", User, program_name(ResearchItem)), "can execute command");

    ExpectFalse(Target->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
}

/////////////////////////////////////////////////////////////////////////////
void ExecuteOnTargetFailsIfEffectNegativeAndTargetButNotUserOnKillList()
{
    User->toggleKillList();

    destruct(Target);
    object Target = clone_object("/lib/tests/support/services/combatWithMockServices");
    Target->Name("Frank");
    Target->addAlias("frank");
    Target->toggleKillList();
    move_object(Target, Room);

    ExpectTrue(ResearchItem->testAddSpecification("penalty to long sword", 5));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectFalse(ResearchItem->testExecuteOnTarget("throw turnip at frank", User, program_name(ResearchItem)), "can execute command");

    ExpectFalse(Target->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
}

/////////////////////////////////////////////////////////////////////////////
void ExecuteOnTargetAppliedIfBothPlayersOnKillList()
{
    destruct(Target);
    object Target = clone_object("/lib/tests/support/services/combatWithMockServices");
    Target->Name("Frank");
    Target->addAlias("frank");
    Target->toggleKillList();
    Target->Str(20);
    Target->Int(20);
    Target->Dex(20);
    Target->Con(20);
    Target->Wis(20);
    Target->Chr(20);
    Target->addSkillPoints(200);
    Target->advanceSkill("long sword", 10);
    move_object(Target, Room);

    ExpectTrue(ResearchItem->testAddSpecification("penalty to long sword", 5));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectEq(10, Target->getSkill("long sword"), "initial long sword skill");
    ExpectTrue(ResearchItem->testExecuteOnTarget("throw turnip at frank", User, program_name(ResearchItem)), "can execute command");

    ExpectTrue(Target->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectEq(5, Target->getSkill("long sword"), "long sword skill after research used");
}

/////////////////////////////////////////////////////////////////////////////
void NegativeExecuteInAreaAppliedOnCorrectTargets()
{
    object bystander = clone_object("/lib/tests/support/services/combatWithMockServices");
    bystander->Name("Earl");
    bystander->addAlias("earl");
    bystander->Str(20);
    bystander->addSkillPoints(200);
    bystander->advanceSkill("long sword", 10);
    move_object(bystander, Room);

    object badguy = clone_object("/lib/realizations/monster");
    badguy->Name("Fred");
    badguy->addAlias("fred");
    badguy->Str(20);
    badguy->addSkillPoints(200);
    badguy->advanceSkill("long sword", 10);
    move_object(badguy, Room);

    ExpectTrue(ResearchItem->testAddSpecification("penalty to long sword", 5));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectEq(5, User->getSkill("long sword"), "initial user long sword skill");
    ExpectEq(10, Target->getSkill("long sword"), "initial target long sword skill");
    ExpectEq(10, bystander->getSkill("long sword"), "initial bystander long sword skill");
    ExpectEq(10, badguy->getSkill("long sword"), "initial badguy long sword skill");
    ExpectTrue(ResearchItem->testExecuteInArea(User, program_name(ResearchItem)), "can execute command");

    ExpectFalse(User->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectTrue(Target->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectTrue(badguy->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectFalse(bystander->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectEq(5, User->getSkill("long sword"), "user long sword skill after research used");
    ExpectEq(5, Target->getSkill("long sword"), "target long sword skill after research used");
    ExpectEq(10, bystander->getSkill("long sword"), "bystander long sword skill after research used");
    ExpectEq(5, badguy->getSkill("long sword"), "badguy long sword skill after research used");
}

/////////////////////////////////////////////////////////////////////////////
void ExecuteInAreaAppliedOnCorrectTargets()
{
    object bystander = clone_object("/lib/tests/support/services/combatWithMockServices");
    bystander->Name("Earl");
    bystander->addAlias("earl");
    bystander->Str(20);
    bystander->addSkillPoints(200);
    bystander->advanceSkill("long sword", 10);
    move_object(bystander, Room);

    object badguy = clone_object("/lib/realizations/monster");
    badguy->Name("Fred");
    badguy->addAlias("fred");
    badguy->Str(20);
    badguy->addSkillPoints(200);
    badguy->advanceSkill("long sword", 10);
    move_object(badguy, Room);

    ExpectTrue(ResearchItem->testAddSpecification("bonus long sword", 5));
    ExpectTrue(ResearchItem->testAddSpecification("duration", 10));

    ExpectEq(5, User->getSkill("long sword"), "initial user long sword skill");
    ExpectEq(10, Target->getSkill("long sword"), "initial target long sword skill");
    ExpectEq(10, bystander->getSkill("long sword"), "initial bystander long sword skill");
    ExpectEq(10, badguy->getSkill("long sword"), "initial badguy long sword skill");
    ExpectTrue(ResearchItem->testExecuteInArea(User, program_name(ResearchItem)), "can execute command");

    ExpectTrue(User->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectFalse(Target->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectFalse(badguy->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectTrue(bystander->registeredInventoryObject(program_name(ResearchItem) + "#" + program_name(User)));
    ExpectEq(10, User->getSkill("long sword"), "user long sword skill after research used");
    ExpectEq(10, Target->getSkill("long sword"), "target long sword skill after research used");
    ExpectEq(15, bystander->getSkill("long sword"), "bystander long sword skill after research used");
    ExpectEq(10, badguy->getSkill("long sword"), "badguy long sword skill after research used");
}
