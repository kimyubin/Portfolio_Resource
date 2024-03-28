using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossMovementTrigger : MonoBehaviour
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
        //벽에 닿으면 방향전환
        if(other.gameObject.tag == "Border" || other.gameObject.tag == "Wall" )
        {
            GetComponentInParent<MouseBoss>().SetRightLeft();
        }

    }
}
