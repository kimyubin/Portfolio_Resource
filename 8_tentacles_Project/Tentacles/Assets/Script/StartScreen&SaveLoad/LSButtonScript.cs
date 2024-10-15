using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LSButtonScript : MonoBehaviour
{
    int ButtonNum;
    JsonSaveLoad saveLoad;
    PlayerSaveData load;
    // Start is called before the first frame update
    void Start()
    {
         //세이브 매니저, 로드 매니저 분리.
        if (GetComponentInParent<SaveMenuScript>() != null)
        {
            saveLoad =GetComponentInParent<SaveMenuScript>().GetComponentInParent<JsonSaveLoad>();
        }
        if (GetComponentInParent<LoadMenuScript>() != null)
        {
            saveLoad =GetComponentInParent<LoadMenuScript>().GetComponentInParent<JsonSaveLoad>();
        }


        transform.GetChild(0).gameObject.SetActive(false);
        //이름으로 각 버튼 구별.
        switch (name)
        {
            case "Load0":
                ButtonNum = 0;
                break;
            case "Load1":
                ButtonNum = 1;            
                break;
            case "Load2":
                ButtonNum = 2;
                break;
            case "Load3":
                ButtonNum = 3;
                break;
            case "LoadBack":
                ButtonNum = 4;
                break;
            default:
                break;
        }
        
        if(ButtonNum != 4)//뒤로가기 제외
        {
            //세이브 파일 이름 출력
            if (saveLoad.SaveFileCheck(ButtonNum))
            {
                load = saveLoad.LoadJsonFile<PlayerSaveData>(ButtonNum);
                GetComponentInChildren<Text>().text = load.saveTime;
            }
            else
                GetComponentInChildren<Text>().text = "비어있음";
        }
    }


    // Update is called once per frame
    void Update()
    {
        //세이브 매니저, 로드 매니저 분리. 
        //버튼을 가리킬 때 밑줄이 생김.
        if (GetComponentInParent<SaveMenuScript>() != null)
        {
            if (SaveMenuScript.GetButtonON() == ButtonNum)
            {
                transform.GetChild(0).gameObject.SetActive(true);
                
                //덮어쓰기 경고문.//뒤로가기 제외
                if(saveLoad.SaveFileCheck(ButtonNum) && (ButtonNum != 4))
                    GetComponentInChildren<Text>().text = "새로운 세이브 파일로 덮어쓰기";
            }                
            else
            {
                transform.GetChild(0).gameObject.SetActive(false);
                if (ButtonNum != 4)
                {
                    //세이브 파일 이름 출력
                    if (saveLoad.SaveFileCheck(ButtonNum))
                        GetComponentInChildren<Text>().text = load.saveTime;
                    else
                        GetComponentInChildren<Text>().text = "비어있음";
                }
            }
                
        }
        if (GetComponentInParent<LoadMenuScript>() != null)
        {
            if (LoadMenuScript.GetButtonON() == ButtonNum)
                transform.GetChild(0).gameObject.SetActive(true);
            else
                transform.GetChild(0).gameObject.SetActive(false);
        }


       
    }
    public void MouseEnter()
    {
        if(GetComponentInParent<SaveMenuScript>() != null)
            SaveMenuScript.SetButtonON(ButtonNum);  

        if(GetComponentInParent<LoadMenuScript>() != null)
            LoadMenuScript.SetButtonON(ButtonNum);
    }
    public void MouseUp()
    {
        if(GetComponentInParent<SaveMenuScript>() != null)
            GetComponentInParent<SaveMenuScript>().ButtonRun();

        if(GetComponentInParent<LoadMenuScript>() != null)
            GetComponentInParent<LoadMenuScript>().ButtonRun();
    }
}
