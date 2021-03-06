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
        addSpecification("name", "storm drake");
        addSpecification("description", "You are a storm drake.");
        addSpecification("root", "creature persona");
        addSpecification("bonus resist air", 75);
        addSpecification("bonus resist electricity", 75);
        "baseTrait"::reset(arg);
    }
}
