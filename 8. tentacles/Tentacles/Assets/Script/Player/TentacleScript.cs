using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TentacleScript : MonoBehaviour
{
    float distanceY;
    float distanceX;
    float distanceHypotenuse;


    float radian;
    float angle;
    float radianToAngleConstant; // 라디안 -> 각도 계산용

    Vector3 targetPosition;
    SpriteRenderer spriteRend;
    // Start is called before the first frame update
    void Start()
    {
        spriteRend = GetComponent<SpriteRenderer>();
        radianToAngleConstant = (180 / Mathf.PI);
        spriteRend.enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (GetComponentInParent<TentacleJumpTrigger>().GetTentacleJumping())
        {

            spriteRend.enabled = true;

            targetPosition = GetComponentInParent<TentacleJumpTrigger>().GetTargetPosition();

            distanceX = targetPosition.x - GetComponentInParent<TentacleJumpTrigger>().GetComponentInParent<PlayerCharacter>().transform.position.x; // 플레이어와 목표까지 거리 x값
            distanceY = GetComponentInParent<TentacleJumpTrigger>().GetComponentInParent<PlayerCharacter>().transform.position.y - targetPosition.y; // 목표까지 높이 y값

            if(GameObject.Find("PlayerCharacter").GetComponent<PlayerCharacter>().GetRightLeft())//오른쪽
                radian = Mathf.Atan2(distanceY, distanceX);
            else //왼쪽은 방향 및 각도 전환
                radian = - Mathf.Atan2(distanceY, -distanceX);
                
            
            angle = radian * radianToAngleConstant;
            distanceHypotenuse = Mathf.Sqrt(Mathf.Pow(distanceX, 2) + Mathf.Pow(distanceY, 2)); // 플레이어 중심으로부터 플랫폼까지의 직선 거리(빗변 길이)

            gameObject.transform.rotation = Quaternion.AngleAxis(-angle, Vector3.forward); // 스프라이트 각도 수정
            gameObject.transform.localScale = new Vector3(distanceHypotenuse, 0.5f, 0); // 스프라이트 길이 수정

            if( (angle >= 85f) || (angle <= -85f) ) // 일정 각도가 된다면 자동으로 스프라이트 disalbe; 
                spriteRend.enabled = false;
        }
        else
        {
            spriteRend.enabled = false;
        }
    }
}
