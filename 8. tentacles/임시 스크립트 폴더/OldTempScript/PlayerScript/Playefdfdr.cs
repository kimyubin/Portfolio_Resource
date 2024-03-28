using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public class Playefdfdr : MonoBehaviour {

	public float movePower;
	public float jumpPower;
	public int MaxHealth = 100;
	int PlayerHealth = 100;
	bool LifeOrDeath = true;
	
	//보고있는 방향
	bool rightLeft;//right = true, left = false
	// 게임 내부 최대 제한 속도(종단속도 겸)
	float terminalVelocity; 
	
	//바닥에 닿고 있는 지
	bool onBridge;

	//대쉬를 위한 시간 저장
	float leftDashTime;
	float leftDeltaTime;
	float rightDashTime;
	float rightDeltaTime;

	bool doDash;
	
	//이전 프레임에서 사다리를 타고있었는지 저장. 사다리에서 벗어났는지 확인용	
	bool prevLabber;
	//사다리 + 플랫폼 통과중에 좌우 이동 제한. ture = 제한


	public SpriteRenderer spriteRenderer;
	public Rigidbody2D rigid;
	public Animator animator;


	bool jumpingState;
	

	// Use this for initialization
	void Start () 
	{
		rigid = gameObject.GetComponent<Rigidbody2D>();
		animator = gameObject.GetComponentInChildren<Animator>();
		spriteRenderer = gameObject.GetComponentInChildren<SpriteRenderer>();
		terminalVelocity = 15f;
		jumpingState = false;

		rightLeft = true;
		doDash  = false;
		prevLabber = false;
		onBridge = false;
	}
	
	// Update is called once per frame
	void Update () 
	{
		TerminalVelocityFunc();
		BridgePermeation();
	}
	void BridgePermeation()
	{		
		if(!Input.GetKey(KeyCode.DownArrow))
		{	
			if(Input.GetButtonDown("Jump"))		
				jumpingState = true;		
		}		

		if(Input.GetKey(KeyCode.DownArrow) && Input.GetButtonDown("Jump") && onBridge)
		{
			Physics2D.IgnoreLayerCollision(8,10,true);
		}
		
		if(rigid.velocity.y > 0 )
		{
			Physics2D.IgnoreLayerCollision(8,10,true);
		}
		if(!onBridge)
		{
			Physics2D.IgnoreLayerCollision(8,10,false);
		}

	}
	void FixedUpdate()
	{
		Move();
		Jump();
	}

	void Move()
	{
		Vector3 moveVelocity = Vector3.zero;
		//atkcol.doAtk이 false=평시면 움직임.
		{
			//사다리 타는 중이면서 플랫폼 안에 있으면 좌우 이동 금지.
			if((Input.GetAxisRaw("Horizontal") < 0))
			{
				moveVelocity = Vector3.left;
				transform.localScale = new Vector3(-1,1,1);	
				}
			else if((Input.GetAxisRaw("Horizontal") > 0))
			{
				moveVelocity = Vector3.right;
				transform.localScale = new Vector3(1,1,1);
			}				
		
			transform.position += moveVelocity * movePower * Time.deltaTime;			
		}
	}
	void Jump()
	{
		if(!jumpingState)
			return;

		rigid.velocity = Vector2.zero;

		Vector2 jumpVelocity = new Vector2(0,jumpPower);
		rigid.AddForce(jumpVelocity, ForceMode2D.Impulse);
		
		jumpingState = false;
	}

	void TerminalVelocityFunc()
	{
		//종단속도. 콜라이더 관통 현상 방지용
		if(rigid.velocity.y < -terminalVelocity)
			rigid.velocity = new Vector2(rigid.velocity.x,-terminalVelocity);
		//이하 비정상적인 속도 증가로 인한 콜라이더 관통현상 방지용
		if(rigid.velocity.y > terminalVelocity)
			rigid.velocity = new Vector2(rigid.velocity.x,terminalVelocity);
		if(rigid.velocity.x < -terminalVelocity)
			rigid.velocity = new Vector2(-terminalVelocity,rigid.velocity.y);
		if(rigid.velocity.x > terminalVelocity)
			rigid.velocity = new Vector2(terminalVelocity,rigid.velocity.y);
	}



	void OnTriggerStay2D(Collider2D other)
	{
		if(other.gameObject.tag == "Bridge")
			onBridge= true;

	}
	void OnTriggerExit2D(Collider2D other)
	{
		//브릿지에 닿지 않거나, 통과하면 다시 불통과 상태
		if(other.gameObject.tag == "Bridge")
		{
			Debug.Log("od");
			onBridge = false;
			Physics2D.IgnoreLayerCollision(8,10,false);
		}
			
	}
	public void SetOnBridge(bool bri)
	{
		onBridge = bri;
	}
	
}
