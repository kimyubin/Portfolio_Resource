using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerScirpt : MonoBehaviour
{
    enum LayerNum { Default = 0, Player = 8, Platform = 9, SideWall = 10, PlayerSideWallBox = 11 }

    public float movePower;
    public float jumpPower;
    //비상용 점프
    int velocityZeroCount;
    float preVelocity;


    //보고있는 방향. staic으로 구현해서 간헐적으로 종류 후 다시 플레이할때 종류직전 눌렀던 버튼이 눌리는 현상이있었음
    bool rightOn;
    bool leftOn;

    bool jumpingState;
	//카메라 세로 크기
	const float cameraSize16_9 = 1.875f; //1080*1980 16:9 에서카메라 크기 = 카메라 세로 크기의 1/2

    //스프링 점프
    bool springJumpState;
    GameObject springObj;

    SpriteRenderer spriteRend;
    Rigidbody2D rigid;
    Animator animator;
    AudioSource jumpSound;



    // Use this for initialization
    void Start()
    {
        rigid = gameObject.GetComponent<Rigidbody2D>();
        animator = gameObject.GetComponentInChildren<Animator>();
        spriteRend = gameObject.GetComponentInChildren<SpriteRenderer>();
        jumpSound = gameObject.GetComponent<AudioSource>();
        jumpingState = false;
        springJumpState = false;

        rightOn = false;
        leftOn = false;

        velocityZeroCount = 0;
        preVelocity = 0;
        //sound
        jumpSound.volume = 0.5f;
        jumpSound.pitch = 1.65f;
        jumpSound.mute = false;

    }

    // Update is called once per frame
    void Update()
    {
        //MoveAnimation();
        JumpAnimation();
        SpringJumpAnimation();
        EmergencyJump();
    }
    void FixedUpdate()
    {
        Move();
        Jump();
        SpringJump();
    }

    //Update


    void JumpAnimation()
    {
        //jump animation
        if (GetComponentInChildren<FootTriggerScript>().GetFootTrigger() && !animator.GetBool("animJumpState"))
        {
            jumpingState = true;
            animator.SetBool("animJumpState", true);
            animator.SetTrigger("animJumpTrigger");
        }
        if (rigid.velocity.y < -1)
        {
            animator.SetBool("animJumpDown", true);
        }
        else if (rigid.velocity.y > -1)
        {
            animator.SetBool("animJumpDown", false);
        }
        //간헐적으로 점프 모션 안나옴. 강하게 점프뛰면 강제 애니메이션 작동.
        if(rigid.velocity.y > 4)
        {
            jumpingState = true;
            animator.SetBool("animJumpState", true);
            animator.SetTrigger("animJumpTrigger");
        }
            

    }
    void SpringJumpAnimation()
    {
        if (springJumpState && rigid.velocity.y < -1.5)
        {
            animator.SetBool("animJumpState", true);
            animator.SetTrigger("animJumpTrigger");
        }
    }
    
    //5프레임 연속 y축 가속도가 고정되어져 있으면 강제 점프.
    void EmergencyJump()
    {
        if(Time.timeScale == 0) // 일시정시 시
            return;            
        if(transform.position.y > 169) // 최대 높이 도달.
            return;
            
        if(preVelocity == rigid.velocity.y)
        {
            velocityZeroCount++;
        }
        else
        {
            velocityZeroCount = 0;
        }
        
        if(velocityZeroCount > 4)
        {
            rigid.velocity = Vector2.zero;

            Vector2 jumpVelocity = new Vector2(0, jumpPower);
            rigid.AddForce(jumpVelocity, ForceMode2D.Impulse);
            jumpingState = false;

            jumpSound.Play();
        }

        preVelocity = rigid.velocity.y;
    }
    //걷기 애니메이션 제어함수. 현재는 사용하지 않음.
    void MoveAnimation()
    {
        if (Input.GetAxisRaw("Horizontal") == 0)
        {
            animator.SetBool("animRunBool", false);
        }
        //move animation
        if (Input.GetAxisRaw("Horizontal") < 0 || leftOn)
        {
            animator.SetBool("animRunBool", true);

        }
        else if (Input.GetAxisRaw("Horizontal") > 0 || rightOn)
        {
            animator.SetBool("animRunBool", true);
        }

    }

    //FixedUpdate

    void Move()
    {
        //
        if(GetComponentInChildren<FootTriggerScript>().GetFootTrigger())
            return;
        
        Vector3 moveVelocity = Vector3.zero;
        if ((Input.GetAxisRaw("Horizontal") < 0) || leftOn)
        {
            moveVelocity = Vector3.left;
            transform.localScale = new Vector3(-1, 1, 1);
        }
        else if ((Input.GetAxisRaw("Horizontal") > 0) || rightOn)
        {
            moveVelocity = Vector3.right;
            transform.localScale = new Vector3(1, 1, 1);
        }

        transform.position += moveVelocity * movePower * Time.deltaTime;
    }

    void Jump()
    {
        if (jumpingState && GetComponentInChildren<FootTriggerScript>().GetFootTrigger())
        {
            rigid.velocity = Vector2.zero;

            Vector2 jumpVelocity = new Vector2(0, jumpPower);
            rigid.AddForce(jumpVelocity, ForceMode2D.Impulse);
            jumpingState = false;

            jumpSound.Play();
        }
    }


    public void SpringJump()
    {
        if (springJumpState && rigid.velocity.y < -1.5f)
        {
            rigid.velocity = Vector2.zero;
            Vector2 jumpVelocity = new Vector2(0, jumpPower * 2f);
            rigid.AddForce(jumpVelocity, ForceMode2D.Impulse);

            springObj.GetComponent<CoilSpringScript>().RunAnim();
            springJumpState = false;
            jumpSound.Play();
        }
    }
	


	/*=================================Access method=================================*/

    public void SetSpringJump(bool sj, GameObject spring)
    {
        springJumpState = sj;
        springObj = spring;
    }
    public void SetAnim(string animStr, bool animbool)
    {
        animator.SetBool(animStr, animbool);
    }
    public Rigidbody2D GetRigid()
    {
        return rigid;
    }
    public void RigidVelocityZero()
    {
        rigid.velocity = Vector2.zero;
    }
    public void GetButton(bool buttonKinds, bool onOff)
    {
        if (buttonKinds)//right
            rightOn = onOff;
        else//left
            leftOn = onOff;
    }
}
