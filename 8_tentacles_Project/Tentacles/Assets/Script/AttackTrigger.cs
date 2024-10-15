using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackTrigger : MonoBehaviour //공격에 대한 내용만 작성. 충돌 상대방이 HitTrigger를 가지고 있는경우 피격처리
{
    public float damage;
    bool isAttacking = false;
    bool rightleft; // 어디로 때렸는지. 왼쪽으로 때리면 왼쪽으로 날아감.

    // Start is called before the first frame update
    void Start()
    {
        rightleft = true;
        if (transform.rotation.eulerAngles.y == 180) rightleft = !rightleft;
        if (transform.localScale.x < 0) rightleft = !rightleft;
    }

    // Update is called once per frame
    void Update()
    {        
    }

    void OnTriggerStay2D(Collider2D collision)
    {
        HitTrigger opposite = collision.gameObject.GetComponent<HitTrigger>();
        if (opposite == null) return; //HitTrigger가 없는 경우 피격대상이 아니므로 트리거 무시
        if (!opposite.getGrace()) //피격무적 상태인지 체크
        {
            if(collision.transform.position.x > transform.position.x) //대상 위치 오른쪽
                rightleft = true;
            else
                rightleft = false;
            opposite.Attacked(damage, rightleft);            
        }
            
    }

    public void setDamage(float value)
    {
        damage = value;
    }
    
}
