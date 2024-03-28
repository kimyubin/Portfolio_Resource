using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Stalactite : MonoBehaviour
{
    public int damage;
    bool hitToggel; // 떨어질 때 한번만 공격. true 공격. 
    StalactiteParent parentCompo;
    Rigidbody2D rigid;
    SpriteRenderer SpriteRenderer;

    Vector3 originPosition;
    // Start is called before the first frame update
    void Start()
    {
        hitToggel = true;
        rigid = GetComponent<Rigidbody2D>();
        SpriteRenderer = GetComponent<SpriteRenderer>();
        parentCompo = GetComponentInParent<StalactiteParent>();
        originPosition = transform.position;

        rigid.isKinematic = true;
    }

    // Update is called once per frame
    void Update()
    {
        if(parentCompo.GetPlayerIn())
        {
            // 트리거 작동하면 떨어뜨림.
            parentCompo.SetPlayerIn(false);
            StartCoroutine("DustActive");

        }

        //땅에 박은 다음에, 다시 떨어지면 공격 가능하게 바뀜.
        if(!hitToggel && rigid.velocity.y < 0)
        {
            hitToggel = true;
        }
            
        if(rigid.velocity.y == 0)//땅에 떨어지면 공격 불가.
        {
            hitToggel = false;
        }
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        string collTag = other.gameObject.tag;

        //트리거에 닿을때 한번만 공격하기 위해 Enter로 바꿈.
        // 땅에 박으면 공격 안함. 다음 IF문에 리턴 있어서 먼저 배치.
        if(collTag== "Platform" || collTag == "Border" || collTag == "Bridge")
        {
            Physics2D.IgnoreLayerCollision(8,15,true);//떨어지고 나면 서로 간섭 불가
            hitToggel = false;
            StartCoroutine("InitialStalactite"); // 위치 초기화
            
        }

        if(hitToggel)        
        {
            HitTrigger opposite = other.gameObject.GetComponent<HitTrigger>();
            if (opposite == null) return; //HitTrigger가 없는 경우 피격대상이 아니므로 트리거 무시
            if (!opposite.getGrace()) //피격무적 상태인지 체크
                opposite.Attacked(damage, transform);
        }
    }

    IEnumerator InitialStalactite()
    {
        yield return new WaitForSeconds(2f);// 2초 후 깜빡거림
        StartCoroutine("Fliker");
        yield return new WaitForSeconds(2f);// 2초동안 깜빡거린 후 초기화
        parentCompo.SetPlayerIn(false);
        transform.position = originPosition;
        rigid.isKinematic = true;
        Physics2D.IgnoreLayerCollision(8,15,false);//위치 초기화 후 간섭 가능.

        parentCompo.DustInisital();//먼지도 초기화

        yield return new WaitForSeconds(2f);// 2초 후 트리거 작동 가능하게 변경.
        parentCompo.SetBoxCollEnabled();
    }
    IEnumerator DustActive()
    {
        parentCompo.FallenDust();
        yield return new WaitForSeconds(2f);// 먼지 떨어지고 2초후 떨어짐
        rigid.isKinematic = false; 

    }

    IEnumerator Fliker()
	{
        //2초동안 10번 깜빡임
		int count = 0;
		while (count <10)
		{
			if(count % 2 == 0)
				SpriteRenderer.color = new Color32(255,255,255,90);
			else
				SpriteRenderer.color = new Color32(255,255,255,180);
			
			yield return new WaitForSeconds(0.2f);
			count++;
		}
		SpriteRenderer.color = new Color32(255,255,255,255);
	}
}
