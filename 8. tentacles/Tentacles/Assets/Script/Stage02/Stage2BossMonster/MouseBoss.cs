    using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MouseBoss : MonoBehaviour
{
    public GameObject stone;
    public GameObject Pattern2Stone;
    public GameObject questDownWall01;
    Rigidbody2D rigid;

    int phase;

    Vector3 playerPosition;
    float targetPositionX;
    float movePower;
    float touchPlayerStopMove;
    float HPmax;
    float HP;

    bool playerIn;

    bool rightLeft; // true= right, false = left
    bool forcedDirectionChange; // 강제로 방향 전환. 플레이어 닿으면 반대 방향으로 일정 시간동안 물러남. 벽에 끼어버리는 것 방지.
    bool tempRightLeft;
    bool startBoss1;
    bool startBoss2;
    bool phase2On;
    bool eventMoveOn;
    bool phase2Jump;//뒤집힌 다음 정상으로 돌아올때 한번 점프하는 거 체크용.
    bool death;//default = false; 죽으면 true;

    // Start is called before the first frame update
    void Start()
    {
        //플랫폼, 브릿지, 종유석 통과 가능
        Physics2D.IgnoreLayerCollision(9,16,true);
        Physics2D.IgnoreLayerCollision(10,16,true);
        Physics2D.IgnoreLayerCollision(15,16,true);

        rigid = GetComponent<Rigidbody2D>();

        phase = 1;

        playerPosition = transform.position;// 진행 목표를 현재 위치로 초기화
        startBoss1 = false;
        death = false;
        phase2On = false;
        eventMoveOn = false;
        phase2Jump=false;
        rightLeft = false;
        forcedDirectionChange = false;
        movePower = 2f;
        touchPlayerStopMove = 1f;

        HPmax = GetComponentInChildren<HitTrigger>().GetHP()[0];//최대체력
        HP = GetComponentInChildren<HitTrigger>().GetHP()[1]; //현재 체력 갱신
    }

    // Update is called once per frame
    void Update()
    {
        HPmax = GetComponentInChildren<HitTrigger>().GetHP()[0];//최대체력
        HP = GetComponentInChildren<HitTrigger>().GetHP()[1]; //현재 체력 갱신

        if(phase == 1)
        {
            //체력이 3/4이하로 떨어지면, 페이즈 2 작동, eventMoveOn이 켜지면 자동으로 멈추고 이동
            if (((HP / HPmax) < 0.77f) && !eventMoveOn)
            {
                //공격, 피격 일시 중지.
                GetComponentInChildren<AttackTrigger>().GetComponent<PolygonCollider2D>().enabled = false;
                phase2On = true;//Move() 함수 정지.    
                EventOn();
            }
        }
        else if( phase == 2 )
        {

        }

    }
    void FixedUpdate()
    {
        if(phase == 1)
        {
            //startboss 시작위치에 도착해야 움직임. phase2on 체력이 3/4으로 깍이면 움직임 멈춤. eventMoveOn eventMove()로 이동할땐 움직임 멈춤.
            if (startBoss1 && !phase2On)
                Move();
            if (eventMoveOn) //돌에 맞으면 움직이기 시작.
                EventMove();
        }
        else if(phase == 2)
        {
            //startboss2 시작위치에 도착해야 움직임. 
            if (startBoss2)
                Move();
        }
    }

    void Move()
	{
		Vector3 moveVelocity = Vector3.zero;

        targetPositionX = playerPosition.x - transform.position.x; // 목표까지 X축 거리
            
        //플레이어가 공격 범위 안에 들어와야 방향전환. 아니면 계속 앞으로 이동.
        if(playerIn)
        {
            //몬스터 위에 있거나 안에 있으면 방향전환 X, 빠르게 진동하듯 좌우 전환하는 거 막는 기능.
            if (targetPositionX > 3f || targetPositionX < -3f)
            {
                if (targetPositionX > 0)
                    rightLeft = true;
                else
                    rightLeft = false;
            }
        }
        if(forcedDirectionChange)
        {
            rightLeft = tempRightLeft;
        }
 
		if(rightLeft)//오른쪽
		{
			moveVelocity = Vector3.right;
            transform.localScale = new Vector3(-8,8,1);
        }
		else
		{
			moveVelocity = Vector3.left;
            transform.localScale = new Vector3(8,8,1);
		}

		transform.position += moveVelocity * movePower * Time.deltaTime * touchPlayerStopMove;
	}
    void EventOn()
    {
        //뒤집힘
		if(rightLeft)//오른쪽
		{
			transform.localScale = new Vector3(-8,-8,1);
		}
		else
		{
			transform.localScale = new Vector3(8,-8,1);

		}
        //20초 뒤 자동 낙하 or 돌을 일정 이상 때리면 자동낙하. 낙하하는 돌이 destroy되면 작동 안함.
        StartCoroutine("Event");
        if(stone.GetComponent<StoneScript>().GetStoneCount()  && !(Pattern2Stone == null))
        {
            Pattern2Stone.GetComponent<Pattern2DropStone>().SetKinematic();
        }
    }
    void EventMove()
    {
        Physics2D.IgnoreLayerCollision(17,18,true); // 벽 뚫고 지나감.


        Vector3 moveVelocity = Vector3.right;
        transform.localScale = new Vector3(-8,8,1);
        if(!phase2Jump)
        {
            phase2Jump = true;
            rigid.velocity = new Vector2(0,10f);
        }

        transform.position += moveVelocity * movePower * 3f * Time.deltaTime;  

        //3번째 방으로 강제 이동.
        if(transform.position.x >= 101f)     
        {
            transform.position = new Vector3(185f,transform.position.y,transform.position.z);
            GetComponentInChildren<AttackTrigger>().enabled = false;

            eventMoveOn = false; // 이벤트 무브 작동 중지.
            questDownWall01.GetComponent<DownWall>().GetComponentInChildren<BrickWall>().SetClear();//퀘스트 완료
            GetComponentInChildren<AttackTrigger>().GetComponent<PolygonCollider2D>().enabled = true; // 공격 및 피격 제개

            Physics2D.IgnoreLayerCollision(17,18,false); // 벽 뚫는거 원상복귀.
            phase = 2;//페이즈 2로 완전히 넘어감.
        }   
    }


    IEnumerator Event()
    {        
        yield return new WaitForSeconds(20f);// 20초 뒤 종유석 자동 낙하.
        if(!(Pattern2Stone == null))
            Pattern2Stone.GetComponent<Pattern2DropStone>().SetKinematic();
    }

    public void SetPhase2On()
    {
        eventMoveOn = true;
    }

    public void SetPlayerIn(bool pl)
    {
        playerIn = pl;
    }
    public void SetPlayerPosition(Vector3 pos)
    {
        playerPosition = pos;
    }
    public void SetRightLeft()
    {
        //방향 전환
        rightLeft = !rightLeft;
    }    
    public bool GetRightLeft()
    {
        return rightLeft;
    }
    public void SetStartBoss1()
    {
        startBoss1 = true;
    }
    public void SetStartBoss2()
    {
        startBoss2 = true;
    }
    public void SetTouchPlayerMove()
    {
        touchPlayerStopMove = 0f;
    }
    public void StopBoss(float move)
    {
        //잠시 동안 정지상태 유지.
        StartCoroutine("StopBossCo",move);
    }
    IEnumerator StopBossCo(float moveTime)
    {
        touchPlayerStopMove = 0f;
        yield return new WaitForSeconds(moveTime);// 플레이어 닿으면 대기 후 다시 움직임
        touchPlayerStopMove = 1f;
    }

    
}
