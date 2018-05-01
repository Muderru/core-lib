//*****************************************************************************
// Copyright (c) 2018 - Allen Cummings, RealmsMUD, All rights reserved. See
//                      the accompanying LICENSE file for details.
//*****************************************************************************
virtual inherit "/lib/modules/conversations/baseConversation.c";

/////////////////////////////////////////////////////////////////////////////
protected void Setup()
{
    addTopic("first conversation", "@S@`##ActorSubjective##'s already dead. "
        "##ActorPossessive## body just hasn't figured "
        "it out yet.', @D@interjects Berenar as he impassively stares at you. He "
        "looks rather beaten up, but is still hale. He is rather rattled and with "
        "a broken sigh, he adds @S@`By the gods, Galadhel, I can't find the words "
        "to even describe what I've seen. Those who were dead animated? Their "
        "blades as fell as ever... Those robed cowards... they're alive. At least "
        "most are. Basil shot one through the throat a few minutes ere you got "
        "here. At least a dozen of those... things... fell to the ground after "
        "that... That got their attention... got Basil killed too. One of the "
        "black-robed ones raised his hand, there was a flash... metal, perhaps, "
        "and something struck Basil - sliced his neck wide open. Still, it was "
        "enough to cause them to flee. Looks like they dragged their fallen "
        "with 'em though.'");

    addResponse("first conversation", "I'm not dead yet...",
        "@S@The rumor of my demise is greatly exaggerated. I hurt... everywhere, "
        "but you'll not be rid of me so easily.");
    addTopic("not dead", "Getting better!");
    addResponseTopic("first conversation", "I'm not dead yet...", "not dead");

    addResponse("first conversation", "[0;31m[Say nothing...][0m", 0);
    addTopic("player will live", "Player's gonna be OK!");
    addResponseTopic("first conversation", "[0;31m[Say nothing...][0m", "player will live");
}