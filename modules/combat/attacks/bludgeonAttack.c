inherit "/lib/modules/combat/attacks/baseAttack.c";

public void init()
{
    ::init();
    setDamageType("bludgeon");
    addHitDictionary(({
        "bash", "smash", "pummel", "slam", "smack", "clobber",
        "whack" }));

    addSimileDictionary(({
        "soundly", "solidly", "on ##TargetPossessive## ##BodyPart##",
        "viciously", "with bone-crushing force" }));

    addMissMessage("##AttackerName## ##Infinitive::bludgeon## invisible foes!");

    addHitMessage("##AttackerName## ##HitDictionary## ##TargetName## "
            "##SimileDictionary##.", "1-50");
}


