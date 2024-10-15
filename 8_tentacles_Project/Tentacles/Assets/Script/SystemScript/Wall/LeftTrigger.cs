using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LeftTrigger : MonoBehaviour
{
    bool onLeftWallIN;
    // Start is called before the first frame update
    void Start()
    {
        onLeftWallIN = false;
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.tag == "Player")
            onLeftWallIN = true;
    }
    public bool GetOnLeftWallIN()
    {
        return onLeftWallIN;
    }

}
