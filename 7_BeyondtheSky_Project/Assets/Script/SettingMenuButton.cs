using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class SettingMenuButton : MonoBehaviour
{
    public GameObject centerButton;
    void Start()
    {
        if(name == "CheckBox")
        {
            if(JsonSaveLoad.SaveFileCheck("SettingData"))//세이브 데이터 있으면 불러옴.
            {
                JsonSaveLoad.DataLoad("SettingData");
            }
            if (PlayerData.mute) // 음소거
            {
                centerButton.SetActive(true);
            }
            else//아님
            {
                centerButton.SetActive(false);
            }
        }
    }
    public void MuteToggle()
    {
        if(PlayerData.mute)//음소거일 때 클릭, 음소거 -> 일반
        {
            centerButton.SetActive(false);
            AudioListener.pause = false;   
            PlayerData.SetMute(false);
            JsonSaveLoad.DataSave("SettingData");                 
        }
        else//일반 -> 음소거
        {
            centerButton.SetActive(true);
            AudioListener.pause = true;  
            PlayerData.SetMute(true);
            JsonSaveLoad.DataSave("SettingData");
        }
    }
    public void BackButton()
    {
        GetComponentInParent<SettingMenu>().startMenuON();
    }
    public void CreditButton()
    {
        SceneManager.LoadScene((int)GameManager.SceneNumber.creditsScene, LoadSceneMode.Single);//크래딧으로 넘어감.
    }
}
