inherit "/lib/modules/combat/attacks/baseAttack.c";

public void init()
{
    ::init();
    setDamageType("physical");
    addHitDictionary(({
        "snip", "clip", "pinch", "poke", "tear", "wrend", "bite" }));

    addSimileDictionary(({
        "on ##TargetPossessive## ##BodyPart##", "repeatedly in "
        "the ##BodyPart##", "viciously" }));
        
    addMissMessage("##AttackerName## ##Infinitive::snap## ##AttackerPossessive##"
            " beak menacingly, but harmlessly.");
    
    addHitMessage("##AttackerName## ##HitDictionary## ##TargetName## "
            "##SimileDictionary## with ##AttackerPossessive## beak.","1-50");
}



