using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameProductManager : MonoBehaviour
{  
    public GameObject PlatformPref;
    public GameObject LRPref;
    public GameObject DisposPref;
    public GameObject JumpPlatPref;
    public GameObject ItemPref;    

    int jumpPercent = 5;// 발판 위에 점프대 생성 확률
    int defaultInterval; // 기본으로 생성되는 플랫폼 구간.
    
    Quaternion PlatformRotate;

    int refPlatformY;//발판 생성 구간용 변수
    int refItemY; // 아이템 생성 구간용 변수

    void Start()
    {        
        PlatformRotate = gameObject.transform.rotation;
        defaultInterval= 6;
        refPlatformY = 0;
        refItemY = 0;        

        //최초 발판 생성용
        FirstPlatformAuto();
        
    }

    void Update()
    {
        PlatformAutoProdu();
        RandomItemFunc();
    }
    
    //한번에 가로 20, 세로 50 총 1,000칸으로 쪼개서 생성.
    //따라서 기준 확률도 1,000이며, 비교용 상수는 한번에 생성되는 객체의 기댓값과 같음.

    
    //플랫폼 자동 생성 함수.
    void PlatformAutoProdu()
    {
        //랜덤 생성. 플레이어 위치.Y + 5~10 생성부터 시작함.
        if(GameManager.playerY >= refPlatformY)
        {
            //Player.y가 5의 배수에 도달할 때마다, 그 다음 5의 배수 위치(Player.Y+5)부터 그 다다음 5의 배수 위치(Player+5+5)까지 미리 랜덤 생성.
            for (int y = 0; y < 50; y++)
            {
                //플랫폼 기본 생성.
                if ((y % defaultInterval) == 0)
                {
                    int tempX = (Random.Range(-10,10));
                    Instantiate(PlatformPref, new Vector3(tempX * 0.1f, (y * 0.1f) + refPlatformY + 5f, 1f), PlatformRotate);
                    
                    if (Random.Range(0, 100) < jumpPercent)
                    {
                        Instantiate(JumpPlatPref, new Vector3(tempX * 0.1f, (y * 0.1f) + refPlatformY + 5f + 0.06f, 1f), PlatformRotate);
                    }
                }
                
                //보조 플랫폼 랜덤 생성.
                for (int x = -10; x < 11; x++)
                {

                    if (Random.Range(0, 1000) < 3)
                    {
                        Instantiate(PlatformPref, new Vector3(x * 0.1f, (y * 0.1f) + refPlatformY + 5f, 1f), PlatformRotate);
                        if (Random.Range(0, 100) < jumpPercent)
                        {
                            Instantiate(JumpPlatPref, new Vector3(x * 0.1f, (y * 0.1f) + refPlatformY + 5f + 0.06f, 1f), PlatformRotate);
                        }                            
                        x += 5;//연속 생성 방지.                  
                    }
                }
            }

            //기타 발판 생성(좌우, 1회용발판)(상하로 움직이는건 점프시 문제 발생해서 뺐음)
            for (int y = 0; y < 50; y++)
            {
                //같은 라인에 동시에 생성되지는 않음.
                if (Random.Range(0, 2) < 1)
                {
                    for (int x = -10; x < 0; x++)
                    {
                        if (Random.Range(0, 1000) < 3)
                        {
                            Instantiate(LRPref, new Vector3(x * 0.1f, (y * 0.1f) + refPlatformY + 5f, 1f), PlatformRotate);
                            x += 5;
                            y += 10;
                        }
                    }
                }
                else
                {
                    for (int x = -10; x < 10; x++)
                    {
                        if (Random.Range(0, 1000) < 3)
                        {
                            Instantiate(DisposPref, new Vector3(x * 0.1f, (y * 0.1f) + refPlatformY + 5f, 1f), PlatformRotate);
                            x += 5;
                            y += 10;
                        }
                    }

                }
            }            
            refPlatformY += 5;
        }  
    }

    void FirstPlatformAuto()
    {
        //최초 y=0~5까지 생성. 첫 y=0~1까지는 생성안함. 시작하자마자 캐릭터 발 밑에 생겨서 멈추는 현상 방지.
        for (int y = 10; y < 50; y++)
        {
            if ((y % defaultInterval) == 0)
            {
                int tempX = (Random.Range(-10,10));

                Instantiate(PlatformPref, new Vector3(tempX * 0.1f, (y * 0.1f), 1f), PlatformRotate);
                if (Random.Range(0, 100) < jumpPercent)
                {
                    Instantiate(JumpPlatPref, new Vector3(tempX * 0.1f, (y * 0.1f) + 0.06f, 1f), PlatformRotate);
                }
            }
            for (int x = -10; x < 11; x++)
            {
                if (Random.Range(0, 1000) < 6)
                {
                    Instantiate(PlatformPref, new Vector3(x * 0.1f, (y * 0.1f), 1f), PlatformRotate);
                    if (Random.Range(0, 10) < jumpPercent)
                    {
                        Instantiate(JumpPlatPref, new Vector3(x * 0.1f, (y * 0.1f) + 0.06f, 1f), PlatformRotate);
                    }                        
                    x += 5;
                }
            }
        }
    }

    //아이템 생성
    void RandomItemFunc()
    {
        if (GameManager.playerY >= refItemY)
        {
            for (int y = 0; y < 50; y++)
            {
                for (int x = -10; x < 11; x++)
                {
                    // 확률이 높은 대신, 생성되면 해당 X축은 더이상 생성 안되고, Y축도 다소 떨어지게 했음. 가급적이면 붙어서 생성되지 못하게.
                    if (Random.Range(0, 1000) < 2)
                    {
                        Instantiate(ItemPref, new Vector3(x * 0.1f, (y * 0.1f)+refItemY+5f, 1f), PlatformRotate);
                        y += 7;
                        break;//1줄에 한 개씩
                    }
                }
            }
            refItemY +=5;
        }
    }
    

}
