inherit "/lib/modules/combat/attacks/baseAttack.c";

public void init()
{
    ::init();
    setDamageType("psionic");
    addHitDictionary(({
        "blast", "slam", "menace", "blast", "crush" }));

    addSimileDictionary(({
        "with a blast of psionic energy", "with a wave of mental force",
        "with ripples of mental energy" }));
        
    addMissMessage("##AttackerName## ##Infinitive::stare## deeply at "
        "##TargetName##. Everyone is impressed.");
    
    addHitMessage("##AttackerName## ##HitDictionary## ##TargetName##"
        " ##SimileDictionary##.", "1-50");
}


