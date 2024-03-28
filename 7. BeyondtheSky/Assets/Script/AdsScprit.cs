using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Advertisements;
public class AdsScprit : MonoBehaviour
{
    // Start is called before the first frame update
    string gameId = "3420219";
    void Start()
    {
        Advertisement.Initialize(gameId);
    }

    public void ShowAds()
    {
        if (Advertisement.IsReady("video"))
        {
            Advertisement.Show("video");
        }
    }
}
