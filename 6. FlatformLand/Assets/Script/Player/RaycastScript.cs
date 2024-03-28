using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RaycastScript : MonoBehaviour
{
    LineRenderer lineRend;
    public GameObject hit;

    int layMask;
    // Start is called before the first frame update
    void Start()
    {
        lineRend = GetComponent<LineRenderer>();
        lineRend.useWorldSpace = true;

        //raycast object ignoreLayer & raycast ignore 
        Physics2D.IgnoreLayerCollision(8,11, true);
        Physics2D.IgnoreLayerCollision(9,11, true);
        
        int playerLayer = 1 << 8;
        int foot = 1 << 9;
        int raycastIgnore = 1 << 11;
        layMask = playerLayer | foot | raycastIgnore; // 플레이어 레이어 및 11번 레이어 = 위치 추적용 트리거는 레이케스트에 작동안함.
        layMask = ~layMask; 
        
    }

    // Update is called once per frame
    void Update()
    {
        RaycastHit2D hitRay = Physics2D.Raycast(transform.position, transform.right,Mathf.Infinity, layMask);
        hit.transform.position = hitRay.point;
        hit.GetComponent<RaycastHitTirgger>().SetRayPos(hitRay.point);

        lineRend.SetPosition(0, transform.position);
        lineRend.SetPosition(1, hit.transform.position);
       
    }
}
