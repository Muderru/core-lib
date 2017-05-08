//*****************************************************************************
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
inherit "/lib/modules/research/passiveResearchItem.c";

/////////////////////////////////////////////////////////////////////////////
public void init()
{
    passiveResearchItem::init();
    addSpecification("name", "intox research");
    addSpecification("scope", "self");
    addSpecification("research type", "granted");
    addSpecification("limited by", (["intoxication":1]));
    addSpecification("bonus strength", 2);
    addSpecification("penalty to long sword", 2);
    addSpecification("bonus fire attack", 15);
    addSpecification("bonus weapon attack", 2);
}
