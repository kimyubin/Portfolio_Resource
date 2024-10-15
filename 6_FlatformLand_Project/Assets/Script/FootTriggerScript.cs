using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FootTriggerScript : MonoBehaviour
{
    bool footTrig;
    // Start is called before the first frame update
    void Start()
    {
        footTrig = false;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void OnTriggerStay2D(Collider2D coll)
    {
        footTrig = true;
    } 
    void OnTriggerExit2D(Collider2D coll)
    {
        footTrig = false;
    }

    public bool GetFootTrigger()
    {
        return footTrig;
    }

}
