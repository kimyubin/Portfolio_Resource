using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StartTuto : MonoBehaviour
{
    bool startTutoIn;
    // Start is called before the first frame update
    void Start()
    {
        startTutoIn = false;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.tag == "Player")
        {
            startTutoIn = true;
        }
    }
    public bool GetStartTutoIn()
    {
        return startTutoIn;
    }
}
