//*****************************************************************************
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
inherit "/lib/tests/framework/testFixture.c";

object Player;
object Selector;

/////////////////////////////////////////////////////////////////////////////
void Setup()
{
    Selector = clone_object("/lib/modules/crafting/craftItemSelector.c");
    Selector->setType("armor");
    Player = clone_object("/lib/tests/support/services/mockPlayer.c");
    Player->Name("bob");
    Player->addCommands();

    move_object(Selector, Player);
}

/////////////////////////////////////////////////////////////////////////////
void CleanUp()
{
    destruct(Player);
    destruct(Selector);
}

/////////////////////////////////////////////////////////////////////////////
void TopLevelMenuWithoutAnyPrereqsMetDisplaysCorrectly()
{
    Selector->initiateSelector(Player);

    ExpectEq("[0;36mCraft Armor - [0m[0;37;1mFrom this menu, you can craft armor[0m:\n"
        "\t[[0;31;1m1[0m] - [0;32mAccessory           [0m\n"
        "\t[[0;31;1m2[0m] - [0;32mClothing            [0m\n"
        "\t[[0;31;1m3[0m] - [0;32mHeavy armor         [0m\n"
        "\t[[0;31;1m4[0m] - [0;32mLight armor         [0m\n"
        "\t[[0;31;1m5[0m] - [0;32mMedium armor        [0m\n"
        "\t[[0;31;1m6[0m] - [0;32mExit Craft Armor Menu[0m\n"
        "[0;32;1mYou must select a number from 1 to 6.\n[0m"
        "[0;32mFor details on a given choice, type 'describe X' (or '? X') where\nX is the option about which you would like further details.\n[0m[0;32;1m[0m",
        Player->caughtMessage());
}
