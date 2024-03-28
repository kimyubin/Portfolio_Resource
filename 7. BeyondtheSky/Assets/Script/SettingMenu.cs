using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SettingMenu : MonoBehaviour
{
    public GameObject StartMenu;
    // Start is called before the first frame update
    void Start()
    {
    }
    public void startMenuON()
    {
        gameObject.SetActive(false);
        StartMenu.SetActive(true);
    }
}
