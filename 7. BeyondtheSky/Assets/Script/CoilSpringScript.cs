using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CoilSpringScript : MonoBehaviour
{
    Animator anim;

    // Start is called before the first frame update
    void Start()
    {
        anim = GetComponentInChildren<Animator>();
    }
    void OnTriggerEnter2D(Collider2D coll)
    {
        if(coll.tag == "Player")
        {
            if(coll.gameObject.GetComponent<PlayerScirpt>() != null)
            {
                //점프 타이밍과 애니메이션의 싱크로율을 위해 실행을 플레이어 쪽에 맡김.
                coll.gameObject.GetComponent<PlayerScirpt>().SetSpringJump(true, gameObject);
            }
                
        }

    }
    void OnTriggerExit2D(Collider2D coll)
    {
        if(coll.tag == "Player")
        {
            if(coll.gameObject.GetComponent<PlayerScirpt>() != null)
                coll.gameObject.GetComponent<PlayerScirpt>().SetSpringJump(false, gameObject);

        }
    }
    
    public void RunAnim()
    {
        anim.SetTrigger("CoilMoveTrigger");
    }
    

}
