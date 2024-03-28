using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class GameManager : MonoBehaviour
{   
    //SceneNumber
    public enum SceneNumber: int{startMenu, stage01, stage02,stage03, stage04,stage05Littel, stage06, stage07, recordMenu, creditsScene}

    static bool gameRestart = false;

    public GameObject PlayerCharacter;
    public GameObject PlatformPref;
    public GameObject JumpPlatPref;
    public static float playerY; // 게임 매니저가 플레이어 현재 높이 추적.
    
    const float standardFloor = 0.2489f; //기준이 되는 바닥의 높이. player.y값이 이 값이면 0m임.
    const float refUnit = 0.338f; //1m의 기준 단위값. 해당 값의 배수마다 1m시 상승함. 한 스테이지당 최대 500m, Y값으로 169;

    Quaternion defaultRotate;

    void Start()
    {
        //매 스테이지마다 초기화.
        PlayerData.stageHeightM = 0f;
        PlayerData.tempPreHeight =0f;

        defaultRotate = gameObject.transform.rotation;
        //음소거 확인용
        MuteFun();
    }

    void Update()
    {
        if(GameObject.FindWithTag("Player") != null)//게임 종료시 작동 안함.
        {
            playerY = GameObject.FindWithTag("Player").transform.position.y;           
            MaxHeight();
        }      

        if(gameRestart)
        {
            GameOverRestart();
        }
  
    }
    public static void GameOver()
    {
        if (PlayerData.heart < 10) // heart 없음
        {
            DestroyScript.AllDistroyCommend();

            PlayerData.HeightM += PlayerData.stageHeightM;
            if (PlayerData.HRecord == null)
                PlayerData.HRecord = new List<HeightRecord>();
            PlayerData.HRecord.Add(new HeightRecord());
            PlayerData.HRecord.Sort((x, y) => y.Height.CompareTo(x.Height));
            JsonSaveLoad.RecordSave(PlayerData.HRecord);

            GameOverScript.GameOverFunc();
        }
        else // heart 있음. 다시 시작
        {
           gameRestart = true;//GameOverRestart()
        }

    }
    public void GameOverRestart()// 죽었지만, Heart있는 경우. 다시 시작
    {
        gameRestart = false; //staic->instance용

        float camY = GameObject.FindWithTag("MainCamera").transform.position.y; // 카메라 상대 위치에서 다시시작.
        

        PlayerData.heart -= 10; //Heart 10만큼만 빼줌.
        PlayerCharacter.GetComponent<PlayerScirpt>().RigidVelocityZero();// Player 위치 조정 시 가속도 유지를 막기 위해, 가속도 초기화
        
        PlayerCharacter.transform.position = new Vector3(0, camY, 0);
        Instantiate(PlatformPref, new Vector3(0, camY- 1.7275f, 0), defaultRotate);
        Instantiate(JumpPlatPref, new Vector3(0, camY- 1.6615f, 0), defaultRotate);
    }
    public static void PrinceGameOver()
    {
        //어린왕자 씬 전용 게임오버.
        if(PlayerData.HRecord == null)
            PlayerData.HRecord = new List<HeightRecord>();
        PlayerData.HRecord.Add(new HeightRecord());
        PlayerData.HRecord.Sort((x, y) => y.Height.CompareTo(x.Height));
        JsonSaveLoad.RecordSave(PlayerData.HRecord);

        initData();//죽었으니 중간 저장 데이터 초기화한 후 중간 저장 없앰
        PlayerData.SetStgae0();//0은 중간 세이브 없음을 의미함. 버튼 비활성화용        
        JsonSaveLoad.DataSave("StateSaveData");//중간세이브 덮어씀
    }

    public static void StageClear()
    {
        PlayerData.HeightM += PlayerData.stageHeightM;
        PlayerData.stageNum++;

        JsonSaveLoad.DataSave("StateSaveData");
        StageClearScript.StageClearUI();

    }
    public static void ContinueLoadGame()
    {        
        JsonSaveLoad.DataLoad("StateSaveData");
        SceneManager.LoadScene(PlayerData.stageNum, LoadSceneMode.Single); 
    }
    public static void initData()
    {
        PlayerData.stageHeightM = 0f;
        PlayerData.HeightM = 0f;// 누적 최고 높이
        PlayerData.score = 0;
        PlayerData.heart = 30;
        PlayerData.stageNum = 1; // 현재 스테이지
        JsonSaveLoad.DataSave("StateSaveData");//덮어쓰기.
        if(JsonSaveLoad.SaveFileCheck("HeightRecord")) //파일 있으면 불러옴.
        {
           JsonSaveLoad.RecordLoad(); 
        }

    }
    public static void NextStage()
    {        
        if(SceneManager.GetActiveScene().buildIndex <= (int)SceneNumber.stage06)
            SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex + 1, LoadSceneMode.Single);
        
        if(SceneManager.GetActiveScene().buildIndex == (int)SceneNumber.stage07)
            SceneManager.LoadScene((int)SceneNumber.stage07, LoadSceneMode.Single);
    }



    //스테이지 최고 높이 기록용 함수. 캐릭터가 최고점을 찍고 밑으로 내려가도 값이 낮아지지는 않음
    void MaxHeight()
    { 
        PlayerData.stageHeightM = (playerY - standardFloor)/refUnit;
        if(PlayerData.stageHeightM < PlayerData.tempPreHeight)//최고 높이 기록용. 찍고 내려가도 변하지 않게해줌.
            PlayerData.stageHeightM = PlayerData.tempPreHeight;
        if(PlayerData.stageHeightM > 500f)//최대 높이 500
            PlayerData.stageHeightM = 500f;
        PlayerData.tempPreHeight = PlayerData.stageHeightM;
    }


    void MuteFun()
    {   
        //파일 있으면 불러옴.
        if(JsonSaveLoad.SaveFileCheck("SettingData"))
        {
            JsonSaveLoad.DataLoad("SettingData");            
        }
        else//파일 없으면 뮤트는 기본 false;
        {
            PlayerData.mute = false;
        }

        if(PlayerData.mute)//음소거 상태
        {
            AudioListener.pause = true;
        }
        else
        {
            AudioListener.pause = false;
        }
    }
}