using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
public class MainMenuButtonScript : MonoBehaviour
{
    
    public void MouseUp()
    {
        Time.timeScale = 1f;
        SceneManager.LoadScene((int)GameManager.SceneNumber.startMenu, LoadSceneMode.Single);
    }
}
