using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class StartMenuScript : MonoBehaviour
{
    public GameObject SettingMenu;
    // Start is called before the first frame update
    public void NewGame()
    {
        GameManager.initData();
        
        SceneManager.LoadScene((int)GameManager.SceneNumber.stage01, LoadSceneMode.Single);
    }
    public void LoadGame()
    {
        GameManager.ContinueLoadGame();
    }
    public void Record()
    {       
        SceneManager.LoadScene((int)GameManager.SceneNumber.recordMenu, LoadSceneMode.Single);
    }
    public void Exit()
    {
        #if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
        #else
            Application.Quit();
        #endif
    }
    public void Settings()
    {
        SettingMenu.SetActive(true);
        gameObject.SetActive(false);
    }
}
