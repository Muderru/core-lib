//*****************************************************************************
// Class: baseCommand
// File Name: baseCommand.c
//
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//
// Description: TBD
//
//*****************************************************************************
#include <mtypes.h>

private string MaterialAttributes = "lib/modules/materialAttributes.c";
private string MessageParser = "/lib/core/messageParser.c";

/////////////////////////////////////////////////////////////////////////////
private nomask object messageParser()
{
    return load_object(MessageParser);
}

/////////////////////////////////////////////////////////////////////////////
private nomask int isValidLiving(object livingCheck)
{
    return (livingCheck && objectp(livingCheck) && 
            (member(inherit_list(livingCheck), MaterialAttributes) > -1));
}

/////////////////////////////////////////////////////////////////////////////
private nomask string formatText(string text, int colorInfo, object viewer)
{
    return color(viewer->query("term"), viewer, colorInfo, format(text, 78));
}

/////////////////////////////////////////////////////////////////////////////
private nomask string parseTemplate(string template, string perspective,
                                    object initiator, object target)
{
    string message = template;
    // ##Infinitive::verb##
    // ##InitiatorName## - Initiator's name
    // ##InitiatorPossessive[::Name]## - Initiator possessive / your / Name's
    // ##InitiatorObjective## - Initiator's objective / you
    // ##InitiatorSubjective## - Initiator's subjective/pronoun / you
    // ##Target[NPOS]## - Target's (one of above 4)
    // ##HitDictionary## - random word from the hit dictionary (slash/slashes,
    //   chop/chops)
    // ##SimileDictionary## - random word from the simile dictionary

    // dictionary calls must be done first!
    int isSecondPerson = (perspective == "initiator");
    
    if(initiator && objectp(initiator))
    {
        message = messageParser()->parseSimileDictionary(message, initiator);
        message = messageParser()->parseVerbDictionary(message, 
            "HitDictionary", initiator);
            
        message = messageParser()->parseVerbs(message, isSecondPerson);
    }
    
    if(isValidLiving(initiator))
    {
        message = messageParser()->parseTargetInfo(message, "Initiator", 
            initiator, isSecondPerson);
    }

    if(isValidLiving(target))
    {    
        isSecondPerson = (perspective == "target");
        message = messageParser()->parseTargetInfo(message, "Target", 
            target, isSecondPerson);      
    }
    
    message = messageParser()->capitalizeSentences(message);
    return message;    
}

/////////////////////////////////////////////////////////////////////////////
protected nomask void displayMessage(string message, object initiator,
    object target)
{
    // This annoying loop handles the fact that everyone has different
    // setting for color.
    if (environment(initiator))
    {
        foreach(object person in all_inventory(environment(initiator)))
        {
            if (person && objectp(person))// && interactive(person))
            {
                string parsedMessage;
                int colorInfo = C_EMOTES;
                if (person == initiator)
                {
                    parsedMessage = parseTemplate(message, "initiator", initiator,
                        target);
                }
                else if (person == target)
                {
                    parsedMessage = parseTemplate(message, "target",
                        initiator, target);
                }
                else
                {
                    parsedMessage = parseTemplate(message, "other",
                        initiator, target);
                }
                tell_object(person, formatText(message, colorInfo,
                    person));
            }
        }
    }
} 

/////////////////////////////////////////////////////////////////////////////
protected nomask void displayMessageToSelf(string message, object initiator)
{
    if (initiator && objectp(initiator))
    {
        string parsedMessage;
        int colorInfo = C_EMOTES;

        parsedMessage = parseTemplate(message, "initiator", initiator,
            initiator);

        tell_object(initiator, formatText(message, colorInfo,
            initiator));
    }
}
