using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FeetCheckerScript : MonoBehaviour
{
    public GameObject CheckerPoint;
    RaycastHit2D hitRay;

    int layMask;
    // Start is called before the first frame update
    void Start()
    {
        //raycast object ignoreLayer & raycast ignore         
        int Ground = 1 << 12;
        int Platform = 1 << 13;
        layMask = Ground | Platform; // 그라운드만 작동함.
        //layMask = ~layMask;
        
    }

    // Update is called once per frame
    void Update()
    {
        hitRay = Physics2D.Raycast(transform.position, Vector3.down ,Mathf.Infinity, layMask);

        CheckerPoint.transform.position = hitRay.point;       
    }
    public bool CheckGround()
    {
        if(hitRay.distance < 0.015f)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
