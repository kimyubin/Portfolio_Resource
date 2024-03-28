using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JumpAssist : MonoBehaviour
{
    bool jumpAssistBool;
    void Start()
    {
        jumpAssistBool = false;
    }
    void OnTriggerStay2D(Collider2D other)
    { 
        string collTag = other.gameObject.tag;
        if (collTag == "Rail"||collTag == "Platform" || collTag == "Border" || collTag == "Bridge")
            jumpAssistBool = true;
    }

    void OnTriggerExit2D(Collider2D other)
    {
        string collTag = other.gameObject.tag;
        if (collTag == "Rail"||collTag == "Platform" || collTag == "Border" || collTag == "Bridge")
            jumpAssistBool = false;
    }

    public bool GetJumpAssist()
    {
        return jumpAssistBool;
    }

        
}
