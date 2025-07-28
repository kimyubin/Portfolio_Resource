using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OutHeadScript : MonoBehaviour
{
    public GameObject headDefPos;
    public float headSpeed;//머리 Y축 이동 속도
    // Start is called before the first frame update
    void Start()
    {
        transform.position = headDefPos.transform.position;//최초 시작 시 머리 위치에서 시작.
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void FixedUpdate()
    {
        HeadUpDown();
    }

    void HeadUpDown()
    {

        Vector3 headTarget = headDefPos.transform.position;//머리 표준 높이의 월드 포지션

        transform.position = Vector3.Lerp(transform.position,headTarget,Time.deltaTime * headSpeed);

    }
}
