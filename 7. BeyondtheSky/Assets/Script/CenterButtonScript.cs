using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class CenterButtonScript : MonoBehaviour
{
    public GameObject centerLine;
    float rightLimitX;
    float leftLimitX;
    float upLimitY;
    float downLimitY;

    Vector3 defaultPos;

    //좌우 버튼 간격 1080, X값 범위 좌측부터 0~1080
    //좌우 20% 간격이 최대 이동 제한 범위;
    //상하 버튼 간격 1920, Y값 범위 아래부터 0~1920
    //위쪽 30%, 아래쪽 10% 밖으로는 안나감.

    // Start is called before the first frame update
    void Start()
    {
        defaultPos= transform.position;  
        rightLimitX = Screen.width *0.8f;
        leftLimitX = Screen.width *0.2f;
        upLimitY = Screen.height * 0.7f;
        downLimitY = Screen.height * 0.1f;
        centerLine.SetActive(false);
    }
    public void OnTouchDown()
    {
        centerLine.SetActive(true);
    }
    public void OnTouchDrag()
    {
        centerLine.SetActive(true);

        Vector3 pos = Input.mousePosition;
        pos.z = defaultPos.z;

        //상하 좌우 이동 범위 제한
        if(pos.x < leftLimitX)
            pos.x = leftLimitX;
        if(pos.x > rightLimitX)
            pos.x = rightLimitX;
        if(pos.y > upLimitY)
            pos.y = upLimitY;
        if(pos.y < downLimitY)
            pos.y = downLimitY;

        GetComponent<RectTransform>().position = pos;
    }
    public void OnTouchUp()
    {
        centerLine.SetActive(false);

    }
    
}
