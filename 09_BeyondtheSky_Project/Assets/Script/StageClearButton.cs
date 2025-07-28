using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
public class StageClearButton : MonoBehaviour
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
        Time.timeScale = 1;
        if (name == "NextStage")
        {
            GameManager.NextStage();
        }            
        if (name == "SaveMainMenu")
        {
            Time.timeScale = 1f;
            SceneManager.LoadScene((int)GameManager.SceneNumber.startMenu, LoadSceneMode.Single);
        }
    }
}
