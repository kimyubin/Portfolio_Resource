using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerScript : MonoBehaviour
{
    const int R = 45*0, RUp = 45*1, Up = 45*2, LUp = 45*3, L = 45*4, LDown = 45*5, Down = 45*6, RDown = 45*7;

    public float movePower;  //최고 속력
    public float refAnimTime;
    public float jumpPower;  //점프력

    public float fallMul = 2.5f; // 추락속도 조절
    public float lowJump = 2f; // 잠깐 눌렀다 땠을때 조절용

    public float rotationPower;// 회전속도
    public float HookSpeed;//후크 당겨지는 속도
    public float hookPendulumPower;//후크 진자 운동 추가 에너지

    List<Vector3> hookPaths;//후크 줄 경로 저장.

    public GameObject tempdeadbody;
    

    public GameObject circleHead; //머리 오브젝트
    public GameObject headDefOut; // 머리가 따라가는 외부 오브젝트
    public GameObject Body; // 몸통
    public GameObject Laser;
    public GameObject rayHitImage;
    public GameObject HookPoint;
    public GameObject HookSprite;
    MeshRenderer HookRend;


    public Rigidbody2D rigid;
    public FootTriggerScript foot;
    public FeetCheckerScript feetChecker;
    public RaycastHitTirgger rayHitTrigger;
    
    SpriteRenderer LaserImage;
    LineRenderer lineRend;
    float jumpMove;

    //후크 버튼
    bool laserDown;//레이저 버튼을 누른 순간 true. 후크 버튼 누르거나, 레이저 버튼 떼면 false;
    bool hookDown;// 레이저 버튼 누른 후, 후크버튼을 누르고 있는 동안 true; 레이저 or 후크 버튼 up하면 false;
    bool hooking;// 레이저 버튼 누른 후, 최초 후크 다운 이후 계속 true, 레이저, 후크 둘다 업 하면 false;
    

    Vector3 tempHookDes;//임시 후크 위치 및 현재 후크 고정 위치

    //bool isJump;

    int layMask;//raycast ignore

    float desAngle;
    float startAngle;
    private IState currentState;

    void Awake()
    {
        SetState(new IdleState());
    }

    // Start is called before the first frame update
    void Start()
    {
        rigid = GetComponent<Rigidbody2D>();
        foot = GetComponentInChildren<FootTriggerScript>();
        feetChecker = GetComponentInChildren<FeetCheckerScript>();
        LaserImage = Laser.GetComponentInChildren<SpriteRenderer>();
        rayHitTrigger = GetComponentInChildren<RaycastHitTirgger>();
        HookRend = HookSprite.GetComponentInChildren<MeshRenderer>();
        HookRend.enabled = false;

        lineRend = GetComponent<LineRenderer>();
        lineRend.useWorldSpace = true;
        lineRend.enabled = false;

        hookPaths = new List<Vector3>();        

        LaserImage.enabled = false;
        rayHitImage.SetActive(false);

        jumpMove = 1f;
        desAngle = 0;
        startAngle = 0f;

        laserDown = false;
        hookDown = false;
        hooking = false;

        //isJump = true;

        //후크 리액티브 조인트2D 기본 상태 초기화.
        GetComponent<RelativeJoint2D>().enabled = false; // 리엑티브 조인트 비활성화
        rigid.freezeRotation = true; // 회전 고정. 안하면 덜덜 떨면서 이동함

        //raycast ignore
        int playerLayer = 1 << 8;
        int footInore = 1 << 9;
        int raycastIgnore = 1 << 11;
        layMask = playerLayer | footInore | raycastIgnore; // 플레이어 레이어 및 11번 레이어 = 위치 추적용 트리거는 레이케스트에 작동안함.
        layMask = ~layMask; 

    }

    // Update is called once per frame
    void Update()
    {
        if(!laserDown) //&& !hooking)
            currentState.SUpdate(); // animation;
        if(!laserDown && !hooking)
        {
            Jump();//키씹힘을 방지하기 위해 Fixed에서 update로 변경.
        }

        Hook();
        
        
        //rigid z축 회전 고정 대신. 
        //후크 걸때 z축 고정시 안돌아가, z축 고정 해제
        //z축 고정 해제 시, 캐릭터가 돌아가기 때문에 z축 회전을 0으로 고정.
        //고정 해제시에만 0으로 돌리면, 훅을 풀어서 다시 고정 시켰을 때 오류 발생 위험이 있어서 상시 작동.
        //update, fixedUpdate 둘다 사용.
        transform.eulerAngles = new Vector3(transform.eulerAngles.x,transform.eulerAngles.y,0);
    }
    void FixedUpdate()
    {
        if(!laserDown && !hooking)
        {
            Move();
        }
        HeadUpDown();//머리 상하로 움직이는 거 구현

        //rigid z축 회전 고정 대신. 
        transform.eulerAngles = new Vector3(transform.eulerAngles.x,transform.eulerAngles.y,0);
    }
    
    public void SetState(IState nextState)
    {
        if(currentState !=null)
        {
            currentState.
            Exit();
        }
        currentState = nextState;
        currentState.Enter(this);
    }
    void HookUnlock()
    {
        laserDown = false;
        hookDown = false;
        hooking = false;
        GetComponent<RelativeJoint2D>().enabled = false; // 리엑티브 조인트 비활성화
        HookRend.enabled = false;//후크 이미지 비활성화
        lineRend.enabled = false;
        rigid.freezeRotation = true; // 회전 고정. 안하면 덜덜 떨면서 이동함
        hookPaths.Clear(); //후크 경로 초기화
        return;
    }
    void Hook()
    {
        if(Input.GetButtonDown("Laser"))
        {
            laserDown = true;
        }        
        if(Input.GetButtonUp("Laser"))
        {
            laserDown = false;
        }
        
        LaserMovement();
        if(laserDown)
        {
            if(Input.GetButtonDown("Hook"))
            {
                laserDown = false;//레이저 누르고 있는 지 유무가 아니라, 레이저가 활성화된 상황만 상정. 
                hookDown = true;
                hooking = true;
               
                tempHookDes = rayHitTrigger.GetRayPos();

                //후크 이미지 회전
                HookSprite.transform.eulerAngles = new Vector3(0,0,Mathf.Atan2((tempHookDes - transform.position).y,(tempHookDes - transform.position).x)*Mathf.Rad2Deg);

            }
        }
        if(Input.GetButtonUp("Hook"))
        {
            hookDown = false;
            GetComponent<RelativeJoint2D>().enabled = false; // 리엑티브 조인트 비활성화
            rigid.freezeRotation = true; // 회전 고정. 안하면 덜덜 떨면서 이동함
        }
        
        if(hooking) // 후크가 갈려있는 모든 상태
        {
            HookRend.enabled = true;//후크 이미지

            //후크 종료
            if(Input.GetButtonUp("Laser"))
            {
                HookUnlock();
                return;
            }
            if (rayHitTrigger.GetRayTag() == "Other")
            {
                RaycastHit2D hitRay = Physics2D.Raycast(transform.position, (tempHookDes - transform.position).normalized, Mathf.Infinity, layMask);

                //후크 위치 != 레이케스트 위치; 중간에 막는 게 있음
                if (new Vector2(tempHookDes.x, tempHookDes.y) != hitRay.point)
                {
                    hookPaths.Add(tempHookDes); //리스트에 이전 후크 위치 기록
                    tempHookDes = new Vector3(hitRay.point.x, hitRay.point.y, 0f); //막힌 위치로 후크 위치 변경.
                }
                    
                //다시 풀릴때용.
                if (hookPaths.Count >= 1)
                {
                    RaycastHit2D hitRay2 = Physics2D.Raycast(transform.position, hookPaths[hookPaths.Count - 1] - transform.position, Mathf.Infinity, layMask);
                    
                    if (new Vector2(hookPaths[hookPaths.Count - 1].x, hookPaths[hookPaths.Count - 1].y) == hitRay2.point)
                    {
                        tempHookDes = hookPaths[hookPaths.Count - 1];
                        hookPaths.RemoveAt(hookPaths.Count - 1);//마지막 변수 뻄
                    }
                }

                //후크 위치
                if (hookPaths.Count >= 1)
                {
                    HookSprite.transform.position = hookPaths[0];                    
                }
                else
                {
                    HookSprite.transform.position = tempHookDes;
                }


                // 밧줄
                lineRend.enabled = true;
                lineRend.positionCount = hookPaths.Count + 2;

                for (int i = 0; i < hookPaths.Count; i++)
                {
                    lineRend.SetPosition(i, hookPaths[i]);
                }
                lineRend.SetPosition(hookPaths.Count, tempHookDes);
                lineRend.SetPosition(hookPaths.Count + 1, transform.position);


                if (Input.GetButtonUp("Hook"))//후크 안당김
                {
                }
                if (Input.GetButton("Hook"))// 후크 당김
                {
                    //최종 후크 걸린 곳에서 1.8f까지만 접근. 그 이상 접근 시 후크 당길 수 없게 함
                    if (hookPaths.Count == 0)
                    {
                        if((transform.position - tempHookDes).sqrMagnitude > 3.24f)//==.magnitude > 1.8; 거리의 제곱으로 계산
                        {
                            transform.position = Vector3.MoveTowards(transform.position, tempHookDes, Time.deltaTime * HookSpeed);
                        }
                    }
                    else
                    {
                        if((transform.position - hookPaths[hookPaths.Count-1]).sqrMagnitude > 3.24f)//==.magnitude > 1.8; 거리의 제곱으로 계산
                        {
                            transform.position = Vector3.MoveTowards(transform.position, tempHookDes, Time.deltaTime * HookSpeed);
                        }
                    }
                }

                HookPoint.transform.position = tempHookDes; //후크 포인트 위치 고정               

                GetComponent<RelativeJoint2D>().enabled = true; //리엑티브 조인트 활성화

                rigid.freezeRotation = false;//회전 고정 해제



                //좌우 흔들기, 애니메이션은 점프 애니메이션 사용
                Vector3 hookMove = Vector3.zero;
                if ((Input.GetAxisRaw("Horizontal") > 0))//right
                {
                    hookMove = Vector3.right;
                }
                else if ((Input.GetAxisRaw("Horizontal") < 0)) //left
                {
                    hookMove = Vector3.left;
                }
                rigid.AddForce(hookMove * hookPendulumPower);
            }



        }
    }
    void LaserMovement()
    {
        if(laserDown)
        {
            LaserImage.enabled = true;
            rayHitImage.SetActive(true);
            Time.timeScale = 0.1f;
            Time.fixedDeltaTime = Time.timeScale * 0.02f; // fixedUpdate는 초당 50회.

        }
        else
        {
            LaserImage.enabled = false;
            rayHitImage.SetActive(false);

            Time.timeScale = 1f;
            Time.fixedDeltaTime = Time.timeScale * 0.02f; // fixedUpdate는 초당 50회.
            return;
        }


        if ((Input.GetAxisRaw("Horizontal") == 0) && (Input.GetAxisRaw("Vertical") == 0)) // no keydown
            return;


        if ((Input.GetAxisRaw("Horizontal") > 0)) // 우
            desAngle = 0;
        if ((Input.GetAxisRaw("Horizontal") < 0)) // 좌
            desAngle = 180;
        if ((Input.GetAxisRaw("Vertical") > 0)) // 상
            desAngle = 90;
        if ((Input.GetAxisRaw("Vertical") < 0)) // 하
            desAngle = 270;
        if ((Input.GetAxisRaw("Horizontal") > 0) && (Input.GetAxisRaw("Vertical") > 0)) // 우상
            desAngle = 45;
        if ((Input.GetAxisRaw("Horizontal") > 0) && (Input.GetAxisRaw("Vertical") < 0)) // 우하
            desAngle = 315;
        if ((Input.GetAxisRaw("Horizontal") < 0) && (Input.GetAxisRaw("Vertical") > 0)) // 좌상
            desAngle = 135;
        if ((Input.GetAxisRaw("Horizontal") < 0) && (Input.GetAxisRaw("Vertical") < 0)) // 좌하
            desAngle = 225;



        if (Laser.transform.eulerAngles.z == desAngle)
        {
            startAngle = Laser.transform.eulerAngles.z;
            return;
        }

        Laser.transform.rotation = Quaternion.Slerp(Quaternion.Euler(new Vector3(0, 0, Laser.transform.eulerAngles.z)),Quaternion.Euler(new Vector3(0, 0, desAngle)),rotationPower);
    }


    void Move()
    {     
        Vector3 moveVelocity = Vector3.zero;

        if ((Input.GetAxisRaw("Horizontal") > 0))//right
        {
            moveVelocity = Vector3.right;
        }
        else if ((Input.GetAxisRaw("Horizontal") < 0)) //left
        {
            moveVelocity = Vector3.left;
        }
          
        if(!feetChecker.CheckGround())//공중
            jumpMove = 0.7f;
        else 
            jumpMove = 1f;

        transform.position += moveVelocity * movePower * Time.deltaTime * jumpMove;
    }
    void Jump()
    {
        //점프시 보정
        if(rigid.velocity.y < 0) //추락 속도 보정. 뛸때보다 약간 빨리 떨어짐.
        {
            rigid.velocity += Vector2.up * Physics2D.gravity.y * (fallMul -1) *Time.deltaTime;

        }
        else if(rigid.velocity.y > 0 && !Input.GetButton("Jump")) // 오래 누르면 더 높이 점프함.
        {
            rigid.velocity += Vector2.up * Physics2D.gravity.y * (lowJump -1) *Time.deltaTime;
        }
        
        if (Input.GetButtonUp("Jump"))
        {
            //isJump = true;
        }

        if(!feetChecker.CheckGround())//공중에 있으면 점프 금지
            return;


        //if(isJump)
        {
            if (Input.GetButtonDown("Jump"))
            {
                Vector2 jumpVelocity = new Vector2(rigid.velocity.x, jumpPower);
                rigid.velocity = Vector2.up * jumpVelocity;
                //rigid.AddForce(jumpVelocity, ForceMode2D.Impulse);
                //isJump = false;
            }

        }

    }
    void HeadUpDown()
    {
        //localPosition Y축 최대 이동 범위 1.0~1.45f;
        //기본은 1.2f
        const float headHi = 1.5f;
        const float headDown = 1.0f;

        float preHeadY = circleHead.transform.position.y;
        
        circleHead.transform.position = new Vector3(circleHead.transform.position.x, headDefOut.transform.position.y, circleHead.transform.position.z);
        
        if(circleHead.transform.localPosition.y >= headHi)
        {
            circleHead.transform.localPosition = new Vector3(circleHead.transform.localPosition.x, headHi, circleHead.transform.localPosition.z);
            //circleHead.transform.position = new Vector3(circleHead.transform.position.x, preHeadY, circleHead.transform.position.z);
        }
        if(circleHead.transform.localPosition.y <= headDown)
        {
            circleHead.transform.localPosition = new Vector3(circleHead.transform.localPosition.x, headDown, circleHead.transform.localPosition.z);
            //circleHead.transform.position = new Vector3(circleHead.transform.position.x, preHeadY, circleHead.transform.position.z);
        }

    }
    public void PlayerDeadCopy()
    {
        HookUnlock();
        DeadBodyScript.SetDeadBodyRotate(circleHead.transform.position,Body.transform.position, Body.transform.rotation);
    }
}