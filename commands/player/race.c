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
        CommandType = "Social";
        addCommandTemplate("race [.*]");
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask int execute(string command, object initiator)
{
    int ret = 0;

    if (canExecuteCommand(command))
    {
        if (initiator->Race() && (initiator->Race() != ""))
        {
            string message = 0;
            if (sizeof(regexp(({ command }), "^race (.+)")))
            {
                ret = 1;
                message = regreplace(command, "^race (.+)", "\\1");

                object channels = load_object("/lib/dictionaries/channelDictionary.c");
                if (channels && sizeof(message))
                {
                    channels->broadcastMessage(initiator->Race(), message, initiator);
                }
            }
            else
            {
                notify_fail("What do you want to say on the race line?\n");
            }
        }
        else
        {
            notify_fail("In order to use the race command, you need to have a race set?\n");
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
protected string wildcardMeaning()
{
    return "\x1b[0;31;1m<Message to speak>\x1b[0m";
}

/////////////////////////////////////////////////////////////////////////////
protected string synopsis(string displayCommand)
{
    return "Send a message to every logged-in player of your race";
}

/////////////////////////////////////////////////////////////////////////////
protected string description(string displayCommand)
{
    return format("Race allows a player to display a message to all players "
        "of their race who are currently logged on.", 78);
}

/////////////////////////////////////////////////////////////////////////////
protected string notes(string displayCommand)
{
    return "See also: tell, whisper, reply, say, guild, faction, party, "
        "and shout";
}
