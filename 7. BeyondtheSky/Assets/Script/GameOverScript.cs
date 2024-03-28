using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class GameOverScript : MonoBehaviour
{
    public GameObject AllUI;
    static bool gameOverBool = false;
    
    public GameObject Canvas;
    public GameObject pauseImage;
    public GameObject MainMenuButton;
    public Text scoreText;
    public Text HeightText;
    public GameObject massageText;
    public GameObject Parachute;
    float parachutePoint;

    // Start is called before the first frame update
    void Start()
    {   
        StartCoroutine("FlickerEvent");
        parachutePoint = 6f;

        Canvas.gameObject.SetActive(false);
        
    }

    void Update()
    {
        //static으로 작동하기 위해 static변수로 실행시킴. 한번만 실행함.
        if (gameOverBool)
        {
            GetComponentInChildren<AdsScprit>().ShowAds();
            GameOverRun();
            gameOverBool = false;
        }        

    }
    public void GameOverRun()
    {
        AllUI.gameObject.SetActive(false);
        Canvas.gameObject.SetActive(true);
        if(Parachute.name == "Parachute")
        {
            parachutePoint = 6f;
        }
        else
        {
            parachutePoint = 2.2f;
        }
        Instantiate(Parachute, new Vector3(0f, GameObject.FindWithTag("MainCamera").transform.position.y + parachutePoint, 1f), gameObject.transform.rotation);

        HeightText.text = PlayerData.HeightM.ToString("N1")+"m";
        scoreText.text = PlayerData.score.ToString();
        
        GameManager.initData();//죽었으니 중간 저장 데이터 초기화한 후 중간 저장 없앰
        PlayerData.SetStgae0();//0은 중간 세이브 없음을 의미함. 버튼 비활성화용        
        JsonSaveLoad.DataSave("StateSaveData");//중간세이브 덮어씀
    }

    IEnumerator FlickerEvent()
    {
        //delayTime간격으로 깜빡임
        float refTime = 0;
        float onTime = 0.5f;
        float offTime = 0.15f;

        massageText.gameObject.SetActive(true);
        while (refTime < onTime)
        {
            yield return null;
            refTime += Time.unscaledDeltaTime;
        }
        refTime = 0;

        massageText.gameObject.SetActive(false);
        while (refTime < offTime)
        {
            yield return null;
            refTime += Time.unscaledDeltaTime;
        }
        StartCoroutine("FlickerEvent");

    }
    public static void GameOverFunc()
    {
        gameOverBool = true;
    }
}
