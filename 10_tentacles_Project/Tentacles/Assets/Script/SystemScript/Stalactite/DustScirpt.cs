using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DustScirpt : MonoBehaviour
{
    BoxCollider2D boxColl2d;
    Rigidbody2D rigid;
    Vector3 intialPosition;
    // Start is called before the first frame update
    void Start()
    {
        boxColl2d = GetComponent<BoxCollider2D>();
        rigid = GetComponent<Rigidbody2D>();
        intialPosition = transform.position;
        rigid.isKinematic = true;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public void FallenDust()
    {
        // 떨어짐
        rigid.isKinematic = false;
    }
    public void DustInisital()
    {
        //위치 초기화 및 위치 고정.
        rigid.isKinematic = true;
        rigid.velocity = new Vector2(0,0);
        transform.position = intialPosition;


    }

}
