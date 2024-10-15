using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    void Awake()
    {
        Screen.SetResolution(1280, 720, true);
        Application.targetFrameRate = 60;
    }



}
