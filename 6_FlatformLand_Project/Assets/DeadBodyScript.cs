using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DeadBodyScript : MonoBehaviour
{
    static Vector3 headPosition;
    static Vector3 bodyPosition;
    static Quaternion bodyRotate;
    

    Rigidbody2D rigid;
    public float speed;
    

    // Start is called before the first frame update
    void Start()
    {
        rigid = GetComponent<Rigidbody2D>();
        Destroy(gameObject, 40f);

        if(name == "Head")
        {
            transform.position = headPosition;
        }
        else if(name == "Body")
        {
            transform.position = bodyPosition;
            transform.rotation = bodyRotate;
        }
    }

    // Update is called once per frame
    void Update()
    {        
    }
    void OnTriggerEnter2D(Collider2D coll)
    {
        if(coll.tag == "KillChain")
        {         
            Vector3 temp = transform.position - coll.transform.position;//공격받는 객체와 반대방향으로 튕김
            temp = temp.normalized;
            Vector2 temp2 = temp;
            Debug.Log(temp2);
            rigid.AddForce(temp2 * speed,ForceMode2D.Impulse);
        }
    }
    
    public static void SetDeadBodyRotate(Vector3 headPos, Vector3 bodyPos, Quaternion bodyRot)
    {
        headPosition = headPos;
        bodyPosition = bodyPos;
        bodyRotate = bodyRot;
    }
}
