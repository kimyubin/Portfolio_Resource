using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SaveMenuScript : MonoBehaviour
{
    static int buttonOn; // 현재 선택된 버튼(오브젝트 간 공유 static)
    const int buttonTotal = 5; // 총 버튼 수
    int xButton; //이전 버튼 넘버 정보. 버튼이 달라질 때 소리를 내기 위함.
    const int initNum = -1; //초기화 숫자.
    JsonSaveLoad saveLoad ;

    // Start is called before the first frame update
    void Start()
    {
        buttonOn = initNum; // 초기화
        xButton = initNum; // 초기화
        saveLoad = GetComponentInParent<JsonSaveLoad>();

    }

    // Update is called once per frame
    void Update()
    {
        //방향키로 버튼 이동
        if (Input.GetKeyDown (KeyCode.UpArrow))
        {
            buttonOn --;
            if(buttonOn < 0)
                buttonOn = (buttonTotal-1);
            buttonOn %= buttonTotal;//방향키 이동시 반복 선택
        }
        else if (Input.GetKeyDown (KeyCode.DownArrow)) 
        {
            buttonOn ++;
            buttonOn %= buttonTotal;//방향키 이동시 반복 선택
        }

        //ESC키로 이전 화면으로 복귀.
        if(Input.GetKeyUp(KeyCode.Escape))
        {
            GetComponentInParent<Canvas>().transform.Find("StartMainPanel").gameObject.SetActive(true);
            gameObject.SetActive(false);
        }
        
        //스페이스와 엔터키로 이동
        if (Input.GetKeyUp(KeyCode.KeypadEnter) || Input.GetKeyUp(KeyCode.Return) || Input.GetKeyUp(KeyCode.Space))
            ButtonRun();

        //이전과 다른 버튼 선택시 소리 출력.
        if(buttonOn != xButton )
            GetComponent<AudioSource>().Play();

        xButton = buttonOn; //이전 프레임 버튼 정보 저장.
        

    }

    // 각 버튼 별 기능
    public void ButtonRun()
    {
        if (buttonOn != 4)
        {
            //아직 무조건 덮어씌워서 저장됨.
            PlayerStaticData.InitialData(); // 처음 지점으로 데이터 초기화.
            saveLoad.SaveFunc(buttonOn);
            SceneManager.LoadScene(PlayerStaticData.GetStageNum(), LoadSceneMode.Single);

        }
        switch (buttonOn)
            {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    GetComponentInParent<Canvas>().transform.Find("StartMainPanel").gameObject.SetActive(true);
                    gameObject.SetActive(false);
                    break;
                default:
                    break;
            }
    }
    public static void SetButtonON(int i)
    {
        buttonOn = i;
    }

    public static int GetButtonON()
    {
        return buttonOn;
    }
}
