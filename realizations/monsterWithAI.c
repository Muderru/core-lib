//*****************************************************************************
// Class: monsterWithAI
// File Name: monsterWithAI.c
//
// Copyright (c) 2011 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//
// Description: TBD
//
//*****************************************************************************
virtual inherit "/lib/realizations/monster.c";
virtual inherit "/lib/modules/artificialIntelligence.c";

public nomask int isRealizationOfMonsterWithAI()
{
    return 1;
}
