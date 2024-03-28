using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Roamer : MonoBehaviour //RoamingRange를 이용하여 행동반경을 제약하는 경우 이 클래스를 상속. 
{
    public RoamingRange roamingRange;
    protected HitTrigger hitTrigger;
    protected AttackTrigger attackTrigger;
    protected Animator animator;
    protected bool knockbackAble;
    protected bool isFallen;
    protected void Start()
    {
        BoxCollider2D collider = roamingRange.GetComponent<BoxCollider2D>();
        hitTrigger = GetComponent<HitTrigger>();
        attackTrigger = GetComponent<AttackTrigger>();
        animator = GetComponent<Animator>();
        animator.SetFloat("RangeX", collider.transform.position.x); //이동범위 중심부 X좌표
        animator.SetFloat("RangeY", collider.transform.position.y); //이동범위 중심부 Y좌표
        animator.SetFloat("RangeW", collider.size.x * 0.5f); //이동범위 가로길이의 절반
        animator.SetFloat("RangeH", collider.size.y * 0.5f); //이동범위 세로길이의 절반
        knockbackAble = true;
        isFallen = false;
    }
    protected void FixedUpdate()
    {
        if (hitTrigger != null) Hurt();
        //setState();
    }
    /*protected abstract void setState();
    protected abstract void Move();
    public abstract void Rotate();*/
    void Hurt()
    {
        if (hitTrigger.getHurt())
        {
            animator.SetTrigger("isHurted");
            if(knockbackAble) animator.SetBool("knockbackRL", hitTrigger.getOppositeRL());
        }
        if (hitTrigger.isDead())
        {
            animator.SetBool("isDead", true);
            StartCoroutine("deadFall");
            Destroy(hitTrigger);
            Destroy(attackTrigger);
            hitTrigger = null;
        }
    }
    private void OnTriggerExit2D(Collider2D collision)
    {
        RoamingRange RR = collision.GetComponent<RoamingRange>();
        if (RR == null || !RR.Equals(roamingRange)) return;
            animator.SetBool("isOut",true);
        //Rotate();
    }
    private void OnCollisionStay2D(Collision2D collision)
    {
        if (collision.gameObject.tag == "Platform" || collision.gameObject.tag == "Border") 
            isFallen = true;
    }
    public bool getRightLeft()
    {
        return animator.transform.rotation.eulerAngles.y == 0;
    }
    IEnumerator deadFall()
    {
        isFallen = false;
        Vector2 moveV = new Vector2(0, -2.0f);
        while(!isFallen)
        {
            if (isFallen) Debug.Log("?");
            transform.Translate(moveV * Time.deltaTime, Space.World);
            yield return null;
        }
        GetComponent<Rigidbody2D>().simulated = false;
    }
}
