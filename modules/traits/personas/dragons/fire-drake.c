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
        addSpecification("name", "fire drake");
        addSpecification("description", "You are a fire drake.");
        addSpecification("root", "creature persona");
        addSpecification("bonus resist fire", 75);
        "baseTrait"::reset(arg);
    }
}
