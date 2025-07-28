using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossAtackTrigger : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        //플레이어와 접촉시 무조건 정지. 플레이어 밀어버리는 현상 방지
        if(other.transform.tag == "Player")
            GetComponentInParent<MouseBoss>().StopBoss(3f);

    }
    void OnTriggerExit2D(Collider2D other)
    {

    }
    
}

