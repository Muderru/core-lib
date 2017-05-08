//*****************************************************************************
// Class: research
// File Name: research.c
//
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//
// Description: TBD
//
//*****************************************************************************
virtual inherit "/lib/core/thing.c";

// This mapping contains all of the research items that the living knows.
private mapping research = ([
//  <research file name>: ([
//      "when research began": time,
//      "time spent learning": seconds,
//      "research complete": true|false,
//  ])
]);
private string *openResearchTrees = ({ });
private mapping researchChoices = ([ ]);
private int researchPoints = 0;

/////////////////////////////////////////////////////////////////////////////
private nomask object researchDictionary()
{
    return getDictionary("research");
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isResearched(string researchItem)
{
    return (researchDictionary()->validResearch(researchItem) &&
            member(research, researchItem) && mappingp(research[researchItem])       
            && member(research[researchItem], "research complete") &&
            research[researchItem]["research complete"]);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isResearching(string researchItem)
{
    string *listOfTimedResearch = m_indices(research) -
        filter_array(m_indices(research), #'isResearched);

    return member(listOfTimedResearch, researchItem) > -1;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int isActiveOrSustainedResearchAbility(string researchItem)
{
    int ret = 0;
    if(researchItem && stringp(researchItem) && isResearched(researchItem))
    {
        ret = researchDictionary()->isActiveOrSustainedAbility(researchItem);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask varargs int canApplyLimitedEffect(string researchItem, string bonus)
{
    object researchObj = researchDictionary()->researchObject(researchItem);

    return isResearched(researchItem) && 
        researchDictionary()->researchEffectIsLimited(researchItem) ?
        (researchObj->canApplySkill(bonus, this_object(),
        function_exists("getTargetToAttack", this_object()) ?
        this_object()->getTargetToAttack() : 0,
        isActiveOrSustainedResearchAbility(researchItem))) : 1;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs int canApplyResearchBonus(string researchItem, string bonus)
{
    int ret = isResearched(researchItem) && canApplyLimitedEffect(researchItem, bonus);

    if(ret && isActiveOrSustainedResearchAbility(researchItem))
    {
        ret &&= ((member(research[researchItem], "sustained active") &&
            research[researchItem]["sustained active"]) ||
            (member(research[researchItem], "active count") &&
            (research[researchItem]["active count"] > 0)));
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int sustainedResearchIsActive(string researchItem)
{
    return (researchDictionary()->isSustainedAbility(researchItem) &&
        member(research[researchItem], "sustained active") &&
        research[researchItem]["sustained active"]);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int blockedByCooldown(string researchItem)
{
    return member(research, researchItem) && 
        member(research[researchItem], "cooldown") &&
        research[researchItem]["cooldown"] > 0;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs int activateSustainedResearch(object researchObj,
    object modifierObject)
{
    int ret = 0;
    object inventory = getService("inventory");
    string researchItem = objectp(researchObj) ? program_name(researchObj) : "???";

    if(inventory && !blockedByCooldown(researchItem) &&
       (!modifierObject || inventory->isModifierItem(modifierObject)) &&
       researchDictionary()->isSustainedAbility(researchItem) &&
       (!member(research[researchItem], "sustained active") ||
       (research[researchItem]["sustained active"] == 0))
       && canApplyLimitedEffect(researchItem))
    {
        research[researchItem]["sustained active"] = 1;
        ret = 1;
        
        if(member(research[researchItem], "active modifier object") &&
           research[researchItem]["active modifier object"] &&
           objectp(research[researchItem]["active modifier object"]))
        {
            object modifierToDestroy = 
                research[researchItem]["active modifier object"];
            m_delete(research[researchItem], "active modifier object");
            modifierToDestroy->unregisterModifierFromTargetList();
            destruct(modifierToDestroy);
        }
        if(modifierObject)
        {
            research[researchItem]["active modifier object"] = modifierObject;
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int deactivateSustainedResearch(string researchItem)
{
    int ret = 0;
    if(researchDictionary()->isSustainedAbility(researchItem) &&
       member(research[researchItem], "sustained active") &&
       research[researchItem]["sustained active"])
    {
        m_delete(research[researchItem], "sustained active");
        ret = 1;
        
        if(member(research[researchItem], "active modifier object") &&
           research[researchItem]["active modifier object"] &&
           objectp(research[researchItem]["active modifier object"]))
        {
            object modifierToDestroy = 
                research[researchItem]["active modifier object"];
            m_delete(research[researchItem], "active modifier object");
            modifierToDestroy->unregisterModifierFromTargetList();
            destruct(modifierToDestroy);        
        }

        object researchObj = researchDictionary()->researchObject(researchItem);
        if(researchObj && researchObj->query("cooldown"))
        {
            research[researchItem]["cooldown"] = researchObj->query("cooldown");
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask string *activeAndSustainedResearchAbilities()
{
    return filter_array(m_indices(research), 
        #'isActiveOrSustainedResearchAbility);
}        

/////////////////////////////////////////////////////////////////////////////
public nomask int addResearchPoints(int amount)
{
    int ret = 0;
    if(intp(amount) && (amount > 0))
    {
        researchPoints += amount;
        ret = researchPoints;
        
        object events = getService("events");
        if(events && objectp(events))
        {
            events->notify("onResearchPointsAdded");
        }        
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int researchPoints()
{
    return researchPoints;
}

/////////////////////////////////////////////////////////////////////////////
public nomask string *availableResearchTrees()
{
    string *ret = openResearchTrees;
    
    string *servicesToCheck = ({ "races", "guilds", "traits", "background" });
    
    foreach(string serviceToCheck in servicesToCheck)
    {
        string methodToExecute = sprintf("%sResearchTrees", serviceToCheck);

        object service = getService(serviceToCheck);
        if(service && function_exists(methodToExecute, service))
        {
            ret += call_other(service, methodToExecute);
        }
    }
    
    // Strip out any redundant research trees, any possible null elements
    ret = m_indices(mkmapping(ret));
    ret -= ({ 0 });
    return ret + ({ });
}

/////////////////////////////////////////////////////////////////////////////
public nomask string *completedResearch()
{
    return filter_array(m_indices(research), #'isResearched);
}

/////////////////////////////////////////////////////////////////////////////
public nomask string *researchInProgress()
{
    return m_indices(research) -
        filter_array(m_indices(research), #'isResearched);
}

/////////////////////////////////////////////////////////////////////////////
private nomask int checkTreeDependenciesMet(string tree, string researchItem)
{
    int ret = 0;
    if(tree && stringp(tree) && researchItem && stringp(researchItem))
    {
        object treeObj = researchDictionary()->researchTree(tree);
        if(treeObj)
        {
            if(treeObj->isMemberOfResearchTree(researchItem) &&
                treeObj->getResearchItem(researchItem))
            {
                ret = treeObj->prerequisitesMetFor(researchItem,
                    this_object());
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int canResearch(string researchItem)
{
    int ret = 0;
    if(researchDictionary()->validResearch(researchItem) && 
       !isResearched(researchItem))
    {        
        ret = 1;
        string *researchTrees = availableResearchTrees();
        foreach(string tree in researchTrees)
        {
            ret &&= checkTreeDependenciesMet(tree, researchItem);
        }
        ret &&= researchDictionary()->checkResearchPrerequisites(researchItem,
            this_object());
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask void registerResearchEvents()
{
    object eventObj = getService("events");

    if (eventObj && objectp(eventObj))
    {
        string *researchAbilities = activeAndSustainedResearchAbilities();
        foreach(string researchItem in researchAbilities)
        {
            object researchObj =
                researchDictionary()->researchObject(researchItem);

            if (researchObj && researchObj->query("event handler"))
            {
                eventObj->registerEventHandler(researchObj->query("event handler"));
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs int initiateResearch(string researchItem)
{
    int ret = 0;
    if(researchItem && stringp(researchItem) && 
       !member(research, researchItem) && canResearch(researchItem))
    {
        switch(researchDictionary()->getResearchType(researchItem))
        {
            case "points":
            {
                if (researchPoints >=
                    researchDictionary()->getResearchCost(researchItem))
                {
                    researchPoints -=
                        researchDictionary()->getResearchCost(researchItem);
                    research[researchItem] = ([
                        "when research began":time(),
                            "when research complete" : time(),
                            "time spent learning" : 0,
                            "research complete" : 1
                    ]);
                    ret = 1;

                    object events = getService("events");
                    if (events && objectp(events))
                    {
                        events->notify("onResearchCompleted", researchItem);
                    }
                }
                break;
            }
            case "timed":
            {
                research[researchItem] = ([
                    "when research began": time(),
                    "time to complete learning": 
                        researchDictionary()->getResearchCost(researchItem),
                    "time spent learning": 0,
                    "research complete": 0
                ]);
                ret = 1;
                
                object events = getService("events");
                if(events && objectp(events))
                {
                    events->notify("onResearchStarted", researchItem);
                }                 
                break;
            }
            case "granted":
            case "tree root":
            {
                research[researchItem] = ([
                    "when research began": time(),
                    "when research complete": time(),
                    "time spent learning": 0,
                    "research complete": 1
                ]);
                ret = 1;
                
                object events = getService("events");
                if(events && objectp(events))
                {
                    events->notify("onResearchCompleted", researchItem);
                }                   
                break;
            }              
            default:
            {
                // Do nothing
            }
        }
    }
    if (ret)
    {
        registerResearchEvents();
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int addResearchChoice(mapping researchChoice)
{
    int ret = researchChoice && mappingp(researchChoice) &&
        member(researchChoice, "name") &&
        stringp(researchChoice["name"]) &&
        member(researchChoice, "description") &&
        stringp(researchChoice["description"]) &&
        member(researchChoice, "research objects") &&
        pointerp(researchChoice["research objects"]);

    if (ret)
    {
        string *availableChoices = researchChoice["research objects"] -
            m_indices(research);

        int selection = 1;
        mapping choices = ([]);
        foreach(string researchItem in availableChoices)
        {
            if(canResearch(researchItem))
            {
                object researchObj = researchDictionary()->researchObject(researchItem);
                if(researchObj)
                {
                    choices[to_string(selection)] = ([
                        "choice": researchChoice["name"],
                        "type": "research object",
                        "name": researchObj->query("name"),
                        "description": researchObj->query("description"),
                        "key": researchItem
                    ]);
                    selection++;
                }
            }
            else if(researchDictionary()->researchTree(researchItem))
            {
                object researchTree = researchDictionary()->researchTree(researchItem);
                choices[to_string(selection)] = ([
                    "choice": researchChoice["name"],
                    "type": "research tree",
                    "name": researchTree->Name(),
                    "description": researchTree->Description(),
                    "key": researchItem
                ]);
                selection++;                
            }
            else
            {
                ret = 0;
            }
        }

        if (ret && sizeof(availableChoices))
        {
            researchChoices[researchChoice["name"]] = choices;

            object events = getService("events");
            if (events && objectp(events))
            {
                events->notify("onResearchChoiceAvailable", choices);
            }
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private void processResearchChoice(string researchItem, string choice, string event)
{
    m_delete(researchChoices, choice);

    object events = getService("events");
    if (events && objectp(events))
    {
        events->notify(event, researchItem);
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask int selectResearchChoice(string researchItem, string choice)
{
    int ret = member(researchChoices, choice) &&
        initiateResearch(researchItem);

    if(ret)
    {
        processResearchChoice(researchItem, choice, "onResearchChoiceChosen");
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int addResearchTree(string researchTree)
{
    int ret = 0;

    object treeObj = researchDictionary()->researchTree(researchTree);
    if(treeObj && (member(openResearchTrees, researchTree) < 0))
    {
        ret = treeObj->checkPrerequisites(this_object()) && treeObj->TreeRoot()
            && researchDictionary()->checkResearchPrerequisites(treeObj->TreeRoot(),
            this_object()) && initiateResearch(treeObj->TreeRoot());
            
        if(ret)
        {
            openResearchTrees += ({ researchTree });
            
            object events = getService("events");
            if(events && objectp(events))
            {
                events->notify("onResearchTreeOpen");
            }               
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int selectResearchPath(string researchTree, string choice)
{
    int ret = member(researchChoices, choice) && 
        addResearchTree(researchTree);

    if (ret)
    {
        processResearchChoice(researchTree, choice, "onResearchPathChosen");
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask mapping *researchExtraAttacks()
{
    mapping *extraAttacks = ({ });
    string *researchItems = m_indices(research);
    foreach(string researchItem in researchItems)
    {
        if(canApplyResearchBonus(researchItem))
        {
            extraAttacks += researchDictionary()->extraAttacks(researchItem,
                this_object());
        }
    }
    
    // Strip out any potential null elements
    extraAttacks -= ({ 0 });
    
    return extraAttacks + ({ });
}

/////////////////////////////////////////////////////////////////////////////
public nomask int researchAttributeBonus(string attribute)
{
    int ret = 0;
    
    if(attribute && stringp(attribute))
    {
        string *researchItems = m_indices(research);
        foreach(string researchItem in researchItems)
        {
            if(canApplyResearchBonus(researchItem, attribute))
            {
                string method = sprintf("%sBonus", capitalize(attribute));
                if(function_exists(method, researchDictionary()))
                {
                    ret = call_other(researchDictionary(), method, 
                        researchItem);
                }
            }
        }    
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int researchBonusTo(string bonus)
{
    int ret = 0;
    
    if(function_exists(bonus, researchDictionary()))
    {
        string *researchItems = m_indices(research);
        foreach(string researchItem in researchItems)
        {
            if(canApplyResearchBonus(researchItem, bonus))
            {
                ret += call_other(researchDictionary(), bonus, researchItem);
                
                if(researchDictionary()->isSustainedAbility(researchItem) &&
                   (member(({ "MaxHitPoints", "MaxSpellPoints", 
                              "MaxStaminaPoints" }), bonus) > -1))
                {
                    ret -= call_other(researchDictionary(), 
                        "applySustainedCostTo", researchItem, bonus);
                }                
            }
        }
    }
    else if(function_exists("BonusSkillModifier", researchDictionary()))
    {
        string *researchItems = m_indices(research);
        foreach(string researchItem in researchItems)
        {
            if(researchDictionary()->researchEffectIsLimited(researchItem) &&
               canApplyResearchBonus(researchItem, bonus))
            {
                ret += call_other(researchDictionary(), "LimitedSkillModifier",
                    researchItem, bonus, this_object(), 
                    (function_exists("getTargetToAttack", this_object()) ? 
                    this_object()->getTargetToAttack() : 0));
            }        
            else if(canApplyResearchBonus(researchItem, bonus))
            {
                ret += call_other(researchDictionary(), "BonusSkillModifier", 
                    researchItem, bonus);
            }           
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int researchCommand(string command)
{
    int ret = 0;
    string commandToExecute = 0;
    string customEvent = 0;
    object researchObj = 0;

    string *researchAbilities = activeAndSustainedResearchAbilities();
    foreach(string researchItem in researchAbilities)
    {
        researchObj = researchDictionary()->researchObject(researchItem);
        
        if(researchObj)
        {
            string commandTemplate = researchObj->commandRegexp();
            customEvent = researchObj->query("event handler");
            if(commandTemplate && stringp(commandTemplate))
            {
                if ((researchObj->query("scope") == "targetted") &&
                    !sizeof(regexp(({ command }), "at [A-Za-z]+")))
                {
                    command += " at " + this_object()->Name();
                }

                string *checkCommand = regexp(({ command }), commandTemplate);
                if(checkCommand && sizeof(checkCommand))
                {
                    commandToExecute = researchItem;
                    break;
                }
                else
                {
                    commandToExecute = 0;
                }                
            }
        }
    }
    if(commandToExecute && canApplyLimitedEffect(commandToExecute))
    {
        ret = researchDictionary()->researchCommand(commandToExecute,
            command, this_object());

        if (ret && researchObj && (researchObj->query("type") != "sustained") &&
            researchObj->query("cooldown"))
        {
            research[commandToExecute]["cooldown"] = researchObj->query("cooldown");
        }

        object eventObj = getService("events");
        if (ret && customEvent && eventObj && objectp(eventObj))
        {
            eventObj->notify(customEvent);
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
static nomask void researchHeartBeat()
{
    // Only increment the time spent / decrement usage counters
    // when not idle.
    if(query_idle(this_object()) < 60)
    {
        string *listOfTimedResearch = researchInProgress();

        foreach(string researchItem in listOfTimedResearch)
        {
            if (researchItem && stringp(researchItem))
            {
                if (!member(research[researchItem], "research complete") ||
                    !research[researchItem]["research complete"])
                {
                    research[researchItem]["time spent learning"] += 2;
                }

                if (research[researchItem]["time to complete learning"] <=
                    research[researchItem]["time spent learning"])
                {
                    research[researchItem]["when research complete"] = time();
                    research[researchItem]["research complete"] = 1;

                    object events = getService("events");
                    if (events && objectp(events))
                    {
                        registerResearchEvents();
                        events->notify("onResearchCompleted", researchItem);
                    }
                }
            }
        }

        string *listOfBlockedResearch = 
            filter_array(m_indices(research), #'blockedByCooldown);

        foreach(string researchItem in listOfBlockedResearch)
        {
            if (researchItem && stringp(researchItem))
            {
                if(member(research[researchItem], "active count") &&
                        (research[researchItem]["active count"] > 0))
                {
                    research[researchItem]["active count"] -= 2;
                    if(research[researchItem]["active count"]  < 1)
                    {
                        m_delete(research[researchItem], "active count");
                    }
                }
                
                if (member(research[researchItem], "cooldown"))
                {
                    research[researchItem]["cooldown"] -= 2;
                    if (!blockedByCooldown(researchItem))
                    {
                        m_delete(research[researchItem], "cooldown");
                    }
                }
            }
        }
    }
}
