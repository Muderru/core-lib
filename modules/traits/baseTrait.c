//*****************************************************************************
// Class: baseTrait
// File Name: baseTrait.c
//
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//
// Description: TBD
//
//*****************************************************************************
virtual inherit "/lib/core/specification.c";
virtual inherit "/lib/core/prerequisites.c";
    
private string TraitItemLocation = "lib/modules/traits";

/////////////////////////////////////////////////////////////////////////////
public nomask int isValidTrait()
{
    // The researchData element is an artifact of /lib/core/specification.
    // All items that use research items check isValidTrait. Trying to
    // circumvent addSpecification won't work particularly well given that
    // the inherit_list and this method are called in unison.
    return member(researchData, "type");
}

/////////////////////////////////////////////////////////////////////////////
protected int addSpecification(string type, mixed value)
{
    int ret = 0;
 
    string bonusToCheck;
    int applyModifier = 1;

    if (sscanf(type, "penalty to %s", bonusToCheck))
    {
        type = sprintf("bonus %s", bonusToCheck);
        applyModifier = -1;
    }

    if (sscanf(type, "bonus %s", bonusToCheck))
    {
        if (getDictionary("bonuses") &&
            getDictionary("bonuses")->isValidBonusModifier(bonusToCheck, value))
        {
            researchData[type] = value * applyModifier;
            ret = 1;
        }
        else if (getDictionary("bonuses"))
        {
            raise_error(sprintf("ERROR - trait: the '%s' "
                "specification must be a valid modifier as defined in %s\n",
                type, program_name(getDictionary("bonuses"))));
        }
    }
    else if(type && stringp(type))
    {
        switch(type)
        {
            case "type":
            {
                if (value && stringp(value) && getDictionary("traits") &&
                    getDictionary("traits")->isValidTraitType(value))
                {
                    ret = 1;
                    researchData[type] = value;
                }
                else
                {
                    raise_error(sprintf("ERROR - trait: '%s' is "
                        "not a valid trait type.\n", to_string(value)));
                }
                break;
            }
            case "root":
            case "opposing root":
            {
                if(value && stringp(value))
                {
                    ret = 1;
                    researchData[type] = value;
                }
                else
                {
                    raise_error(sprintf("ERROR - trait: The '%s' value "
                        "must be a string.\n", type));
                }
                break;
            }
            case "opinion":
            case "opposing opinion":
            case "cost":
            {
                if (value && intp(value))
                {
                    ret = 1;
                    researchData[type] = value;
                }
                else
                {
                    raise_error(sprintf("ERROR - trait: The '%s' value "
                        "must be an integer.\n", type));
                }
                break;
            }
            case "research tree":
            {
                if(getDictionary("research") &&
                    getDictionary("research")->researchTree(value))
                {
                    ret = 1;
                    researchData[type] = value;
                }
                else
                {
                    raise_error(sprintf("ERROR - trait: The '%s' value "
                        "must be a valid research tree.\n", type));
                }
                break;
            }
            default:
            {
                ret = specification::addSpecification(type, value);
            }
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int isBonusAttack(string bonusItem)
{
    int ret = 0;
    string attackType = 0;
    if(bonusItem && stringp(bonusItem) && member(researchData, bonusItem) &&
       sscanf(bonusItem, "bonus %s attack", attackType) && 
       getDictionary("attacks"))
    {
        ret = (getDictionary("attacks")->getAttack(attackType) != 0) || 
              (attackType == "weapon");
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask mapping *getExtraAttacks()
{
    mapping *ret = ({ });
    string *keys = filter_array(m_indices(researchData), #'isBonusAttack);
    if(keys)
    {
        foreach(string key in keys)
        {
            // attacksDictionary is verfied to exist via call to
            // isBonusAttack
            string attack = 0;
            if (key == "bonus weapon attack")
            {
                int numAttacks = researchData["bonus weapon attack"];
                for (int i = 0; i < numAttacks; i++)
                {
                    ret += ({ (["attack type":"weapon"]) });
                }
            }
            else if(sscanf(key, "bonus %s attack", attack) && researchData[key] &&
               intp(researchData[key]))
            {
                mapping attackMap =
                    getDictionary("attacks")->getAttackMapping(attack, 
                    researchData[key]);
                attackMap["to hit"] = 35;
                if(attackMap)
                {
                    ret += ({ attackMap });
                }
            }
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int queryBonus(string bonus)
{
    int ret =  0;
    string bonusToCheck;
    if(sscanf(bonus, "bonus %s", bonusToCheck))
    {
        if(getDictionary("bonuses") && objectp(getDictionary("bonuses")) &&
           getDictionary("bonuses")->isValidBonus(bonusToCheck) &&
           member(researchData, bonus))
        {
            ret = researchData[bonus];
        }
    }
    return ret;
}

