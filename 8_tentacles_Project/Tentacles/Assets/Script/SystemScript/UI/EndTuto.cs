using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EndTuto : MonoBehaviour
{
    bool endTutoIn;
    // Start is called before the first frame update
    void Start()
    {
        endTutoIn = false;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.tag == "Player")
        {
            endTutoIn = true;
        }
    }
    public bool GetEndTutoIn()
    {
        return endTutoIn;
    }
}
