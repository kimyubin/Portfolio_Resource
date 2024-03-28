using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LRVirtualButton : MonoBehaviour
{
    bool buttonKinds;
    // Start is called before the first frame update
    void Start()
    {
        switch(name)
        {
            case "RightButton":
                buttonKinds = true;
                break;
            case "LeftButton" :
                buttonKinds = false;
                break;
            default:
                break;
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public void ButtonDown()
    {
        GameObject.FindWithTag("Player").GetComponent<PlayerScirpt>().GetButton(buttonKinds,true);
    }
    public void ButtonUp()
    {
        GameObject.FindWithTag("Player").GetComponent<PlayerScirpt>().GetButton(buttonKinds,false);
    }
}
