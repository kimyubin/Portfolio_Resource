using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StandbyScript : MonoBehaviour
{
    public GameObject numObj;
    public GameObject BlackImage;
    // Start is called before the first frame update
    void Start()
    {
        BlackImage.SetActive(true);
        numObj.GetComponent<Text>().text = "3";
        Time.timeScale = 0f;
        StartCoroutine("Standby");

    }
    IEnumerator Standby()
    {
        float startVal = 0.7f;
        float target = 1.3f;
        float refenceVal = target - startVal;
        float CumulatTime = 0;
        int num = 3;


        numObj.GetComponent<Text>().color = new Color32(255,255,255,255);
        
        while (num > 0)
        {
            numObj.GetComponent<Text>().rectTransform.localScale = new Vector3(startVal,startVal,1);
            while (CumulatTime < 1)
            {
                numObj.GetComponent<Text>().text = num.ToString();

                CumulatTime += Time.unscaledDeltaTime;
                //시간 계산용 누적값에 refval을 곱해서 비율을 구함. 시간이 1초라 그냥 뺀 값을 곱하는 형태를 취했음.

                numObj.GetComponent<Text>().rectTransform.localScale = new Vector3(startVal+(CumulatTime*refenceVal), startVal+(CumulatTime*refenceVal), 1);
                yield return null;
            }
            num--;

            CumulatTime = 0;

        }
        gameObject.SetActive(false);
        Time.timeScale = 1;
    }


}
