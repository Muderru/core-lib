//*****************************************************************************
// Copyright (c) 2018 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
inherit "/lib/commands/baseCommand.c";

/////////////////////////////////////////////////////////////////////////////
public nomask void reset(int arg)
{
    if (!arg)
    {
        CommandType = "Player R&D";
        addCommandTemplate("crafting");
        addCommandTemplate("craft");
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask void onSelectorCompleted(object caller)
{
    caller->cleanUp();
}

/////////////////////////////////////////////////////////////////////////////
public nomask void onSelectorAborted(object caller)
{
    caller->cleanUp();
}

/////////////////////////////////////////////////////////////////////////////
public nomask int execute(string command, object initiator)
{
    int ret = 0;

    if (canExecuteCommand(command))
    {
        object selector = clone_object("/lib/modules/crafting/craftingSelector.c");
        move_object(selector, initiator);
        selector->registerEvent(this_object());
        selector->initiateSelector(initiator);
        ret = 1;
    }
    return ret;
}
