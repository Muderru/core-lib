//*****************************************************************************
// Class: combat
// File Name: combat.c
//
// Copyright (c) 2018 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
virtual inherit "/lib/core/thing.c";
#include "/lib/modules/secure/combat.h"

//-----------------------------------------------------------------------------
// Method: combatDelay
// Description: This property is used to determine if an interactive object can
//              attack. While it it greater than 0, this object will not
//              attack from within the attack() method.
//
// Parameters: delay - optional delay to add to the current combat delay
//
// Returns: the current combat delay.
//-----------------------------------------------------------------------------
public nomask varargs int combatDelay(int delay)
{
    if(delay > 0)
    {
        combatDelay += delay;
    }
    return combatDelay;
}

//-----------------------------------------------------------------------------
// Method: attackObject
// Description: This method returns the attacks dictionary - used for
//              determining damage attack types, messages, and so on.
//
// Returns: the attacks dictionary
//-----------------------------------------------------------------------------
private nomask object attackObject()
{
    return getDictionary("attacks");
}

//-----------------------------------------------------------------------------
// Method: spellAction
// Description: This property is used to determine if an interactive object can
//              execute a combat-related action such as combat, equipping /
//              unequipping items, and casting spells.
//
// Parameters: delay - optional delay to add to the current combat delay
//
// Returns: the current spell action delay.
//-----------------------------------------------------------------------------
public nomask varargs int spellAction(int delay)
{
    if(delay > 0)
    {
        spellAction += delay;
    }
    return spellAction;
}

//-----------------------------------------------------------------------------
// Method: validCombatModifiers
// Description: The valid* methods are admittedly a bit of a misnomer. The real
//              intent is to list all of the calculated bonuses used in combat
//              that are controlled by this object. It is only accessible
//              within the inheritance tree.
//
// Returns: the list of bonus methods controlled by this object
//-----------------------------------------------------------------------------
static nomask string *validCombatModifiers()
{
    return ({ "armor class", "defense class", "bonus attack", "soak",
        "encumberance", "skill penalty", "bonus defense", "haste", "slow",
        "enfeebled", "fortified", "poison", "paralysis", "disease",
        "bonus spell points", "bonus stamina points", "bonus heal hit points rate", 
        "bonus heal hit points", "bonus heal spell points", "bonus hit points",
        "bonus heal spell points rate", "bonus heal stamina",
        "bonus heal stamina rate", "damage reflection",
        "bonus defense class", "bonus damage", "bonus weapon attack",
        "bonus resist bludgeon", "bonus resist fire", "bonus resist evil", 
        "bonus resist neutral", "bonus resist physical", "bonus resist slash", 
        "bonus resist air", "bonus resist acid", "bonus resist disease", 
        "bonus resist good", "bonus resist magical", "bonus resist energy", 
        "bonus resist electricity", "bonus resist thrust", "bonus resist sonic", 
        "bonus resist chaos", "bonus resist earth", "bonus resist paralysis", 
        "bonus resist psionic", "bonus resist poison", "bonus resist undead", 
        "bonus resist water", "bonus resist cold" });
}

/////////////////////////////////////////////////////////////////////////////
public nomask string *combatEffectModifiers()
{
    return ({ "haste", "slow", "enfeebled", "fortified", "poison",
        "paralysis", "disease", "damage reflection" });
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isNegativeCombatModifier(string modifier)
{
    return member(({ "slow", "enfeebled", "poison", "paralysis",
        "disease" }), modifier) > -1;
}

//-----------------------------------------------------------------------------
// Method: combatNotification
// Description: This method is used to broadcast all combat-related events
//              as they occur. See events.c for details.
//
// Parameters: event - the event that has occured. 
//-----------------------------------------------------------------------------
private nomask varargs void combatNotification(string event, mixed message)
{
    object eventObj = getService("events");
    
    if(event && stringp(event) && eventObj && objectp(eventObj))
    {
        eventObj->notify(event, message);
    }
}

//-----------------------------------------------------------------------------
// Method: calculateServiceBonuses
// Description: This method will scan through the various non-inventory 
//              services that might have combat-related modifiers and applies
//              the relevant bonus. For example, where methodToCheck is set
//              to MaxHitPoints, this method calls through races, guilds, and
//              so on to determine the total bonus applied to the maxHitPoints
//              value.
//
// Parameters: methodToCheck - the type of bonus for which to check
//
// Returns: the total bonus for methodToCheck
//-----------------------------------------------------------------------------
private nomask int calculateServiceBonuses(string methodToCheck)
{
    int ret = 0;
    
    string *servicesToCheck = ({ "races", "guilds", "research", "traits",
                                 "biological", "background" });
    
    foreach(string serviceToCheck in servicesToCheck)
    {
        object service = getService(serviceToCheck);
        if(service)
        {
            ret += call_other(service, 
                   sprintf("%sBonusTo", serviceToCheck), methodToCheck);
        }
    }
    
    object room = environment(this_object());
    if(room && (member(inherit_list(room), "lib/environment/environment.c") > -1))
    {
        // this_object is passed as a type check - meaning that environmental
        // affects are not always applied to everything equally - there may
        // be a faction limitation, race, guild, or perhaps only non-NPCs are
        // given a benefit, monsters given a penalty, etc...
        ret += room->environmentalBonusTo(methodToCheck, this_object());
    }
    return ret;
}   

//-----------------------------------------------------------------------------
// Method: maxHitPoints
// Description: This method returns the effective total maximum hit points that
//              are available to this living creature. This includes the actual
//              maxHitPoints value plus any bonuses as applied through
//              inventory elements or ancillary things such as guilds, race,
//              traits, research, background, and biological influences.
//
// Returns: the total maximum number of hit points available to this object
//-----------------------------------------------------------------------------
public nomask int maxHitPoints()
{
    if (!maxHitPoints)
    {
        maxHitPoints = 30;
    }
    int ret = maxHitPoints;
    
    object inventory = getService("inventory");
    if(inventory)
    {
        ret += inventory->inventoryGetModifier("combatModifiers",
            "bonus hit points");
    }
    
    ret += calculateServiceBonuses("MaxHitPoints");
    
    object attributes = getService("attributes");
    if(attributes)
    {
        ret += attributes->Con() * 6;
    }
    
    // This case will never happen with properly cloned living things. However,
    // the improperly done is possible. This removes the chance for division by
    // zero.
    if (ret < 1)
    {
        ret = 1;
    }

    return ret;
}

//-----------------------------------------------------------------------------
// Method: hitPoints
// Description: This method returns the current hit points available to this
//              living object. If the optional increase parameter is passed,
//              the hitPoints will be increased by that amount and an
//              onHitPointsChanged event will be broadcast. It is important to
//              note that hit points can only be increased via this method and
//              all hit point decreases MUST be accomplished through the hit()
//              method as this applies resistences and the like - the only
//              exception to this rule is research items.
//
// Parameters: increase - the optional increase in hit points
//
// Returns: the total number of hit points available to this object
//-----------------------------------------------------------------------------
public nomask varargs int hitPoints(int increase)
{
    // hitPoints can ONLY be removed via the hit method
    if(intp(increase) && ((increase > 0) || (previous_object() && 
       (member(inherit_list(previous_object()), 
       "lib/modules/research/researchItem.c") > -1) && (abs(increase) <= hitPoints))))
    {
        hitPoints += increase;
        combatNotification("onHitPointsChanged");
    }

    if (increase && (hitPoints > maxHitPoints()))
    {
        hitPoints = maxHitPoints();
    }
    return hitPoints;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs string healthDescription()
{
    string ret = 0;
    if (hitPoints() < (maxHitPoints() / 16))
    {
        ret = "is knocking on death's door.";
    }
    else if (hitPoints() < (maxHitPoints() / 8))
    {
        ret = "is severely injured.";
    }
    else if (hitPoints() < (maxHitPoints() / 4))
    {
        ret = "has taken a beating.";
    }
    else if (hitPoints() < (maxHitPoints() / 2))
    {
        ret = "has been roughed up.";
    }
    else if (hitPoints() < (3 * maxHitPoints() / 4))
    {
        ret = "has some minor injuries.";
    }
    else if (hitPoints() < (7 * maxHitPoints() / 8))
    {
        ret = "is barely inured.";
    }
    else
    {
        ret = "is in good shape.";
    }

    return ret;
}

//-----------------------------------------------------------------------------
// Method: maxSpellPoints
// Description: This method returns the effective total maximum spell points
//              that are available to this living creature. This includes the 
//              actual maxSpellPoints value plus any bonuses as applied
//              through inventory elements or ancillary things such as guilds,
//              race, traits, research, background, and biological influences.
//
// Returns: the total maximum number of spell points available to this object
//-----------------------------------------------------------------------------
public nomask int maxSpellPoints()
{
    if (!maxSpellPoints)
    {
        maxSpellPoints = 30;
    }
    int ret = maxSpellPoints;
    
    object inventory = getService("inventory");
    if(inventory)
    {
        ret += inventory->inventoryGetModifier("combatModifiers",
            "bonus spell points");
    }
    
    ret += calculateServiceBonuses("MaxSpellPoints");
    
    object attributes = getService("attributes");    
    if(attributes)
    {
        ret += (attributes->Int() * 3) + (attributes->Wis() * 3);
    }
    if (ret < 0)
    {
        ret = 0;
    }
    return ret;
}

//-----------------------------------------------------------------------------
// Method: spellPoints
// Description: This method returns the current spell points available to this
//              living object. If the optional increase parameter is passed,
//              the spellPoints will be modified by that amount and an
//              onSpellPointsChanged event will be broadcast.
//
// Parameters: increase - the optional change in spell points
//
// Returns: the total number of spell points available to this object
//-----------------------------------------------------------------------------
public nomask varargs int spellPoints(int increase)
{
    if(intp(increase) && (increase != 0))
    {
        spellPoints += increase;

        object inventory = getService("inventory");
        if(increase > 0)
        {
            if(inventory)
            {
                spellPoints += inventory->inventoryGetModifier("guildModifiers",
                    "bonus recover spell points");
            }
            
            spellPoints += calculateServiceBonuses("RecoverSpellPoints");
        }
        else
        {
            if(inventory)
            {
                spellPoints += inventory->inventoryGetModifier("guildModifiers",
                    "bonus reduce spell points");
            }
            
            spellPoints += calculateServiceBonuses("ReduceSpellPoints");
        }
        
        combatNotification("onSpellPointsChanged");
    }

    if(increase && (spellPoints > maxSpellPoints()))
    {
        spellPoints = maxSpellPoints();
    }
    else if (spellPoints < 0)
    {
        spellPoints = 0;
    }

    return spellPoints;
}

//-----------------------------------------------------------------------------
// Method: maxStaminaPoints
// Description: This method returns the effective total maximum stamina points
//              that are available to this living creature. This includes the 
//              actual maxStaminaPoints value plus any bonuses as applied
//              through inventory elements or ancillary things such as guilds,
//              race, traits, research, background, and biological influences.
//
// Returns: the total maximum number of stamina points available to this object
//-----------------------------------------------------------------------------
public nomask int maxStaminaPoints()
{
    if (!maxStaminaPoints)
    {
        maxStaminaPoints = 30;
    }
    int ret = maxStaminaPoints;
    
    object inventory = getService("inventory");
    if(inventory)
    {
        ret += inventory->inventoryGetModifier("combatModifiers",
            "bonus stamina points");
    }
    
    ret += calculateServiceBonuses("MaxStaminaPoints");

    object attributes = getService("attributes");  
    if(attributes)
    {
        ret += (attributes->Str() * 3) + (attributes->Con() * 3);
    }    

    if (ret < 0)
    {
        ret = 0;
    }
    return ret;
}

//-----------------------------------------------------------------------------
// Method: staminaPoints
// Description: This method returns the current stamina points available to
//              this living object. If the optional increase parameter is 
//              passed, the staminaPoints will be modified by that amount and
//              an onStaminaPointsChanged event will be broadcast.
//
// Parameters: increase - the optional change in stamina points
//
// Returns: the total number of stamina points available to this object
//-----------------------------------------------------------------------------
public nomask varargs int staminaPoints(int increase)
{
    if(intp(increase) && (increase != 0))
    {
        staminaPoints += increase;

        object inventory = getService("inventory");
        if(increase > 0)
        {
            if(inventory)
            {
                staminaPoints += inventory->inventoryGetModifier("guildModifiers",
                    "bonus recover stamina points");
            }
            
            staminaPoints += calculateServiceBonuses("RecoverStaminaPoints");
        }
        else
        {
            if(inventory)
            {
                staminaPoints += inventory->inventoryGetModifier("guildModifiers",
                    "bonus reduce stamina points");
            }
            
            staminaPoints += calculateServiceBonuses("ReduceStaminaPoints");
        }        
        combatNotification("onStaminaPointsChanged");       
    }
    if (increase && (staminaPoints > maxStaminaPoints()))
    {
        staminaPoints = maxStaminaPoints();
    }
    else if (staminaPoints < 0)
    {
        staminaPoints = 0;
    }
    return staminaPoints;
}

//-----------------------------------------------------------------------------
// Method: calculateDefendAttack
// Description: This monstrosity of a method will determine this object's
//              ability to defend against attacks that occur during combat. It
//              is not intended to handle the ability to resist an attack,
//              reduce damage, or things of that nature - nor is it used to
//              determine defense against spells and special guild/race/trait/
//              research abilities, but only determine the ability to avoid an
//              attack from landing. It does check inventory/equipment
//              modifiers and apply parry, shield, and equipment bonuses, any
//              penalties/bonuses to dodging that armor might incur, and apply
//              bonuses from guild, race, traits, research, etc. This method is
//              not shadowable or in any way overridable. In order to modify
//              this result via a shadow or some other method, the
//              magicalDefendAttackBonus() method must be either shadowed or
//              overridden. In all likelihood, this should never be needed as
//              the inventory options provide a more than sufficient means of
//              handling this and the shadow should ONLY be used if other 
//              ancillary activities are being done.
//
// Returns: the overall defend attack value
//-----------------------------------------------------------------------------
public nomask int calculateDefendAttack()
{
    int ret = 0;
    object inventory = getService("inventory");
    object skills = getService("skills");
    
    if(inventory)
    {
        ret += inventory->inventoryGetDefendAttackBonus() -
               inventory->inventoryGetEncumberance();

        object primary = inventory->equipmentInSlot("wielded primary");
        if (inventory->isEquipped(primary) && skills)
        {
            string weaponType = primary->query("weapon type");
            if (weaponType)
            {
                ret += (skills->getSkillModifier(weaponType) > 0) ? skills->getSkillModifier(weaponType) / 4 : 0;
            }
        }

        object offhand = inventory->equipmentInSlot("wielded offhand");
        if(inventory->isEquipped(offhand) && skills)
        {
            string weaponType = offhand->query("weapon type");
            // Give a bonus for one's shield skill if using a shield
            if(weaponType && (weaponType == "shield"))
            {
                ret += (skills->getSkillModifier(weaponType) > 0) ? skills->getSkillModifier(weaponType) : 0;
            }
            else if (weaponType)
            {
                ret += (skills->getSkillModifier(weaponType) > 0) ? skills->getSkillModifier(weaponType) / 4 : 0;
            }
        }
    
        // Apply the armor skill bonus
        object armor = inventory->equipmentInSlot("armor");
        if(armor && skills)
        {
            string armorType = armor->query("armor type");
            
            if(armorType && stringp(armorType))
            {
                ret += skills->getSkillModifier(armorType) -
                    armor->query("skill penalty");
            }            
        }
        
        if(inventory->inventoryGetModifier("combatModifiers", "disease"))
        {
            ret = to_int(ret * 0.9);
        }
    }
    
    ret += calculateServiceBonuses("DefendAttackBonus");
   
    object materialAttributes = getService("materialAttributes");
    if (materialAttributes && skills && !materialAttributes->canSee())
    {
        ret += skills->getSkillModifier("blind fighting") - 10;
    }
    object attributes = getService("attributes");
    if(attributes)
    {
        ret += (attributes->dexterityBonus() / 2) +
               (attributes->wisdomBonus() / 2);
    }
    
    ret += this_object()->magicalDefendAttackBonus();
    return ret;
}

//-----------------------------------------------------------------------------
// Method: calculateAttack
// Description: This monstrosity of a method will determine this object's
//              ability to successfully attack during combat. It is not 
//              intended to handle the ability to do damage during an attack,
//              increase damage, or things of that nature (those are handled in
//              calculateSoakDamage) - nor is it used to determine attack
//              for spells and special guild/race/trait/ research abilities,
//              but only determine the ability to land an attack. It does check
//              inventory/equipment modifiers and apply weapon skills and 
//              equipment bonuses, any penalties/bonuses that armor might 
//              incur, and apply bonuses from guild, race, traits, research.
//              It will decrease the attack by the passed in opponent's
//              calculateDefendAttack result. This method is not shadowable or
//              in any way overridable. In order to modify this result via a
//              shadow or some other method, the magicalAttackBonus() method
//              must be either shadowed or overridden. In all likelihood, this
//              should never be needed as the inventory options provide a more
//              than sufficient means of handling this and the shadow should
//              ONLY be used if other ancillary activities are being done.
//
// Parameters: attacker - this object's opponent.
//             weapon - the weapon used for this attack.
//
// Returns: the overall attack value
//-----------------------------------------------------------------------------
public nomask varargs int calculateAttack(object attacker, object weapon, int doNotRandomize) 
{
    int toHit = this_object()->magicalAttackBonus();

    if (!doNotRandomize)
    {
        toHit += random(101) - 25;
    }

    if(attacker && objectp(attacker) && 
       function_exists("calculateDefendAttack", attacker))
    {
        toHit -= attacker->calculateDefendAttack();
    }

    object inventory = getService("inventory");
    object skills = getService("skills");
    
    if(inventory)
    {
        toHit -= inventory->inventoryGetEncumberance();

        if(inventory->isEquipped(weapon))
        {
            toHit += inventory->inventoryGetAttackBonus(weapon);
        }
        else if(weapon && !inventory->isEquipment(weapon) &&
            attackObject()->isAttack(weapon))
        {
            // This is a special case - weapon is an AttackType
            toHit += weapon->attackTypeCalculateAttack();
        }
        
        if(inventory->inventoryGetModifier("combatModifiers", "disease"))
        {
            toHit = to_int(toHit * 0.9);
        } 
        if(inventory->inventoryGetModifier("combatModifiers", "enfeebled"))
        {
            toHit = to_int(toHit * 0.85);
        }
        if(inventory->inventoryGetModifier("combatModifiers", "fortified"))
        {
            toHit = to_int(toHit * 1.15);
        }        
    }
    
    toHit += calculateServiceBonuses("AttackBonus");

    object materialAttributes = getService("materialAttributes");
    if (materialAttributes && skills && !materialAttributes->canSee())
    {
        toHit += skills->getSkillModifier("blind fighting") - 10;
    }

    object attributes = getService("attributes");
    if(attributes)
    {
        toHit += (attributes->dexterityBonus() / 2) +
                 (attributes->intelligenceBonus() / 2);
    }

    return ((toHit < -100) ? -101 : toHit);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int calculateSoakDamage(string damageType)
{
   int ret = 0;
    object inventory = getService("inventory");
    
    if(inventory)
    {
        ret += inventory->inventoryGetDefenseBonus(damageType);
        
        if(inventory->inventoryGetModifier("combatModifiers", "paralysis"))
        {
            ret = to_int(ret * 0.9);
        }        
    }
    
    ret += calculateServiceBonuses("DefenseBonus");
    
    object attributes = getService("attributes");
    if(attributes)
    {
        ret += (attributes->constitutionBonus() / 2) +
               (attributes->strengthBonus() / 2);
    }
    
    ret += this_object()->magicalDefenseBonus();
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs int calculateDamage(object weapon, string damageType, int doNotRandomize)
{
    int ret = 0;
    
    object inventory = getService("inventory");
    
    if (attackObject()->isAttack(weapon))
    {
        ret += weapon->attackTypeCalculateDamage(this_object());
    }

    if(inventory)
    {
        ret += inventory->inventoryGetDamageBonus(weapon, damageType);
        
        if(inventory->inventoryGetModifier("combatModifiers", "enfeebled"))
        {
            ret = to_int(ret * 0.75);
        }
        if(inventory->inventoryGetModifier("combatModifiers", "fortified"))
        {
            ret = to_int(ret * 1.25);
        }        
    }
    
    ret += calculateServiceBonuses("DamageBonus");
    
    object attributes = getService("attributes");
    if(attributes)
    {
        if ((damageType == "physical") || attackObject()->isAttack(weapon))
        {
            ret += attributes->strengthBonus() / 2;
        }
        if(ret)
        {
            ret += (attributes->intelligenceBonus() / 4) +
                (attributes->wisdomBonus() / 4);
        }
    }
    
    ret += this_object()->magicalDamageBonus();

    if (!doNotRandomize)
    {
        ret += to_int((ret / 8.0) - random(ret / 4));
    }

    if (ret < 0)
    {
        ret = 0;
    }

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public int magicalDefendAttackBonus()
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
public int magicalAttackBonus()
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
public int magicalDefenseBonus()
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
public int magicalDamageBonus()
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
private nomask void attackFumble()
{
    // Currently, we don't do this
}

/////////////////////////////////////////////////////////////////////////////
protected nomask float damageModifierFromAttack(int toHit)
{
    float retVal = 0.0;
    
    switch(toHit) 
    {
        // Really bad miss
        case -101: 
        {
            attackFumble();
            break;
        }
        // Missed
        case -100..10: 
        {
            retVal = 0.0;
            break;
        }

        // Standard hit
        case 11..95: 
        {
            retVal = 1.0;
            break;
        }

        // Slight critical
        case 96..135: 
        {
            retVal = 1.1;
            break;
        }

        // Moderate critical
        case 136..150: 
        {
            retVal = 1.25;
            break;
        }

        // Solid critical
        case 151..200: 
        {
            retVal = 1.5;
            break;
        }

        // Amazing attack
        default: 
        {
            retVal = 2.0;
            break;
        }
    }
    return retVal;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs int addAttack(string attackToAdd, int damage, int toHit)
{
    int ret = 0;

    if(attackToAdd && damage)
    {
        mapping newAttack = ([
            "attack type": attackToAdd,
            "to hit": toHit,
            "damage": damage
        ]);
        
        if(attackObject()->isValidAttack(newAttack))
        {
            attacks += ({ newAttack });
            ret = 1;
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask void clearAttacks()
{
    attacks = ({ });
}

/////////////////////////////////////////////////////////////////////////////
private nomask varargs mapping *getWeaponAttacksFromBonus(int numAttacks, int addOffhand)
{
    mapping *attacksToReturn = ({});

    object inventory = getService("inventory");
    if (inventory)
    {
        int hasOffhandWeapon = objectp(inventory->equipmentInSlot("wielded offhand"));

        for (int i = 0; i < numAttacks; i++)
        {
            attacksToReturn += ({ (["attack type":"wielded primary"]) });

            if (hasOffhandWeapon && ((i % 2) || addOffhand))
            {
                attacksToReturn += ({ (["attack type":"wielded offhand"]) });
            }
        }
    }
    return attacksToReturn;
}

/////////////////////////////////////////////////////////////////////////////
public nomask mapping *getAttacks()
{
    mapping *attacksToReturn = attacks;

    object inventory = getService("inventory");
    if (inventory)
    {
        if (inventory->equipmentInSlot("wielded primary"))
        {
            attacksToReturn += ({ (["attack type":"wielded primary"]) });
        }

        object offhand = inventory->equipmentInSlot("wielded offhand");

        // Don't add attacks for defense-only shields.
        if (offhand && offhand->query("weapon class") &&
            (offhand != inventory->equipmentInSlot("wielded primary")))
        {
            attacksToReturn += ({ (["attack type":"wielded offhand"]) });
        }
    }

    if (!sizeof(attacksToReturn))
    {
        attacksToReturn += ({ ([
            "attack type":"unarmed",
            "to hit" : 50,
            "damage" : 10
        ]) });
    }

    // Add attacks from external sources
    string *servicesToCheck = ({ "races", "guilds", "research", "traits", "background" });
    int addOffhandWeapon = 0;

    foreach(string serviceToCheck in servicesToCheck)
    {
        object service = getService(serviceToCheck);
        if(service)
        {
            mapping *extraAttacks = 
                call_other(service, sprintf("%sExtraAttacks", serviceToCheck));
            
            if (sizeof(extraAttacks))
            {
                foreach(mapping extraAttack in extraAttacks)
                {
                    if (attackObject()->isWeaponAttack(extraAttack))
                    {
                        attacksToReturn += getWeaponAttacksFromBonus(1, addOffhandWeapon % 2);
                        addOffhandWeapon++;
                    }
                    else if (attackObject()->isValidAttack(extraAttack))
                    {
                        attacksToReturn += ({ extraAttack });
                    }
                }
            }
        }
    }

    object *modifiers = inventory->registeredInventoryObjects();

    if (modifiers && pointerp(modifiers) && sizeof(modifiers))
    {
        object attacksDictionary = getDictionary("attacks");
        if (attacksDictionary && function_exists("validAttackTypes",
            attacksDictionary))
        {
            string *attackList = attacksDictionary->validAttackTypes();
                
            foreach(object modifier in modifiers)
            {
                foreach(string attack in attackList)
                {
                    if (modifier->query(sprintf("bonus %s attack", attack)))
                        attacksToReturn += ({ (["attack type":attack,
                            "to hit" : 50 + modifier->query(sprintf("bonus %s attack", attack)),
                            "damage" : modifier->query(sprintf("bonus %s attack", attack))
                    ]) });
                }
                if (modifier->query("bonus weapon attack") && inventory->equipmentInSlot("wielded primary"))
                {
                    int numAttacks = modifier->query("bonus weapon attack");
                    attacksToReturn += getWeaponAttacksFromBonus(numAttacks);
                }
            }
        }
    }
    
    return attacksToReturn + ({ });
}

/////////////////////////////////////////////////////////////////////////////
public nomask object getTargetToAttack()
{
    object attacker = 0;
    
    object *listOfPotentialAttackers = sort_array(filter(m_indices(hostileList),
            (: present($1, environment(this_object())) :)),
            (: hostileList[$1]["time"] > hostileList[$2]["time"] :));
    
    int numPotentialAttackers = sizeof(listOfPotentialAttackers);
    if(numPotentialAttackers)
    {        
        attacker = listOfPotentialAttackers[0];
        if(attacker && objectp(attacker) && 
          ((random(101) + attacker->calculateDefendAttack()) > 50))
        {
            attacker = listOfPotentialAttackers[random(numPotentialAttackers)];
        }
    }
    return attacker;    
}

/////////////////////////////////////////////////////////////////////////////
public nomask string getHostileList()
{
    string ret = "Nothing at all, aren't you lucky?";

    object *attackers = m_indices(hostileList);
    if(sizeof(attackers))
    {
        ret = "";
        foreach(object attacker in attackers)
        {
            if (ret != "")
            {
                ret += ", ";
            }
            ret += capitalize(attacker->RealName());
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int unregisterAttacker(object attacker)
{
    int ret = 0;
    if (member(hostileList, attacker))
    {
        ret = 1;
        m_delete(hostileList, attacker);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask void stopFight(object attacker)
{
    if (attacker && objectp(attacker))
    {
        unregisterAttacker(attacker);
        attacker->unregisterAttacker(this_object());
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask void toggleKillList()
{
    object player = getService("player");
    if (player)
    {
        onKillList = !onKillList;
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask int onKillList()
{
    int ret = 1;

    object player = getService("player");
    if (player)
    {
        ret = onKillList;
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int checkKillList(object foe)
{
    int ret = onKillList() && foe->onKillList();

    if (!ret)
    {
        printf("You're not allowed to attack players not on the list!\n");
        foe->stopFight(this_object());
        stopFight(foe);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask varargs int hitIsAllowed(object foe)
{
    int ret = living(this_object()) && environment() &&
        !this_object()->isDead() && !this_object()->isInvulnerable() &&
        !environment()->violenceIsProhibited() && 
        (!foe || present(foe, environment()));

    object player = getService("player");
    if (player)
    {
        //        ret &&= query_ip_number(this_object());
    }
    if (ret && foe && objectp(foe))
    {
        if (function_exists("isDead", foe))
        {
            ret &&= !foe->isDead();
        }
        if (function_exists("isNPC", foe) &&
            !foe->isNPC())
        {
            ret &&= checkKillList(foe);
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isInCombatWith(object attacker)
{
    return member(hostileList, attacker);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int registerAttacker(object attacker)
{
    int ret = hitIsAllowed(attacker);
    if (ret && !member(hostileList, attacker) && attacker->has("combat"))
    {
        hostileList[attacker] = (["time":time()]);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int abortCombat(object attacker)
{
    int ret = !hitIsAllowed(attacker) || spellAction();
    
    if(combatDelay)
    {
        combatNotification("onCombatDelayed");
        combatDelay--;
        ret = 1;
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public int isDead()
{
    int ret = 0;

    object materialAttributes = getService("materialAttributes");
    if (materialAttributes)
    {
        ret = materialAttributes->Ghost();
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask void generateCombatStatistics(object foe)
{
    if(foe && function_exists("has", foe) &&
        foe->has("materialAttributes") && getService("player") &&
        function_exists("saveCombatStatistics", this_object()))
    {
        getService("player")->saveCombatStatistics(this_object(), foe);
    }            
}

/////////////////////////////////////////////////////////////////////////////
private nomask void generateCorpse(object murderer)
{
    object materialAttributes = getService("materialAttributes");
    if(materialAttributes)
    {
        materialAttributes->Ghost(1);
        object corpse = clone_object("/lib/items/corpse.c");
        if(corpse)
        {
            corpse->corpseSetup(this_object());

            if(function_exists("isRealizationOfPlayer", murderer) &&
               function_exists("isRealizationOfPlayer", this_object()))
            {
                corpse->set("killed by player", murderer->RealName());
            }

            move_object(corpse, environment(this_object()));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
private nomask void updateFactionDispositionsFromCombat(object murderer)
{
    object factionService = getService("factions");
    if (factionService && murderer && objectp(murderer))
    {
        int reputationPenalty;
        if (function_exists("effectiveLevel", this_object()))
        {
            int levelDisparity = this_object()->effectiveLevel() -
                murderer->effectiveLevel();

            reputationPenalty = 0 - ((levelDisparity > 1) ? levelDisparity : 1);
        }
        else
        {
            reputationPenalty = -1;
        }

        string *factionList = factionService->Factions();

        foreach(string faction in factionList)
        {
            murderer->updateFactionDisposition(faction, reputationPenalty, 1);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
private nomask int finishOffThisPoorDeadBastard(object murderer)
{
    int ret = 1;

    object player = getService("player");
    if(player)
    {
        object persistence = getService("secure/persistence");
        if (persistence)
        {
            persistence->save();
        }
        object logger = getDictionary("log");
        if(logger)
        {
            logger->logDeath(player, murderer);
        }
    }
    
    if(murderer && function_exists("generateCombatStatistics", murderer))
    {
        murderer->generateCombatStatistics(this_object());
    }
    generateCorpse(murderer);
    
    if(function_exists("secondLife", this_object()))
    {
        ret = !this_object()->secondLife();
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int determineFateFromDeath(object murderer)
{
    int killMe = 0;
    
    if(!isDead())
    {
        object wizard = getService("wizard");
        if(wizard)
        {
            tell_object(wizard, "Your wizardhood protects you from death.\n");
        }
        else if(environment() && 
                function_exists("suppressDeath", environment()))
        {
            object player = getService("player");
            string notDeadMessage = environment()->suppressDeath();
            if(player && notDeadMessage)
            {
                tell_object(this_object(), notDeadMessage);
            }
            else
            {
                // Room didn't save this poor sot.
                killMe = 1;
            }
        }
        else
        {
            killMe = 1;
        }
    }
    
    if(killMe)
    {
        combatNotification("onDeath");
        updateFactionDispositionsFromCombat(murderer);
        killMe = finishOffThisPoorDeadBastard(murderer);
    }
    return killMe;
}
  
/////////////////////////////////////////////////////////////////////////////
private nomask int canBeHitIfEthereal(string damageType)
{
    int ret = 1;

    object traits = getService("traits");
    if (traits && traits->hasTraitOfRoot("ethereal") &&
        (member(({ "electricity", "energy", "evil", "good", "fire",
            "magical" }), damageType) < 0))
    {
        ret = 0;
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int calculateDamageResistance(int damage, string damageType)
{
    return to_int(damage *
        calculateServiceBonuses(sprintf("resist %s", damageType)) / 100.0);
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs int hit(int damage, string damageType, object foe)
{
    int ret = 0;

    if(!damageType)
    {
        damageType = "physical";
    }

    if(hitIsAllowed() && canBeHitIfEthereal(damageType))
    {
        if(!foe || !objectp(foe))
        {
            foe = previous_object();
        }
        registerAttacker(foe);
        // Make sure there's a heartbeat so that this can attack back
        set_heart_beat(1);

        int resistance = calculateDamageResistance(damage, damageType);
        ret = damage - calculateSoakDamage(damageType) - resistance;

        if(ret < 0)
        {
            ret = 0;
        }
        
        if(ret > hitPoints)
        {
            ret = hitPoints;
        }

        hitPoints -= ret;
        combatNotification("onHit", ([ "type": damageType, 
                                       "damage": damage ]));
  
        if(hitPoints() <= 0)
        {
            determineFateFromDeath(foe);
        }
        
        if(foe && function_exists("addExperience", foe) &&
            function_exists("effectiveExperience", this_object()))
        {
            float levelModifier = to_float(this_object()->effectiveLevel()) /
                (foe->effectiveLevel() * 200.0);

            float expToGive = this_object()->effectiveExperience() * 
                (to_float(ret) / to_float(maxHitPoints())) * levelModifier;
            foe->addExperience(to_int(expToGive));
        }

        object inventory = getService("inventory");        
        if(foe && inventory)
        {
            int reflection = inventory->inventoryGetModifier("combatModifiers", 
                "damage reflection");
                
            if(reflection)
            {
                reflection = to_int(
                    (((reflection > 50) ? 50 : reflection) / 100.0) * damage);

                if (reflection > 1)
                {
                    object victim = getTargetToAttack();
                    if (objectp(victim))
                    {
                        victim->hit(reflection, damageType, this_object());
                        attackObject()->displayMessage(this_object(), victim,
                            damage, attackObject()->getAttack("reflection"));
                    }
                }
            }
        }
    }
    if (isDead())
    {
        say(sprintf("%s died.\n", capitalize(this_object()->RealName())));
        getDictionary("combatChatter")->displayCombatChatter(ret,
            foe, this_object());

        if (!getService("player"))
        {
            destruct(this_object());
        }
    }

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
protected nomask void doOneAttack(object foe, object weapon)
{
    int toHit = calculateAttack(foe, weapon);
    int damageInflicted = 0;
    int damage = 0;
    string primaryDamageType = "physical";
    float hitSucceeded = damageModifierFromAttack(toHit);
    
    if(weapon && objectp(weapon) && foe && objectp(foe))
    {
        if(weapon->getDamageType())
        {
            primaryDamageType = weapon->getDamageType();
        }

        if (foe->hasTraitOfRoot("ethereal") && !weapon->canDamageEthereal())
        {
            hitSucceeded = 0.0;
        }

        if((hitSucceeded > 0.0) && foe && objectp(foe))
        {
            object inventory = getService("inventory");
            if(inventory && weapon && objectp(weapon) &&
                (inventory->isEquipped(weapon) || weapon->getDamageType()))
            {
                string *extraDmg = inventory->getExtraDamageTypes(weapon);
                if(extraDmg)
                {
                    foreach(string damageType in extraDmg)
                    {
                        damage = calculateDamage(weapon, damageType);

                        // foe can die / be destructed at any time - need to verify that it
                        // still exists
                        if(foe && objectp(foe))
                        {
                            damageInflicted += foe->hit(damage, damageType, this_object());
                        }
                    }
                }
            }

            damage = to_int(calculateDamage(weapon, primaryDamageType) * hitSucceeded);

            string extraHitFunction = weapon->query("hit method");
            if(stringp(extraHitFunction) && function_exists(extraHitFunction, weapon))
            {
                int extraDamage = call_other(weapon, extraHitFunction);
                if(intp(extraDamage))
                {
                    damage += extraDamage;
                }
            }

            // foe can die / be destructed at any time - need to verify that it
            // still exists
            if(foe && objectp(foe))
            {
                damageInflicted += foe->hit(damage, primaryDamageType, this_object());
            }
        }

        if(foe && objectp(foe))
        {
            getDictionary("combatChatter")->displayCombatChatter(damageInflicted, 
                this_object(), foe);

            attackObject()->displayMessage(this_object(), foe, damageInflicted,
                weapon);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask int attack(object foe)
{
    int ret = 0;
    if(abortCombat(foe))
    {
        if(foe)
        {
            stopFight(foe);
        }
    }
    else if(foe && objectp(foe))
    {
        foe->registerAttacker(this_object());
        registerAttacker(foe);

        set_heart_beat(1);
        combatNotification("onAttack");
        if(function_exists("notify", foe))
        {
            call_other(foe, "notify", "onAttacked");
        }

        ret = 1;
        foreach(mapping attack in getAttacks())
        {
            if(attackObject()->isWeaponAttack(attack))
            {
                object inventory = getService("inventory");
                if(inventory)
                {
                    object weapon = 
                        inventory->equipmentInSlot(attack["attack type"]);
                    if(!weapon)
                    {
                        weapon = attackObject()->getAttack("unarmed");
                    }
                    
                    doOneAttack(foe, weapon);
                }                   
            }
            else if(attackObject()->isValidAttack(attack))
            {
                object weapon = attackObject()->getAttack(
                    attack["attack type"]);
                
                weapon->setAttackValues(attack["damage"], attack["to hit"]);                
                doOneAttack(foe, weapon);
            }
        }

        object persona = getService("personas");
        if (objectp(persona) && objectp(foe))
        {
            persona->executePersonaResearch(foe->RealName());
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
static void handleMoveFromCombat()
{
    spellAction(1);

    if (getTargetToAttack())
    {
        tell_object(this_object(), 
            "You have fled the scene of battle! Your enemies shan't forget you.\n");
    }
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs int Wimpy(string newWimpyVal, int suppressDisplay)
{
    if(!suppressDisplay && (!newWimpyVal || !stringp(newWimpyVal)))
    {
        printf("Wimpy: %d%%\n", wimpy);
    }
    else if (!suppressDisplay)
    {
        // Alas, to_int will return 0 if a string w/o an int is passed.
        wimpy = to_int(newWimpyVal);

        if(wimpy > 70)
        {
            wimpy = 70;
            printf("Valid wimpy values range from 0-70. ");
        }
        else if(wimpy < 0)
        {
            wimpy = 0;
            printf("Valid wimpy values range from 0-70. ");
        }
        printf("Wimpy set to: %d%%\n", wimpy);
    }
    return wimpy;
}

/////////////////////////////////////////////////////////////////////////////
static nomask int triggerWimpy()
{
    int ret = hitPoints() < (maxHitPoints() * (wimpy / 100.0));
    
    if(ret)
    {
        combatNotification("onRunAway");
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int calculateVitalsHealRate(string vital)
{
    int ret = 0;
    object attributes = getService("attributes");
    
    if(vital && stringp(vital) && attributes)
    {
        switch(vital)
        {
            case "hit points":
            {
                ret = 1 + (attributes->Con() / 10) +
                       calculateServiceBonuses("BonusHealHitPoints");
                break;
            }
            case "spell points":
            {
                ret = 1 + ((attributes->Int() + attributes->Wis()) / 12) +
                      calculateServiceBonuses("BonusHealSpellPoints");
                break;
            }
            case "stamina":
            {
                ret = 1 + (attributes->Con() / 10) + (attributes->Dex() / 15) + 
                      calculateServiceBonuses("BonusHealStamina");
                break;
            }
        }
        
        object inventory = getService("inventory");
        if(inventory)
        {
            ret += inventory->inventoryGetModifier("combatModifiers",
                sprintf("bonus heal %s", vital));
        }  
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int calculateTimeToNextVitalsHeal(string vital)
{
    int ret = IntervalBetweenHealing;
    object attributes = getService("attributes");
    
    if(vital && stringp(vital) && attributes)
    {
        switch(vital)
        {
            case "hit points":
            {
                ret -= calculateServiceBonuses("BonusHealHitPointsRate");
                break;
            }
            case "spell points":
            {
                ret -= calculateServiceBonuses("BonusHealSpellPointsRate");
                break;
            }
            case "stamina":
            {
                ret -= calculateServiceBonuses("BonusHealStaminaRate");
                break;
            }
        }
        
        object inventory = getService("inventory");
        if(inventory)
        {
            ret -= inventory->inventoryGetModifier("combatModifiers",
                sprintf("bonus heal %s rate", vital));
        }  
    }
    if (ret < 4)
    {
        ret = 4;
    }
    
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
static nomask void combatHeartBeat()
{
    if(spellAction > 0)
    {
        spellAction--;
    }

    object factions = getService("factions");
    if(factions)
    {
        object *foes = factions->getAggressive(environment());
        if (foes)
        {
            foreach(object foe in foes)
            {
                if (foe && !member(hostileList, foe))
                {
                    hostileList[foe] = (["time":time()]);
                }
            }
        }
    }
    
    object attacker = getTargetToAttack();
    if(attacker)
    {
        object chat = getService("combatChatter");
        if(chat)
        {
            chat->combatChatterDisplayMessage();
        }
      
        object artificialIntelligence = getService("artificialIntelligence");
        if(artificialIntelligence)
        {
            artificialIntelligence->aiCombatAction(getTargetToAttack());
        }

        int numberAttackRounds = 1;
        object inventory = getService("inventory");
        if(inventory)
        {
            numberAttackRounds -= 
                inventory->inventoryGetModifier("combatModifiers", "slow") % 2;
            if(!WasSlowedLastRound && !numberAttackRounds)
            {
                WasSlowedLastRound = 1;
            }
            else
            {
                numberAttackRounds = 1;
                WasSlowedLastRound = 0;
            }

            numberAttackRounds +=
                inventory->inventoryGetModifier("combatModifiers", "haste") ? 1 : 0;
        }
        while(numberAttackRounds > 0)
        {
            attack(attacker);
            numberAttackRounds--;
        }

        object movement = getService("movement");

        if(attacker && present(attacker) && triggerWimpy() && movement)
        {
            movement->runAway();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
static nomask void healingHeartBeat()
{
    if((timeToHealHP <= 0) && (hitPoints() < maxHitPoints()))
    {
        hitPoints(calculateVitalsHealRate("hit points"));
        timeToHealHP = calculateTimeToNextVitalsHeal("hit points");
    }
    else
    {
        timeToHealHP -= 2;
    }

    if((timeToHealSP <= 0) && (spellPoints() < maxSpellPoints()))
    {
        spellPoints(calculateVitalsHealRate("spell points"));
        timeToHealSP = calculateTimeToNextVitalsHeal("spell points");
    }
    else
    {
        timeToHealSP -= 2;
    }

    if((timeToHealST <= 0) && (staminaPoints() < maxStaminaPoints()))
    {
        staminaPoints(calculateVitalsHealRate("stamina"));
        timeToHealST = calculateTimeToNextVitalsHeal("stamina");
    }
    else
    {
        timeToHealST -= 2;
    }    
}

/////////////////////////////////////////////////////////////////////////////
private nomask string vitalsDetails(string vital)
{
    string format = "\x1b[0;36m%12s:\x1b[0m \x1b[0;35;1m%s\x1b[0m ";
    int current = call_other(this_object(), lower_case(vital) + "Points");
    int max = call_other(this_object(), "max" + capitalize(vital) + "Points");

    if (current > max)
    {
        current = max;
    }

    string bar = "==========";
    if (!max)
    {
        bar = "\x1b[0m\x1b[0;31m//////////";
    }
    else
    {
        bar[(10 * current) / max..] = "\x1b[0m\x1b[0;31m";
        for (int i = ((10 * current) / max); i < 10; i++)
        {
            bar += ".";
        }
    }

    string extra = (vital != "Stamina") ? " Points" : "";

    return sprintf(format, vital + extra, bar);
}

/////////////////////////////////////////////////////////////////////////////
public nomask string vitals()
{
    return "\x1b[0;31m|\x1b[0m" + vitalsDetails("Hit") +
        " " + vitalsDetails("Spell") +
        " " + vitalsDetails("Stamina") + "\x1b[0;31m|\x1b[0m\n" +
        sprintf("\x1b[0;31m|\x1b[0m%13s \x1b[0;33m%-11s\x1b[0m %13s \x1b[0;33m%-11s\x1b[0m %13s \x1b[0;33m%-11s\x1b[0m\x1b[0;31m|\x1b[0m\n",
            "", sprintf("%d/%d", hitPoints(), maxHitPoints()),
            "", sprintf("%d/%d", spellPoints(), maxSpellPoints()),
            "", sprintf("%d/%d", staminaPoints(), maxStaminaPoints()));
}
