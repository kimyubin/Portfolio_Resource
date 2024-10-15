using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PauseStartButtonScript : MonoBehaviour
{
    int ButtonNum;// 현재 오브젝트 이름 1 = pause, 2 = start;
    
    // Start is called before the first frame update
    void Start()
    {
        switch(name)
        {
            case "PauseButton":
                ButtonNum = 1;
                break;
            case "StartButton":
                ButtonNum = 2;
                break;
            case "StageReset":
                ButtonNum = 3;
                break;
            case "MainMenu":
                ButtonNum = 4;
                break;
            default:
                break;
        }
    }

    public void MouseUp()
    {
        GetComponentInParent<PauseStartSuperScript>().ButtonRun(ButtonNum);             
    }
}
