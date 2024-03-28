using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OptionButtonScript : MonoBehaviour
{
    int ButtonNum;

    // Start is called before the first frame update
    void Start()
    {

        transform.GetChild(0).gameObject.SetActive(false);
        //이름으로 각 버튼 구별.
        switch (name)
        {
            case "Option0":
                ButtonNum = 0;
                break;
            case "Option1":
                ButtonNum = 1;            
                break;
            case "Option2":
                ButtonNum = 2;
                break;
            case "Option3":
                ButtonNum = 3;
                break;
            case "Back":
                ButtonNum = 4;
                break;
            default:
                break;
        }        
    }


    // Update is called once per frame
    void Update()
    {
        //버튼을 가리킬 때 밑줄이 생김.
        if (OptionMenuScript.GetButtonON() == ButtonNum)
            transform.GetChild(0).gameObject.SetActive(true);
        else
            transform.GetChild(0).gameObject.SetActive(false);

    }
    public void MouseEnter()
    {
        OptionMenuScript.SetButtonON(ButtonNum);
    }
    public void MouseUp()
    {
        GetComponentInParent<OptionMenuScript>().ButtonRun();
    }
}
