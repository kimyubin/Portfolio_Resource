using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class blockButton : MonoBehaviour
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
        int buttonNum = GridSys.GetBlockNum();
        if(name == "Quad")
        {
            buttonNum = (int)GridSys.blockNum.quad;
        }
        else if(name == "26.57")
        {
            buttonNum = (int)GridSys.blockNum.tir26;
        }
        else if(name == "45")
        {
            buttonNum = (int)GridSys.blockNum.tri45;
        }
        else if(name == "63.43")
        {
            buttonNum = (int)GridSys.blockNum.tri63;
        }
        GridSys.SetBlockNum(buttonNum);
    }
}
