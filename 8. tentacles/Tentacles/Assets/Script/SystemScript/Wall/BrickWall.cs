using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BrickWall : MonoBehaviour
{
    bool QuestOnOff; // 퀘스트 유무
    bool MissionComplete; // 퀘스트 완료 유무
    bool rightTrigger;
    bool leftTrigger;
    bool topEnd;
    Rigidbody2D rigid;
    // Start is called before the first frame update
    void Start()
    {
        rigid = GetComponent<Rigidbody2D>();
        topEnd = false;

        //기본적으로 문은 앞에 도착하면 자동으로 열림.
        MissionComplete = true;
        
        //퀘스트가 있는 방이면, MissionComplete을 완수해야 문이 열림. 
        if(QuestOnOff)
        {
            MissionComplete= false;
        } 
    }

    void FixedUpdate()
    {
        rightTrigger = GetComponentInParent<DownWall>().GetRightTrigger();
        leftTrigger = GetComponentInParent<DownWall>().GetLeftTrigger();
        
        if(MissionComplete)//미션 완수해야 작동.
        {
            if (leftTrigger && !topEnd) // 끝에 도달하면 작동 안하게
            {
                rigid.isKinematic = true;
                if (transform.localPosition.y <= 4f) // 상대 위치 4까지만 움직임
                    transform.Translate(Vector3.up * 1f * Time.deltaTime);
                else // 끝까지 다올라간 것을 확인하는 용도.
                    topEnd = true;
            }
        }

        if(rightTrigger && topEnd) // 끝까지 올라간 다음에, 지나가면 떨어짐.
            rigid.isKinematic = false;
    }
    public void SetQuest(bool set)
    {
        QuestOnOff = set;
    }
    
    public void SetClear()
    {
        // 외부에서 클리어 조건 완료되면 함수로 클리어.
        MissionComplete = true;
    }
}
