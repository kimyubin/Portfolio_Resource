using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class RecordButtonScript : MonoBehaviour
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
        switch(name)
        {
            case "BackButton":
                SceneManager.LoadScene((int)GameManager.SceneNumber.startMenu, LoadSceneMode.Single);
                break;
            case "UpButton":
                RecordScript.UpCount();
                break;
            case "DownButton":
                RecordScript.DownCount();
                break;            
            default :
                break;
        }
    }
}
