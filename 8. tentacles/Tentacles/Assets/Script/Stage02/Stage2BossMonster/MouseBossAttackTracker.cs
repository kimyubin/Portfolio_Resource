using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MouseBossAttackTracker : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void OnTriggerStay2D(Collider2D other)
    {
        // 플레이어가 닿으면 플레이어 위치 전송.
        if(other.tag == "Player")
        {
            GetComponentInParent<MouseBoss>().SetPlayerPosition(other.transform.position);
        }
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        //플레이어가 있어야 방향전환.
        if(other.transform.tag == "Player")
        {
            GetComponentInParent<MouseBoss>().SetPlayerIn(true);
        }
    }
    void OnTriggerExit2D(Collider2D other)
    {
        //플레이어가 있어야 방향전환.
        if(other.transform.tag == "Player")
        {
            GetComponentInParent<MouseBoss>().SetPlayerIn(false);
        }
    }
}
