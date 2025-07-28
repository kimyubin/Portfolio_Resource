using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pattern2DropStone : MonoBehaviour
{
    public GameObject Mouse;
    Rigidbody2D rigid;
    bool positionChange;
    // Start is called before the first frame update
    void Start()
    {
        rigid = GetComponent<Rigidbody2D>();
        rigid.isKinematic = true;
        Physics2D.IgnoreLayerCollision(8,18,true);//플레이어 무시
        Physics2D.IgnoreLayerCollision(9,18,true);
        Physics2D.IgnoreLayerCollision(10,18,true);
        Physics2D.IgnoreLayerCollision(15,18,true);
        positionChange = true;

    }

    // Update is called once per frame
    void Update()
    {
        //보스 위를 계속 따라다니다가, 떨어질 때는 따라다니지 않음.
        if(positionChange)
            transform.position = new Vector3(Mouse.transform.position.x, transform.position.y, transform.position.z);
        //떨어지다가 종유석에 부딫히는 현상 방지.
        if(transform.position.y > -4)
            Physics2D.IgnoreLayerCollision(18,18,true);
        else
            Physics2D.IgnoreLayerCollision(18,18,false);

    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.gameObject.name == "MouseBoss")// && !other.isTrigger)
        {
            other.GetComponent<MouseBoss>().SetPhase2On();
            Destroy(gameObject,5f);
            //페이즈 2 작동 후 오브젝트 파괴.
        }
    }
    public void SetKinematic()
    {
        rigid.isKinematic = false;
        positionChange = false;
    }

}
