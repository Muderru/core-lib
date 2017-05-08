//*****************************************************************************
// Class: researchTree
// File Name: researchTree.c
//
// Copyright (c) 2017 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//
// Description: TBD
//
//*****************************************************************************
virtual inherit "/lib/core/prerequisites.c";

private string ResearchDictionary = "/lib/dictionaries/researchDictionary.c";

private string name;
private string description;
private string treeRoot;

private mapping tree = ([
]);

/////////////////////////////////////////////////////////////////////////////
public void init()
{
    name = "";
    description = "";
    treeRoot = "";
    tree = ([]);
}

/////////////////////////////////////////////////////////////////////////////
private nomask object researchDictionary()
{
    object ret = 0;
    if (file_size(ResearchDictionary) > -1)
    {
        ret = load_object(ResearchDictionary);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int isMemberOfResearchTree(string element)
{
    return member(tree, element);
}

/////////////////////////////////////////////////////////////////////////////
public nomask object getResearchItem(string element)
{
    object ret = 0;

    object dictionary = researchDictionary();
    if (dictionary)
    {
        ret = dictionary->researchObject(element);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs string Name(string newName)
{
    if(newName && stringp(newName))
    {
        name = newName;
    }
    return name;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs string Description(string newDescription)
{
    if(newDescription && stringp(newDescription))
    {
        description = newDescription;
    }
    return description;
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs string TreeRoot(string newRoot)
{
    if(newRoot && stringp(newRoot) && member(tree, newRoot))
    {
        if(getResearchItem(newRoot))
        {
            treeRoot = newRoot;
        }
        else
        {
            raise_error("ERROR - researchTree: the tree root element must "
                "exist and be a valid research item.\n");
        }
    }
    return treeRoot;
} 

/////////////////////////////////////////////////////////////////////////////
private nomask mapping getTreeNode(string element, object owner)
{
    mapping ret = ([ ]);

    if(element && stringp(element) && member(tree, element) && 
       (file_size(element) > 0))
    {
        object researchItem = getResearchItem(element);
        if(!tree[element]["children"])
        {
            ret[researchItem] = 0;
        }
        else if(pointerp(tree[element]["children"]) && 
                sizeof(tree[element]["children"]) &&
                stringp(tree[element]["children"][0]))
        {
            ret[researchItem] = ([ ]);
            foreach(string child in tree[element]["children"])
            {
                object childItem = getResearchItem(child);
                ret[researchItem] += getTreeNode(child, owner);
            }
        }

        if (owner && objectp(owner))
        {
            if (owner->isResearched(element))
            {
                ret[researchItem] += (["researched":1]);
            }
            else if (owner->isResearching(element))
            {
                ret[researchItem] += (["researching":1]);
            }
        }
    }
    return ret + ([ ]);
}

/////////////////////////////////////////////////////////////////////////////
public nomask varargs mapping getResearchTree(object owner)
{
    mapping ret = ([ ]);
    if(treeRoot && stringp(treeRoot))
    {
        ret += getTreeNode(treeRoot, owner);
    }
    return ret + ([ ]);
}

/////////////////////////////////////////////////////////////////////////////
public nomask int prerequisitesMetFor(string researchItem, object owner)
{
    int ret = 0;

    if(researchItem && stringp(researchItem) && owner && objectp(owner) &&
       function_exists("isResearched", owner) && member(tree, researchItem))
    {
        object research = getResearchItem(researchItem);
        ret = checkPrerequisites(owner) && research &&
            research->checkPrerequisites(owner);

        string *dependencies = tree[researchItem]["parents"];
        if(dependencies && pointerp(dependencies) && sizeof(dependencies))
        {
            foreach(string dependency in dependencies)
            {
                ret &&= owner->isResearched(dependency);
            }
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
private nomask int validateRelationship(string child, string parent)
{
    int ret = 1;
    if (member(tree, parent) && member(tree[parent], "parents") && tree[parent]["parents"] &&
        (member(tree[parent]["parents"], child) > -1))
    {
        ret = 0;
    }
    else if (member(tree[parent], "parents") && sizeof(tree[parent]["parents"]))
    {
        foreach(string grandparent in tree[parent]["parents"])
        {
            ret &&= validateRelationship(child, grandparent);
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int addChild(string child, string parent)
{
    int ret = 0;
    if((child != parent) && getResearchItem(child) && getResearchItem(parent))
    {
        if(!tree[child]["parents"])
        {
            tree[child]["parents"] = ({ });
        }
        if(!tree[parent]["children"])
        {
            tree[parent]["children"] = ({ });
        }
        
        ret = validateRelationship(child, parent);
        if (ret)
        {
            tree[child]["parents"] += ({ parent });
            tree[parent]["children"] += ({ child });
        }
        else
        {
            raise_error(sprintf("ERROR - researchTree: The relationship for "
                "child (%s) and parent (%s) results in infinite recursion.\n",
                child, parent));
        }
    }
    else
    {
        raise_error(sprintf("ERROR - researchTree: The child (%s) and "
            "parent (%s) must be unique valid researchItem objects.\n", 
            child, parent));
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
public nomask int addResearchElement(string location)
{
    int ret = 0;
    
    if(getResearchItem(location))
    {
        if(!member(tree, location))
        {
            tree[location] = ([
                "parents": 0,
                "children": 0
            ]);
            ret = 1;
        }
        else
        {
            raise_error(sprintf("ERROR - researchTree: The research item "
                "%s is already a member of the research tree.\n", 
                location));
        }
    }
    else
    {
        raise_error("ERROR - researchTree: The research element must exist"
            " in the location passed to this method and be a valid "
            "object before it can be added to any trees.\n");
    }
    return ret;
}
