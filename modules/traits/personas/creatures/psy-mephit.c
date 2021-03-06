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
        addSpecification("name", "psy mephit");
        addSpecification("description", "You are a psy mephit.");
        addSpecification("root", "creature persona");
        addSpecification("bonus resist psionic", 25);
        addSpecification("bonus resist physical", 20);
        "baseTrait"::reset(arg);
    }
}
