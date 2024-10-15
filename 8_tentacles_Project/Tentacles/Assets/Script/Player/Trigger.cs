using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Trigger : MonoBehaviour
{
    bool touchPlatform; // 플랫폼에 닿고 있는지

    BoxCollider2D box2d;

    void OnTriggerStay2D(Collider2D other)
    {
        string collTag = other.gameObject.tag;

        if(collTag == "Platform" || collTag == "Border" || collTag == "Bridge" )
        {
            touchPlatform = true;
        }   

    }
    void OnTriggerExit2D(Collider2D other)
    {        
        string collTag = other.gameObject.tag;

        if(collTag == "Platform" || collTag == "Border" || collTag == "Bridge" )
        {
            touchPlatform = false;
        }      

    }
    public bool GetTouchPlatform()
    {
        return touchPlatform;
    }
}
