using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Text;

[System.Serializable]
public class PlayerStaticData : MonoBehaviour
{
    //스테이지 간 정보 보호를 위해 static처리

    //현재 세이브 슬롯 번호
    static int saveSlotNum;

    static bool init = false; // 캐릭터 최초 생성될 때 값 초기화용. true면 이미 초기화 한번 한거라 다시 작동 안함.
    //플레이어 스탯
    static float playerMaxHP;
    static float playerHP;

    //스킬 획득 여부
    static bool tentacleAttack;
    static bool tentacleJump;

    //현재 스테이지 위치
    static int stageNum = 0;
    static Vector3 initPosition = Vector3.zero;

    public PlayerStaticData() { }
    //저장 데이터 업데이트용
    public static void PlayerDataUpdate()
    {
        //저장된 값이랑 애니메이션에 있는 값이랑 다르면 자동으로 같게 만듦.
        //스테이지 옮길때 누락되지 않게.
        if(HeroBehaviour.AttackPossible != tentacleAttack)
            HeroBehaviour.AttackPossible = tentacleAttack;
    }

    public static void InitialData()
    {
        // 새로운 게임 시작 시 값 초기화.
        if (!init)
        {
            init = true;
            playerMaxHP = 200;
            playerHP = 200;
            tentacleJump = false;
            tentacleAttack = false;
            stageNum = 1;
            initPosition = new Vector3(0f,-10f,0f);
        }
    }
    
    public static int GetSaveSlotNum()
    {
        return saveSlotNum;
    }
    public static void SetSaveSlotNum(int tempsaveSlotNum)
    {
        saveSlotNum = tempsaveSlotNum;
    }

    public static bool GetInit()
    {
        return init;
    }
    public static void SetInit(bool tempinit)
    {
        init = tempinit;
    }

    public static float GetPlayerMaxHP()
    {
        return playerMaxHP;
    }
    public static void SetPlayerMaxHP(float tempMaxHP)
    {
        playerMaxHP = tempMaxHP;
    }
    public static float GetPlayerHP()
    {
        return playerHP;
    }
    public static void SetPlayerHP(float tempHP)
    {
        playerHP = tempHP;
    }
    public static bool GetTentacleAttack()
    {
        return tentacleAttack;
    }
    public static void SetTentacleAttack(bool tenAttack)
    {
        tentacleAttack = tenAttack;
    }
    public static bool GetTentacleJump()
    {
        return tentacleJump;
    }
    public static void SetTentacleJump(bool tenJump)
    {
        tentacleJump = tenJump;
    }

    public static int GetStageNum()
    {
        return stageNum;
    }
    public static void SetStageNum(int tempstageNum)
    {
        stageNum = tempstageNum;
    }
    public static Vector3 GetInitPosition()
    {
        return initPosition;
    }
    public static void SetInitPosition(Vector3 tempPosition)
    {
        initPosition = tempPosition;
    }

    
}
