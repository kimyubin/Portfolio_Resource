using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StartManager : MonoBehaviour
{
    enum buttonNum {New, Load, Exit};
    static int buttonOn;
    int xButton; //이전 버튼 넘버 정보. 버튼이 달라질 때 소리를 내기 위함.
    const int initNum = -1; //초기화 숫자.
 
    // Start is called before the first frame update
    void Start()
    {
        buttonOn = initNum; // 초기화
        xButton = initNum; // 초기화
    }

    // Update is called once per frame
    void Update()
    {
        //방향키로 버튼 이동
        if (Input.GetKeyDown (KeyCode.UpArrow))
        {
            buttonOn --;
            if(buttonOn < 0)
                buttonOn = 2;
            buttonOn %= 3;//방향키 이동시 반복 선택
        }
        else if (Input.GetKeyDown (KeyCode.DownArrow)) 
        {
            buttonOn ++;
            buttonOn %= 3;//방향키 이동시 반복 선택
        }

        //스페이스와 엔터키로 이동
        if (Input.GetKeyUp(KeyCode.KeypadEnter) || Input.GetKeyUp(KeyCode.Return) || Input.GetKeyUp(KeyCode.Space))
        {
            ButtonRun();
        }

        //이전과 다른 버튼 선택시 소리 출력.
        if(buttonOn != xButton )
            GetComponent<AudioSource>().Play();

        xButton = buttonOn; //이전 프레임 버튼 정보 저장.

    }
    public void ButtonRun()
    {
        //각 패널 이동.
        switch (buttonOn)
            { 
                case (int)buttonNum.New:
                    GetComponentInParent<Canvas>().transform.Find("SavePanel").gameObject.SetActive(true);
                    gameObject.SetActive(false);
                    break;
                case (int)buttonNum.Load:
                    GetComponentInParent<Canvas>().transform.Find("LoadPanel").gameObject.SetActive(true);
                    gameObject.SetActive(false);
                    break;
                case (int)buttonNum.Exit:
                    #if UNITY_EDITOR
                        UnityEditor.EditorApplication.isPlaying = false;
                    #else
                        Application.Quit();
                    #endif
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
