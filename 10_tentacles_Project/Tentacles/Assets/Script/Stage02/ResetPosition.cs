using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResetPosition : MonoBehaviour
{
    public  GameObject restartObject;
    Vector3 restartPoint;
    // Start is called before the first frame update
    void Start()
    {
        restartPoint = restartObject.transform.position;
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        // 실수로 떨어지면 위치 초기화
        if(other.tag == "Player")
        {
            other.transform.position = restartPoint;
        }

    }
}
