using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossStartPoint : MonoBehaviour
{
    public GameObject Boss;
    public GameObject VirtualCamera;
    // Start is called before the first frame update
    void OnTriggerEnter2D(Collider2D other)
    {
        //이미 파괴된 다음엔 작동안함.
        if(other.tag == "Player")
        {
            //가상 카메라 작동.
            VirtualCamera.GetComponent<BossCinema>().Cinematic();
            StartCoroutine("StartBoss");
            Destroy(gameObject,5f);//5초 후 파괴. 줌아웃 3초 후 줌인 시작 + 2초후 가상 카메라 파괴 

        }
    }
    IEnumerator StartBoss()
    {
        yield return new WaitForSeconds(4f);// 4초 후 보스 움직임.
        if(gameObject.name == "BossStartPoint1")
            Boss.GetComponent<MouseBoss>().SetStartBoss1();
        if(gameObject.name == "BossStartPoint2")
            Boss.GetComponent<MouseBoss>().SetStartBoss2();


    }
}
