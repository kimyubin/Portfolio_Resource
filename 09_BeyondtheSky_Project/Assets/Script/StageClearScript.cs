using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StageClearScript : MonoBehaviour
{
    public GameObject AllUI;
    public GameObject canvas;
    public GameObject ScoreMessage;
    public GameObject ScoreText;
    public GameObject HeightMessage;
    public GameObject HeightText;

    public GameObject Button;

    static bool stageClearRun;
    
    // Start is called before the first frame update
    void Start()
    {
        stageClearRun = false;
        canvas.SetActive(false);
        ScoreMessage.SetActive(false);
        ScoreText.SetActive(false);
        HeightMessage.SetActive(false);
        HeightText.SetActive(false);
        Button.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        if(stageClearRun)
        {
            AllUI.SetActive(false);
            StartCoroutine("ClearEvent");
        }
        
    }
    public static void StageClearUI()
    {
        stageClearRun = true;
    }
    IEnumerator ClearEvent()
    {
        Time.timeScale = 0;

        float messageOnTime = 1f;

        float countTime = 1.5f;
        float target = PlayerData.score;
        float current = 0;
        float offset = (target-current)/countTime;
        

        canvas.SetActive(true);
        
        //딜레이 후 활성화
        StartCoroutine(Delay(0, messageOnTime, ScoreMessage));        

        //딜레이 후 활성화
        StartCoroutine(Delay(0, messageOnTime*2, ScoreText));

        //숫자 카운팅
        StartCoroutine(CountingNum(countTime,messageOnTime*2, true)); //true = Score

        //딜레이 후 활성화
        StartCoroutine(Delay(0, messageOnTime*3, HeightMessage));

        //딜레이 후 활성화
        StartCoroutine(Delay(0, messageOnTime*4, HeightText));

        //숫자 카운팅
        StartCoroutine(CountingNum(countTime,messageOnTime*4, false));          //false = Height
        //딜레이 후 활성화.
        StartCoroutine(Delay(0, messageOnTime*6, Button));

        stageClearRun = false;
        yield return null;
    }
    IEnumerator Delay(float refTime, float messageOnTime, GameObject gObj)
    {
        while (refTime < messageOnTime)
        {
            refTime += Time.unscaledDeltaTime;
            yield return null;
        }
        gObj.SetActive(true);
    }

    IEnumerator CountingNum(float countTime, float messageOnTime, bool ScoreHeight)//score or Height;
    {
        float target;
        float current;
        float offset;
        float refTime = 0;
        if(ScoreHeight)
        {
            target = PlayerData.score;
            current = 0;
            offset = (target - current) / countTime;

            while (refTime < messageOnTime)
            {
                refTime += Time.unscaledDeltaTime;
                yield return null;
            }

            while (current < target - 10)
            {
                current += offset * Time.unscaledDeltaTime;
                ScoreText.GetComponent<Text>().text = current.ToString("N0");
                yield return null;
            }
            current = target;
            ScoreText.GetComponent<Text>().text = current.ToString("N0");
        }
        else
        {
            target = PlayerData.HeightM;
            current = 0;
            offset = (target - current) / countTime;

            while (refTime < messageOnTime)
            {
                refTime += Time.unscaledDeltaTime;
                yield return null;
            }

            while (current < target - 10)
            {
                current += offset * Time.unscaledDeltaTime;
                HeightText.GetComponent<Text>().text = current.ToString("N0") + "m";
                yield return null;
            }
            current = target;
            HeightText.GetComponent<Text>().text = current.ToString("N0") + "m";
        }
    }
}
