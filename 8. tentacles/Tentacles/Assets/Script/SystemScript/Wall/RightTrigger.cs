using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RightTrigger : MonoBehaviour
{
    bool onRightWallIN;
    // Start is called before the first frame update
    void Start()
    {
        onRightWallIN = false;
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.tag == "Player")
            onRightWallIN = true;
    }
    public bool GetOnRightWallIN()
    {
        return onRightWallIN;
    }
}
