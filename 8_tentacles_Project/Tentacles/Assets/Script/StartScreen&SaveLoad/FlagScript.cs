using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;


public class FlagScript : MonoBehaviour
{    
    public bool nextStage;//스테이지 넘어가는 깃발이면 true; 저장용은 false; default는 false;
    public GameObject blue;
    JsonSaveLoad saveLoad;
    Vector3 flagPosition;//저장 장소 위치.
    int nowSceneNum;
    // Start is called before the first frame update
    void Start()
    {   
        saveLoad = GetComponent<JsonSaveLoad>();
        flagPosition = transform.position; 
        blue.SetActive(false);
        nowSceneNum = SceneManager.GetActiveScene().buildIndex;
    }

    // Update is called once per frame
    void Update()
    {


    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if (other.tag == "Player")
        {
            if (nextStage)//저장 및 다음 스테이지 이동.
            {
                switch (nowSceneNum + 1)
                {
                    case 2:
                        PlayerStaticData.SetInitPosition(new Vector3(-11, 0, 0));
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                    default:
                        break;
                }

                PlayerStaticData.SetStageNum(nowSceneNum + 1);

                saveLoad.SaveFunc(SaveMenuScript.GetButtonON());//해당 저장 슬롯에 저장.

                SceneManager.LoadScene(nowSceneNum + 1, LoadSceneMode.Single);
            }

            
            else if (!nextStage) // 저장 전용.
            {
                PlayerStaticData.SetStageNum(nowSceneNum);
                PlayerStaticData.SetInitPosition(flagPosition);

                saveLoad.SaveFunc(SaveMenuScript.GetButtonON());//해당 저장 슬롯에 저장.

                blue.SetActive(true);
            }
        }
    }
}