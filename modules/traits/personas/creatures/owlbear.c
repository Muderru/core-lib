//*****************************************************************************
// Copyright (c) 2018 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
inherit "/lib/modules/traits/baseTrait.c";

/////////////////////////////////////////////////////////////////////////////
public void reset(int arg)
{
    if (!arg)
    {
        addSpecification("type", "persona");
        addSpecification("name", "owlbear");
        addSpecification("description", "You are an owlbear.");
        addSpecification("root", "creature persona");
        addSpecification("bonus resist physical", 15);
        "baseTrait"::reset(arg);
    }
}
