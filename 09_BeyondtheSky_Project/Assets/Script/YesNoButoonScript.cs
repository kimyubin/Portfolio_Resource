using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class YesNoButoonScript : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public void MouseClick()
    {
        LPrinceManager.ButtonClick = true;
        if(name == "YesButton")
            LPrinceManager.yesNo = true;
        else
            LPrinceManager.yesNo = false;
        
    }
}
