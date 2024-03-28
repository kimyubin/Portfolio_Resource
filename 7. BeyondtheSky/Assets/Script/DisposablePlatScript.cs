using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DisposablePlatScript : MonoBehaviour
{
    Rigidbody2D rigid;
    BoxCollider2D boxColl;    
    SpriteRenderer render;
    float colorStart;
    float colorDestination;
    float refColor;
    float bgColor;
    float delta;//누적시간
    float speed;//점멸 속도

    bool touch;
    bool flickerChange;//점멸 구분용. true= 일반 점멸, false = 빠르게 점멸. 밟으면 빠르게 점멸함.

    // Start is called before the first frame update
    void Start()
    {
        rigid = GetComponent<Rigidbody2D>();
        boxColl = GetComponent<BoxCollider2D>();
        render = GetComponent<SpriteRenderer>();
        touch = false;
        colorStart = 0; // 빨강
        colorDestination = 255; // 옅은 빨강
        speed = 15f;
        refColor = colorDestination-colorStart;
        delta = 0f;
    }

    // Update is called once per frame
    void Update()
    {            
        //닿으면 깜빡거림.
        if(touch)
        {
            StartCoroutine(Destroy(0.2f));
            delta += Time.deltaTime * speed;
            if(delta > Mathf.PI)
                delta = 0;
            
            bgColor = (refColor * Mathf.Sin(delta)) + colorStart; // (목표-처음) * sin(누적시간(최대PI)) + 처음 -> 시간에 따라 0~PI-> 0~1~0 출력 -> 200~120~200
            render.color = new Color32((byte)(render.color.r*255),(byte)(render.color.g*255),(byte)(render.color.b*255),(byte)bgColor);
        }
    }
    IEnumerator Destroy(float limitTime)
    {
        yield return new WaitForSeconds(limitTime);
        rigid.isKinematic = false;
        boxColl.enabled = false;
    }   
    public void SetDisposTouch()
    {
        touch = true;
    }

}
