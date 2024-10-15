using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KillPlayer : MonoBehaviour
{
    // Start is called before the first frame update
   
    void OnTriggerEnter2D(Collider2D coll)
    {
        /*if(coll.GetComponent<PlayerScript>() != null)
        {
            coll.GetComponent<PlayerScript>().PlayerDead();
            GameManager.DieFuncStatic();
        }*/
        if(coll.gameObject.layer == (int)GameManager.Layer.PlayerLayer)
        {
            GameManager.DieFuncStatic();

        }
            
    }

}
