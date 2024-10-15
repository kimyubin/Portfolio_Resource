using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI2Script : MonoBehaviour
{
    bool startIn;
    bool endIn;
    float imagePositionX; // 이미지 최초 위치;
    float imageDestination; // 최종 목적지
    float imageRefValue; // 현재 위치로부터 목적지까지 남은 거리를 %로 계산하기 위한 상수, 최초 위치 - 목적지 위치. 음의 방향으로 움직이기 때문에 반대로 뺐음
    float convertPiValue; // imageRefValue를 파이 값으로 변환한 계수
    float trigValue; // 삼각함수 값
    float speed; // 움직이는 속도 조절 계수.

    public GameObject MessageGUI;
    public GameObject CanvasImage;
    // Start is called before the first frame update
    void Start()
    {
        startIn = false;
        endIn = false;
        MessageGUI.SetActive(false);
        imagePositionX = 867f;
        imageDestination = 455f;
        imageRefValue = imagePositionX-imageDestination;
        convertPiValue = (1/imageRefValue) * Mathf.PI * 0.5f;
        speed = 1400f;
        //cos[sin] (imageX - localX) /refValue * Pi/2
    }
    // Update is called once per frame
    void Update()
    {
        MessageOutput();
    }
    void MessageOutput()
    {
        if (GetComponentInChildren<StartTuto>().GetStartTutoIn() && !GetComponentInChildren<EndTuto>().GetEndTutoIn())
        {
            MessageGUI.SetActive(true);

            //자연스러운 속도를 위해 코사인 0 ~ Pi/2(1~0)값을 곱해줌. 연산 속도를 위해 미리 계산된 변수를 사용(convertPiValue).
            trigValue = Mathf.Cos((imagePositionX - CanvasImage.gameObject.GetComponent<RectTransform>().localPosition.x) * convertPiValue);
            if (CanvasImage.gameObject.GetComponent<RectTransform>().localPosition.x >= imageDestination)
            {
                CanvasImage.gameObject.GetComponent<RectTransform>().Translate(Vector3.left * Time.deltaTime * speed * trigValue);
            }

        }

        if(GetComponentInChildren<EndTuto>().GetEndTutoIn())
        {
            MessageGUI.SetActive(true);
            trigValue = Mathf.Sin((imagePositionX - CanvasImage.gameObject.GetComponent<RectTransform>().localPosition.x) * convertPiValue);
            if (CanvasImage.gameObject.GetComponent<RectTransform>().localPosition.x <= imagePositionX-1)
            {
                CanvasImage.gameObject.GetComponent<RectTransform>().Translate(Vector3.right * Time.deltaTime * speed * trigValue);
            }
            else
                Destroy(gameObject);
        }
    }
}
