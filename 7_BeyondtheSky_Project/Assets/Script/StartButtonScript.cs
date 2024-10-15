using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
 
public class StartButtonScript : MonoBehaviour
{
    void Start()
    {
        if (name == "LoadGame")
        {
            //세이브 파일 유무 확인
            if (JsonSaveLoad.SaveFileCheck("StateSaveData"))
            {
                //세이브 파일 스테이지 넘버가 0이면 중간 저장 데이터 없음
                JsonSaveLoad.DataLoad("StateSaveData");
                //죽어서 데이터가 없음
                if (PlayerData.stageNum == 0)
                {
                    GetComponentInChildren<Text>().color = Color.gray;
                }
                //중간 저장 데이터 있음
                else
                {
                    GetComponentInChildren<Text>().color = Color.white;
                }
            }
            else//세이브 파일 자체가 없음
            {
                GetComponentInChildren<Text>().color = Color.gray;
            }
                 
        }
    }
    public void StartButton()
    {
        GetComponent<AudioSource>().Play();
        switch(name)
        {            
            case "NewGame":
                GetComponentInParent<StartMenuScript>().NewGame();
                break;
            case "LoadGame":
                { 
                    //세이브 파일 유무 확인
                    if (JsonSaveLoad.SaveFileCheck("StateSaveData"))
                    {
                        //세이브 파일 스테이지 넘버가 0이면 중간 저장 데이터 없음
                        JsonSaveLoad.DataLoad("StateSaveData");
                        if (PlayerData.stageNum != 0)
                        {
                            GetComponentInParent<StartMenuScript>().LoadGame();
                        }
                    }
 
                }
                break;
            case "Record":
                GetComponentInParent<StartMenuScript>().Record();
                break;
            case "Exit":
                GetComponentInParent<StartMenuScript>().Exit();
                break;
            case "Settings":
                GetComponentInParent<StartMenuScript>().Settings();
                break;
            default:
                break;
        }
    }
}