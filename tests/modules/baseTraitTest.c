//*****************************************************************************
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
inherit "/lib/tests/framework/testFixture.c";
#include "/lib/include/inventory.h"

object Trait;

/////////////////////////////////////////////////////////////////////////////
void Setup()
{
    Trait = clone_object("/lib/tests/support/traits/testTraitWithoutResearch.c");
    Trait->init();
}

/////////////////////////////////////////////////////////////////////////////
void CleanUp()
{
    destruct(Trait);
}

/////////////////////////////////////////////////////////////////////////////
void SettingInvalidBonusThrowsError()
{
    string err = catch (Trait->addSpecification("bonus llama", 2));
    string expectedError = "*ERROR - trait: the 'bonus llama' specification must be a valid modifier as defined in lib/dictionaries/bonusesDictionary.c\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid names");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetBonusAttributes()
{
    ExpectEq(1, Trait->addSpecification("bonus strength", 2), "set bonus strength to 2");
    ExpectEq(2, Trait->query("bonus strength"), "can query the bonus");
    ExpectEq(2, Trait->queryBonus("bonus strength"), "queryBonus returns the correct result");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetPenaltyAttributes()
{
    ExpectEq(1, Trait->addSpecification("penalty to strength", 2), "set penalty to strength to 2");
    ExpectEq(-2, Trait->query("bonus strength"), "querying the bonus applies the penalty");
    ExpectEq(-2, Trait->queryBonus("bonus strength"), "queryBonus returns the correct result");
}


/////////////////////////////////////////////////////////////////////////////
void CanSetBonusSkill()
{
    ExpectEq(1, Trait->addSpecification("bonus long sword", 3), "set bonus long sword to 2");
    ExpectEq(3, Trait->query("bonus long sword"), "can query the bonus");
    ExpectEq(3, Trait->queryBonus("bonus long sword"), "queryBonus returns the correct result");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetPenaltyToSkill()
{
    ExpectEq(1, Trait->addSpecification("penalty to long sword", 3), "set penalty to long sword to 2");
    ExpectEq(-3, Trait->query("bonus long sword"), "querying the bonus applies the penalty");
    ExpectEq(-3, Trait->queryBonus("bonus long sword"), "queryBonus returns the correct result");
}

/////////////////////////////////////////////////////////////////////////////
void SetBonusAttackAddsExtraAttack()
{
    mapping *expectedAttacks = ({ (["attack type":"fire", "damage": 15, "to hit": 35]) });

    ExpectEq(1, Trait->addSpecification("bonus fire attack", 15), "set bonus fire attack");
    ExpectEq(expectedAttacks, Trait->getExtraAttacks(), "getExtraAttacks returns the correct result");
}

/////////////////////////////////////////////////////////////////////////////
void SetBonusWeaponAttackAddsExtraAttack()
{
    mapping *expectedAttacks = ({ (["attack type":"weapon"]), (["attack type":"weapon"]) });

    ExpectEq(1, Trait->addSpecification("bonus weapon attack", 2), "set bonus weapon attack");
    ExpectEq(expectedAttacks, Trait->getExtraAttacks(), "getExtraAttacks returns the correct result");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetMultipleBonusAttacks()
{
    mapping *expectedAttacks = ({ (["attack type":"fire", "damage": 15, "to hit": 35]), (["attack type":"weapon"]) });

    ExpectEq(1, Trait->addSpecification("bonus fire attack", 15), "set bonus fire attack");
    ExpectEq(1, Trait->addSpecification("bonus weapon attack", 1), "set bonus weapon attack");
    ExpectEq(expectedAttacks, Trait->getExtraAttacks(), "getExtraAttacks returns the correct result");
}

/////////////////////////////////////////////////////////////////////////////
void SettingInvalidTypeThrowsError()
{
    string err = catch (Trait->addSpecification("type", "blah"));
    string expectedError = "*ERROR - trait: 'blah' is not a valid trait type.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid names");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetTypeToHealth()
{
    ExpectEq(1, Trait->addSpecification("type", "health"), "set the type");
    ExpectEq("health", Trait->query("type"), "can query the type");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetTypeToEducational()
{
    ExpectEq(1, Trait->addSpecification("type", "educational"), "set the type");
    ExpectEq("educational", Trait->query("type"), "can query the type");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetTypeToPersonality()
{
    ExpectEq(1, Trait->addSpecification("type", "personality"), "set the type");
    ExpectEq("personality", Trait->query("type"), "can query the type");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetTypeToGenetic()
{
    ExpectEq(1, Trait->addSpecification("type", "genetic"), "set the type");
    ExpectEq("genetic", Trait->query("type"), "can query the type");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetTypeToProfessional()
{
    ExpectEq(1, Trait->addSpecification("type", "professional"), "set the type");
    ExpectEq("professional", Trait->query("type"), "can query the type");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetTypeToGuild()
{
    ExpectEq(1, Trait->addSpecification("type", "guild"), "set the type");
    ExpectEq("guild", Trait->query("type"), "can query the type");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetTypeToRole()
{
    ExpectEq(1, Trait->addSpecification("type", "role"), "set the type");
    ExpectEq("role", Trait->query("type"), "can query the type");
}

/////////////////////////////////////////////////////////////////////////////
void SettingInvalidRootThrowsError()
{
    string err = catch (Trait->addSpecification("root", 3));
    string expectedError = "*ERROR - trait: The 'root' value must be a string.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid root");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetRoot()
{
    ExpectEq(1, Trait->addSpecification("root", "some root"), "set the root");
    ExpectEq("some root", Trait->query("root"), "can query the root");
}

/////////////////////////////////////////////////////////////////////////////
void SettingInvalidOpposingRootThrowsError()
{
    string err = catch (Trait->addSpecification("opposing root", ({ "blah" })));
    string expectedError = "*ERROR - trait: The 'opposing root' value must be a string.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid opposing root");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetOpposingRoot()
{
    ExpectEq(1, Trait->addSpecification("opposing root", "some root"), "set the opposing root");
    ExpectEq("some root", Trait->query("opposing root"), "can query the opposing root");
}

/////////////////////////////////////////////////////////////////////////////
void SettingInvalidOpinionThrowsError()
{
    string err = catch (Trait->addSpecification("opinion", ([ "blah": 5 ])));
    string expectedError = "*ERROR - trait: The 'opinion' value must be an integer.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid opinion");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetOpinion()
{
    ExpectEq(1, Trait->addSpecification("opinion", 10), "set the opinion");
    ExpectEq(10, Trait->query("opinion"), "can query the opinion");
}

/////////////////////////////////////////////////////////////////////////////
void SettingInvalidOpposingOpinionThrowsError()
{
    string err = catch (Trait->addSpecification("opposing opinion", "blah"));
    string expectedError = "*ERROR - trait: The 'opposing opinion' value must be an integer.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid opposing opinion");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetOpposingOpinion()
{
    ExpectEq(1, Trait->addSpecification("opposing opinion", -10), "set the opposing opinion");
    ExpectEq(-10, Trait->query("opposing opinion"), "can query the opposing opinion");
}

/////////////////////////////////////////////////////////////////////////////
void SettingInvalidCostThrowsError()
{
    string err = catch (Trait->addSpecification("cost", "blah"));
    string expectedError = "*ERROR - trait: The 'cost' value must be an integer.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid cost");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetCost()
{
    ExpectEq(1, Trait->addSpecification("cost", 5), "set the cost");
    ExpectEq(5, Trait->query("cost"), "can query the cost");
}

/////////////////////////////////////////////////////////////////////////////
void SettingInvalidResearchTreeThrowsError()
{
    string err = catch (Trait->addSpecification("research tree", "fake/research/tree.c"));
    string expectedError = "*ERROR - trait: The 'research tree' value must be a valid research tree.\n";

    ExpectEq(expectedError, err, "The correct exception is thrown when setting invalid opinion");
}

/////////////////////////////////////////////////////////////////////////////
void CanSetResearchTree()
{
    ExpectEq(1, Trait->addSpecification("research tree", "lib/tests/support/research/testResearchTree.c"), "set the research tree");
    ExpectEq("lib/tests/support/research/testResearchTree.c", Trait->query("research tree"), "can query the research tree");
}