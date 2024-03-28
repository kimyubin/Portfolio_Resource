using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerFeet : MonoBehaviour
{
    bool inRail;
    bool inNotRail;

    public float railPower ;
    float accelerationPower;// 숙이면 가속
    Vector3 playerVelocity;
    Vector3 velo;
    PlayerCharacter playerCharacter;
    BoxCollider2D collider;
    Vector2 distance;
    void Start()
    {
        playerCharacter = GetComponentInParent<PlayerCharacter>();
        collider = GetComponent<BoxCollider2D>();
        inRail = false;
        inNotRail = true;
        accelerationPower = 1f;
    }
    void FixedUpdate()
    {
        InRailMove();
    }
    void InRailMove()
    {
        //슬라이더에 타면, 자동으로 오른쪽으로 이동. PlayerCharator에서 inRail이 true면 좌우 이동 불가하게 만듦
        //왼쪽으로 접근했을때도 이동하는 걸 방지
        //45도로 떨어지면 이동 정지.
        if(inRail && playerCharacter.GetRightLeft())
        {
            if(playerCharacter.getJumping())
            {
                //점프할땐 바닥에 닿고 있던 마지막 속도 기준. 만들필요없는데 명시용으로 만듦.
                if(railPower == 12f)
                    railPower = 12f;
                else if(railPower == 1f)
                    railPower = 1f;
            }
            else
            {
                railPower = 12f;
                //45도 떨어질때 용. 오르막용 안부딫히고, 내리막용엔 부딫힘.
                if (GameObject.Find("Rail45BackTrigger").GetComponent<Rail45BackTrigger>().GetBackRail() && !GameObject.Find("RailFrontTrigger").GetComponent<RailFrontTrigger>().GetFrontRail())
                {
                    railPower = 1f;
                }
            }      
            
            if(Input.GetAxisRaw("Vertical") < 0)
                accelerationPower = 1.4f;
            else
                accelerationPower = 1f;

            GetComponentInParent<PlayerCharacter>().transform.Translate(Vector3.right * railPower * accelerationPower * Time.deltaTime);
        }    
    }
    
    void OnTriggerEnter2D(Collider2D collision)
    {
        string collTag = collision.gameObject.tag;
        //촉수 점프용
        if(collTag == "Platform" || collTag == "Border" || collTag == "Bridge" )
        {
            GameObject.Find("TentacleJumpTrigger").GetComponent<TentacleJumpTrigger>().SetTentacleJumping(false);
            GameObject.Find("TentacleJumpTrigger").GetComponent<TentacleJumpTrigger>().SetAttachedFloor(true);
            inRail = false;
        }
        if(collision.gameObject.tag == "Rail" && playerCharacter.GetRightLeft())
        {   
            inRail = true;
        }             
    }

    void OnTriggerStay2D(Collider2D collision)
    { 
        string collTag = collision.gameObject.tag;

        if (collTag == "Rail"||collTag == "Platform" || collTag == "Border" || collTag == "Bridge")
        {
            //true = 벽에 닿고 있음 && 바닥에 정지하고 있으면 점프가능, 공중에서는 불가능. 벽에서 연속 점프 방지
            //일부 구간에서 점프 해제가 안되던 현상을 방지하기 위해 메커니즘 변경
            if (GetComponentInParent<PlayerCharacter>().getRigid().velocity.y == 0)
            {
                if (GameObject.Find("JumpAssist").GetComponent<JumpAssist>().GetJumpAssist())
                    playerCharacter.setJumping(true);
                else
                    playerCharacter.setJumping(false);
            }

        }
    }

    void OnTriggerExit2D(Collider2D collision)
    {
        string collTag = collision.gameObject.tag;

        //추락시 애니메이션
        if (!playerCharacter.getJumping())
        {
            if (collTag == "Rail" || collTag == "Platform" || collTag == "Border" || collTag == "Bridge")
            {
                playerCharacter.setJumping(true);
            }
        }
            
        
        if(collTag == "Platform" || collTag == "Border" || collTag == "Bridge" )
        {
            GameObject.Find("TentacleJumpTrigger").GetComponent<TentacleJumpTrigger>().SetAttachedFloor(false);
        }
        
    }

    public bool GetInRail()
    {
        return inRail;
    }
}
