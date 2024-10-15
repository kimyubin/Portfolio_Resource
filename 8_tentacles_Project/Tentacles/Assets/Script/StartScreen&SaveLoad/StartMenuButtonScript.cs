using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StartMenuButtonScript : MonoBehaviour
{
    int ButtonNum;
    // Start is called before the first frame update
    void Start()
    {
        transform.GetChild(0).gameObject.SetActive(false);
        switch (name)
        {
            case "NewButton":
                ButtonNum = 0;
                break;
            case "LoadButton":
                ButtonNum = 1;            
                break;
            case "ExitButton":
                ButtonNum = 2;
                break;
            default:
                break;
        }

    }

    // Update is called once per frame
    void Update()
    {
        if(StartManager.GetButtonON() == ButtonNum)
            transform.GetChild(0).gameObject.SetActive(true);
        else
            transform.GetChild(0).gameObject.SetActive(false);
       
    }
    public void MouseEnter()
    {
        StartManager.SetButtonON(ButtonNum);
    }
    public void MouseUp()
    {
        GetComponentInParent<StartManager>().ButtonRun();
    }
}
