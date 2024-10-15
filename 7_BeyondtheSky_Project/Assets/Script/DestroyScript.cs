using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestroyScript : MonoBehaviour
{
    float deadLineLevel;
    static bool allDistroy;// 인스턴스 공유. 게임 종료 시 true가 되면 일괄 파괴.
    void Awake()
    {
        allDistroy = false;
    }
    // Start is called before the first frame update
    void Start()
    {
        deadLineLevel = 2; // 카메라 중심과의 거리; 여기부터 카메라 범위를 벗어남.
    }

    // Update is called once per frame
    void Update()
    {
        DeadLineDestroy();
    }

    public void DeadLineDestroy()
    {

        float cameraY = GameObject.FindWithTag("MainCamera").transform.position.y;

        if (tag == "Player")// 플레이어용
        {
            if (cameraY - gameObject.transform.position.y > deadLineLevel)
            {
                GameManager.GameOver();

                if (PlayerData.heart < 10) // heart 없음
                {
                    Destroy(gameObject);
                }
            }
        }
        else if (tag != "Player") // 기타 오브젝트용
        {
            if (cameraY - gameObject.transform.position.y > deadLineLevel)
                Destroy(gameObject);
            if (gameObject.transform.position.y >= 168.6f) // 최대 높이 이후는 자동 삭제(카메라 최대 높이 169에서 약간 낮은 구간 값. 169까지 생성하면 겹쳐서 클리어 불가 할 수 있음.)
                Destroy(gameObject);
        }
        if (allDistroy)//게임 종료시.
            Destroy(gameObject);

    }
    public static void AllDistroyCommend()
    {
        allDistroy = true;
    }

}
