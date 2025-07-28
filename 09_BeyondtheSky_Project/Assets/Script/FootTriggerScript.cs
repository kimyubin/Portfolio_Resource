using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FootTriggerScript : MonoBehaviour
{
    bool FootTriggerON;
    // Start is called before the first frame update
    void Start()
    {
        FootTriggerON = false;
    }
    void OnTriggerStay2D(Collider2D coll)
    {
        if(coll.tag == "Platform")
        {
            //상단 플랫폼 통과 도중 애니메이션이 바뀌는 것을 방지하기 위해 정지 했을 때 Idle로 전환.
            if (GetComponentInParent<PlayerScirpt>().GetRigid().velocity.y == 0)
            {
                FootTriggerON = true;

                GetComponentInParent<PlayerScirpt>().SetAnim("animJumpState", false);
                GetComponentInParent<PlayerScirpt>().SetAnim("animJumpDown", false);
                
                //일회용 발판에 닿았을 때.
                if (coll.GetComponent<DisposablePlatScript>() != null)
                {
                    coll.GetComponent<DisposablePlatScript>().SetDisposTouch();
                }
            }   
            
        }
    }
    void OnTriggerExit2D(Collider2D coll)
    {
        if(coll.tag == "Platform")
        {
            FootTriggerON = false;
        }
    }
    void OnTriggerEnter2D(Collider2D coll)
    {
        if(coll.tag == "EndStage")
            StartCoroutine("StageClearEvent");
    }
    //일정 시간 이후 작동.
    IEnumerator StageClearEvent()
    {
        yield return new WaitForSeconds(1f);
        GameManager.StageClear();
    }

    public bool GetFootTrigger()
    {
        return FootTriggerON;
    }

}
