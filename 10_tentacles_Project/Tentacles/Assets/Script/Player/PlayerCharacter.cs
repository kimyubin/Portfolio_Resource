using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public class PlayerCharacter : MonoBehaviour {
    
    public float movePower;  //이동속력
    public float jumpPower;  //점프력
    public float cameraMoveMax;   //고개 들고 숙일때 카메라가 움직이는 최대 거리
    public float cameraMoveSpeed; //고개 들고 숙일때 카메라가 움직이는 속도
    public float flickInterval;
    
    Animator animator;
    Rigidbody2D rigid;
    SpriteRenderer renderer;
    PlayerFeet feet;
    HitTrigger hitTrigger;
    BoxCollider2D boxColl2d;

    Cinemachine.CinemachineVirtualCamera vcam;

    public float cameraMoveRange;
    public float cameraYIdle;
    public float cameraXIdle;
    
    float attackTime;

    static bool attackPossible; // 스테이지간 상태 유지를 위해 static으로.
    bool isHurted;
    bool isJumping; //점프모션중 true
    bool isDucking; //고개숙이는중 true
	bool onBridge;//다리 위에 있는 지 확인용
    
    float horizontal_in; //업데이트시 horizontal 입력값이 임시로 저장되는 변수
    float vertical_in; //업데이트시 vertical 입력값이 임시로 저장되는 변수
    float jump_in; //업데이트시 jump 입력값이 임시로 저장되는 변수
    bool attack_in; // 업데이트시 attack 입력값이 임시로 저장되는 변수
    float charge_in; // 업데이트시 attack 입력값이 임시로 저장되는 변수


    void Awake()
    {
        //플레이어 초기화 위치
        transform.position = PlayerStaticData.GetInitPosition();

    }
    // Start is called before the first frame update
    void Start()
    {
        Random.InitState((int)(Time.time * 100f)); // 나중에 라운드 시작함수가 생기면 옮겨야함. srand(time(NULL))역할
        animator = GetComponent<Animator>();
        rigid = GetComponent<Rigidbody2D>();
        renderer = GetComponent<SpriteRenderer>();
        feet = GetComponentInChildren<PlayerFeet>();
        hitTrigger = GetComponent<HitTrigger>();
        vcam = GameObject.FindObjectOfType<Cinemachine.CinemachineVirtualCamera>();
        boxColl2d = GetComponent<BoxCollider2D>();
        isJumping = true;
        isHurted = isDucking = false;
        attackTime = 0;
        HeroBehaviour.initBehaviour(this);
        HeroBehaviour.initCamera(this);


        cameraMoveRange = 0.2f;
        cameraYIdle = 0.6f;
        cameraXIdle = 0.5f;

        //플레이어 ignore layer
        Physics2D.IgnoreLayerCollision(8,18,true); // 플레이어, playerIgnore
        Physics2D.IgnoreLayerCollision(18,18,true);//playerIgnore, PlayerIgnore


    }
    
    void Update()
    {
        CameraMovement();
		BridgePermeation();
        PlayerStaticData.PlayerDataUpdate();//데이터 업데이트용.
    }

    void FixedUpdate()
    {
        Hurt();
        getInput();
    }
    
    //입력을 받아 애니메이션 컨트롤러에 알려줌
    void getInput()
    {
        //입력값을 미리 받아서 사용
        vertical_in = Input.GetAxisRaw("Vertical"); //음수인경우 아래, 양수인경우 위 방향키
        horizontal_in = Input.GetAxisRaw("Horizontal"); //음수인경우 좌, 양수인경우 우 방향키
        jump_in = Input.GetAxisRaw("Jump"); //점프버튼이 눌린 동안 양수
        charge_in = Input.GetAxisRaw("Charge");
        attack_in = !attack_in & Input.GetAxisRaw("Attack") > 0;

        if(GetComponentInChildren<PlayerFeet>().GetInRail())//레일 안에서 이동불가
            horizontal_in = 0;
        
        animator.SetBool("JumpIn", jump_in == 1);
        animator.SetInteger("HorizontalIn", (int)horizontal_in);
        animator.SetInteger("VerticalIn", (int)vertical_in);
        animator.SetBool("AttackIn", attack_in);
        //if (animator.GetBool("AttackIn")) Debug.Log("aa");
        animator.SetBool("ChargeIn", charge_in > 0);
    }
    
    void CameraMovement()// 카메라 위아래 보는 기능. 레일 탈 때 카메라 고정용.
    {
        //레일 이동시 카메라 X축 고정용. 레일에서 나온다음 왼쪽을 보면 카메라 떨림 현상 방지 위해 rightLeft 첨가.
        if(GetComponentInChildren<PlayerFeet>().GetInRail() && GetRightLeft())
            StartCoroutine("CameraXMove");
        else
        {
            // 캐릭터가 바라보는 방향으로 카메라 돌림.
            if(GetRightLeft())
                cameraXIdle = 0.3f;
            else
                cameraXIdle = 0.5f+0.2f;
            vcam.GetCinemachineComponent<Cinemachine.CinemachineFramingTransposer>().m_ScreenX = cameraXIdle;
        }
    }
    IEnumerator CameraXMove()
    {
		while (cameraXIdle > 0.0f) 
        {
            cameraXIdle -=0.005f;
			vcam.GetCinemachineComponent<Cinemachine.CinemachineFramingTransposer>().m_ScreenX = cameraXIdle;
			yield return new WaitForSeconds(0.1f);
	    }
    }
    
    void Hurt()
    {
        Vector2 velocity;
        isHurted = hitTrigger.getHurt();
        if (!isHurted) return;
        
        //몬스터 오른쪽에서 맞으면(= 플레이 기준 몬스터 위치 왼쪽) 왼쪽을 바라보고, 오른쪽으로 날아감.
        if (hitTrigger.getOppositeRL())
        {
            rigid.transform.localScale = new Vector3(-1, 1, 1);
            velocity.x = 5;
        }
        else
        {
            rigid.transform.localScale = new Vector3(1, 1, 1);
            velocity.x = -5;
        }
        velocity.y = 3;

        rigid.velocity = velocity;

        animator.SetBool("Hurted", true);
        
        StartCoroutine("flicker");
    }
  
    void BridgePermeation() 
	{		
        //아래로 점프 하면 통과가능
		if(vertical_in < 0 && jump_in > 0 && onBridge)
		{
			Physics2D.IgnoreLayerCollision(8,10,true);
            vcam.GetCinemachineComponent<Cinemachine.CinemachineFramingTransposer>().m_ScreenY = cameraYIdle; 
            // 아래로 내려보면서 통과할 때 카메라 원상 복귀 안되는 현상 방지. 함수에서 Idle상태로 변경 되지 않은 상태에서 탈출하면서 생기는 현상으로 추정.
		}
		if(getJumping() && rigid.velocity.y > 0) // 점프하면 통과 가능.
		{
			Physics2D.IgnoreLayerCollision(8,10,true);
		}
		if(!onBridge) // 발판을 빠져나오면 다시 통과 불가능
		{
			Physics2D.IgnoreLayerCollision(8,10,false);
		}

	}

    //발쪽 충돌 콜라이더에서 호출하여 점프상태를 조절하는 함수

    public bool getJumping()
    {
        return animator.GetBool("IsJumping");
    }
    public void setJumping(bool value)
    {
        animator.SetBool("IsJumping", value);
    }

    public void SetOnBridge(bool bri)
	{
		onBridge = bri;
	}

    public bool GetRightLeft()
    {
        return rigid.transform.localScale.x == 1;
    }
    public float GetHorizontal()
    {
        return horizontal_in;
    }

    public Rigidbody2D getRigid()
    {
        return rigid;
    }

    public Cinemachine.CinemachineVirtualCamera getVcam()
    {
        return vcam;
    }

    public Animator getAnim()
    {
        return animator;
    }

    IEnumerator flicker() //맞은뒤에 깜빡이는 함수. 지속시간을 Inspector에서 설정할 수 있으나,
    {                       // 한번 깜빡이는데에 0.2초가 걸리므로, 지속시간은 0.2초의 배수로 적용됨(ex : 지속시간을 0.3초로 설정한경우, 실제로는 0.4초동안 깜빡임)
        float flickTime = flickInterval;
        Color32 color = renderer.color;
        while (flickTime > 0)
        {
            color.a = 90;
            renderer.color = color;
            yield return new WaitForSeconds(0.1f);
            color.a = 180;
            renderer.color = color;
            yield return new WaitForSeconds(0.1f);
            flickTime -= 0.2f;
        }
        color.a = 255;
        renderer.color = color;
        isHurted = false;
    }
}
