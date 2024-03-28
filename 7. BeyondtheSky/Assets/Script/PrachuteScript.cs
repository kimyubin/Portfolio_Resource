using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PrachuteScript : MonoBehaviour
{
    public GameObject BenchMarkPoint;
    float intevelX;
    float intevelY;
    float degree;

    float radToDegree;

    // Start is called before the first frame update
    void Start()
    {
        intevelX = BenchMarkPoint.transform.position.x - transform.position.x;
        intevelY = BenchMarkPoint.transform.position.y - transform.position.y;
        radToDegree = 180f/Mathf.PI;
    }

    // Update is called once per frame
    void Update()
    {
        // 기준점 방향으로 회전 시키기 위해 아크 탄젠트로 radian을 구한 후 degree변환. rotation 값으로 바꾸기 위해 - 90도해줌.
        // 라디안 변환식은 미리 계산해둬서 연산부하 줄임
        intevelX = BenchMarkPoint.transform.position.x - transform.position.x;
        intevelY = BenchMarkPoint.transform.position.y - transform.position.y;
        degree =  (Mathf.Atan2(intevelY,intevelX) * radToDegree) - 90f;       
        

        transform.rotation = Quaternion.Euler(transform.rotation.x, transform.rotation.y, degree);
        //0~180 degree
        //-90~90 회전
        
    }
}
