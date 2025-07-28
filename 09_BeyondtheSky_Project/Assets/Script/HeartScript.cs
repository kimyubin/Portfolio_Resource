using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class HeartScript : MonoBehaviour
{
    Image[] heartArray;
    // Start is called before the first frame update
    void Start()
    {
        heartArray = GetComponentsInChildren<Image>();
    }

    // Update is called once per frame
    void Update()
    {
        HeartFill();
    }
    void HeartFill()
    {        
        heartArray[0].fillAmount = (float)PlayerData.heart / 10;
        heartArray[1].fillAmount = ((float)PlayerData.heart - 10) / 10;
        heartArray[2].fillAmount = ((float)PlayerData.heart - 20)/10;
    }
}
