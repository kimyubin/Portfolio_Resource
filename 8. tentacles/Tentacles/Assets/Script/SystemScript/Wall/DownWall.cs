using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DownWall : MonoBehaviour
{
    public bool QuestOnOff;
    // 퀘스트 존재 유무. 클리어 조건이 필요한 구간은 인스펙터에서 체크.
    //체크하기 편하게 부모 컴포넌트에서 체크하고, 퀘스트 유무를 자식에게 물려줌.

    BoxCollider2D wallColl;
    
    void Awake()
    {
        //BrickWall의 start()보다 먼저 함수를 발동해, 퀘스트 유무를 전달하기 위해 Awake()사용. Awake()는 모든 start함수보다 먼저 실행됨.
        GetComponentInChildren<BrickWall>().SetQuest(QuestOnOff);
    }

    void Start()
    {
        wallColl = GetComponent<BoxCollider2D>();
        wallColl.enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        if(GetRightTrigger()) // 플레이어가 오른쪽으로 넘어가면, 벽이 다 내려오지 않아도, 자동으로 이동을 콜라이더로 막음.
            wallColl.enabled = true;
    }

    public bool GetRightTrigger()
    {
        return GetComponentInChildren<RightTrigger>().GetOnRightWallIN();
    }
    public bool GetLeftTrigger()
    {
        return GetComponentInChildren<LeftTrigger>().GetOnLeftWallIN();
    }
}
