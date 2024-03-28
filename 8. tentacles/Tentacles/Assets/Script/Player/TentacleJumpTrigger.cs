using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TentacleJumpTrigger : MonoBehaviour
{

    Vector3 targetPosition;//점프 플랫폼 좌상단 좌표값.


    float cellInterval; //셀 1개 간격
    float playerHeight;

    Rigidbody2D playerRigid;
    bool tentacleJumping; // 촉수 점프 중인가 유무확인.
    bool attachedFloor;//바닥에 붙어있으면 true;
    bool inUpperPlatform; // 위쪽에 플랫폼이 있으면


    // Start is called before the first frame update
    void Start()
    {
        playerRigid = GetComponentInParent<PlayerCharacter>().GetComponent<Rigidbody2D>();
        targetPosition = Vector3.zero;
        tentacleJumping = false;
        attachedFloor = false;
        inUpperPlatform = false; // 위쪽 플랫 폼에 닿을 경우

        cellInterval= 1.47f;
    }

    // Update is called once per frame
    void Update()
    {
        if(PlayerStaticData.GetTentacleJump())
            TentacleJumpFunc();
    }

    void TentacleJumpFunc()
    {
        int touchNum = 0;
        float tentacleJumpPower = 9f;
        float playerFeetPosition;

        //0번째 발판이 없고, 텐타클 점프 중이 아니면 작동.
        if(!gameObject.transform.GetChild(0).GetComponent<Trigger>().GetTouchPlatform() && !tentacleJumping)// && attachedFloor)
        {
            if(Input.GetButtonDown("TentacleJump"))
            {

                // 다음 발판까지 거리 계산
                for(int i = 1; i < (transform.childCount - 1); i++) //TempTentacle 때문에 1개 빠짐.
                {
                    if(gameObject.transform.GetChild(i).GetComponent<Trigger>().GetTouchPlatform())
                    {
                        touchNum = i;
                        break;
                    }
                }

                //touchNum가 0이면 점프 가능 발판 없음.
                //다음 발판까지의 점프력 및 목표까지 거리 계산.
                if(touchNum != 0)
                {
                    tentacleJumping = true;
                    //좌우 구분
                    if(GetComponentInParent<PlayerCharacter>().GetRightLeft())
                        targetPosition.x = (int)(gameObject.transform.position.x + touchNum + cellInterval + 0.5f);//소숫점 이하 절삭. 가우스. 타켓의 위치 x좌표. 트리거 중심 좌표라 +0.5f를 해줘서 끝 좌표 기준으로 바꿔줌.
                    else
                        targetPosition.x = (int)(gameObject.transform.position.x - (touchNum + cellInterval + 0.5f - 1));//셀의 왼쪽이 아니라 오른쪽 끝이 기준이라 -1
                    
                    //목표의 x축 값이 음수이면, 원 값과 달라서 1칸 줄임.
                    if(targetPosition.x < 0)
                    {
                        targetPosition.x -= 1;
                    }

                    playerFeetPosition = gameObject.GetComponentInParent<PlayerCharacter>().transform.position.y - 0.128f;
                    //박스콜라이더로 인해 생기는 플레이어 중심부터 지표면까지의 거리. 0.128은 콜라이더 중앙부터 지표면까지의 거리. 침투 현상 때문에 계산식 사용은 부적합.

                    //y축 양음수에 따른 포인트 부분
                    if(playerFeetPosition >= 0)
                        targetPosition.y = (int)(playerFeetPosition);// 부모의 y값의 가우스. 
                    else
                        targetPosition.y = (int)(playerFeetPosition - 1);// 0 밑에서 가우스값은 원 값보다 값이 커지기 때문에 -1 해줌. 1.5 > 1, -1.5 < -1
                    
                                
                    if(inUpperPlatform)//위쪽 부분에 플렛폼이 닿으면 1칸 위를 목표로 스프라이트 생성
                        targetPosition.y += 1;
                    
                    Vector2 newV = playerRigid.velocity;
                    newV.y = 9f;

                    switch (touchNum)
                    {
                        case 1:
                            tentacleJumpPower = 7f;
                            break;
                        case 2:
                            tentacleJumpPower = 9f;
                            break;
                        case 3:
                            tentacleJumpPower = 12f;
                            break;
                        case 4:
                            tentacleJumpPower = 14f;
                            break;
                        case 5:
                            tentacleJumpPower = 16f;
                            break;
                        case 6:
                            tentacleJumpPower = 18f;
                            break;
                        case 7:
                            tentacleJumpPower = 20f;
                            break;
                        default:
                            break;
                    }

                    if (GetComponentInParent<PlayerCharacter>().GetRightLeft())//오른쪽
                        newV.x = tentacleJumpPower;
                    else
                        newV.x = -tentacleJumpPower;

                    playerRigid.velocity = newV;
                }

            }

        }

    }
    public Vector3 GetTargetPosition()
    {
        return targetPosition;
    }
    public bool GetTentacleJumping()
    {
        return tentacleJumping;
    }
    public void SetTentacleJumping(bool jum)
    {
        tentacleJumping = jum;
    }

    public void SetAttachedFloor(bool att)
    {
        attachedFloor = att;
    }



    void OnTriggerStay2D(Collider2D other)
    {
        if(other.gameObject.tag == "Platform" || other.gameObject.tag == "Border" || other.gameObject.tag == "Bridge" )
        {
            inUpperPlatform = true;
        }
    }
    void OnTriggerExit2D(Collider2D other)
    {
        if(other.gameObject.tag == "Platform" || other.gameObject.tag == "Border" || other.gameObject.tag == "Bridge" )
        {
            inUpperPlatform = false;
        }
    }


}
