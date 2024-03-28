using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Text;
using System;


public class JsonSaveLoad : MonoBehaviour
{
    void CreateJsonFile(int saveSlot, string SaveData)
    {
        FileStream fileStream = new FileStream(string.Format("{0}/{1}.json",Application.dataPath, saveSlot), FileMode.Create);
        byte[] data = Encoding.UTF8.GetBytes(SaveData);
        fileStream.Write(data,0,data.Length);
        fileStream.Close();
    }

    public PlayerSaveData LoadJsonFile<PlayerSaveData>(int saveSlot)
    {
        FileStream fileStream = new FileStream(string.Format("{0}/{1}.json",Application.dataPath,saveSlot), FileMode.Open);
        byte[] data = new byte[fileStream.Length];
        fileStream.Read(data,0,data.Length);
        fileStream.Close();

        string jsondata = Encoding.UTF8.GetString(data);
        return JsonUtility.FromJson<PlayerSaveData>(jsondata);
    }

    public void SaveFunc(int saveSlot)
    {
        PlayerSaveData saveData = new PlayerSaveData(); //생성자에서 현재 상태 데이터로 자동으로 초기화.
        string jsonData = JsonUtility.ToJson(saveData); // object to json
        CreateJsonFile(saveSlot,jsonData);
    }
    public void LoadFunc(int saveSlot)
    {
        PlayerSaveData loadData = LoadJsonFile<PlayerSaveData>(saveSlot);
        loadData.PlayerLoadData();
    }
    
    
    
    
    //세이브 파일 존재 유무 확인
    public bool SaveFileCheck(int slotNum)
    {
        string file = string.Format("{0}/{1}.json",Application.dataPath,slotNum);
        FileInfo info = new FileInfo(file);
        return info.Exists;
    }
}
//저장용 데이터 클래스
[System.Serializable]
public class PlayerSaveData
{
    //저장 시간
    public string saveTime;
    public int saveSlotNum;
    public bool init;
    //플레이어 스탯
    public float playerMaxHP;
    public float playerHP;
    //스킬 획득 여부
    public bool tentacleAttack;
    public bool tentacleJump;
    //현재 스테이지 위치
    public int stageNum;
    public Vector3 initPosition;

    //default/load

    //save
    public PlayerSaveData()
    {
        saveTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
        this.saveSlotNum = PlayerStaticData.GetSaveSlotNum();
        this.init = PlayerStaticData.GetInit();
        this.playerMaxHP = PlayerStaticData.GetPlayerMaxHP();
        this.playerHP = PlayerStaticData.GetPlayerHP();
        this.tentacleAttack = PlayerStaticData.GetTentacleAttack();
        this.tentacleJump = PlayerStaticData.GetTentacleJump();
        this.stageNum = PlayerStaticData.GetStageNum();
        this.initPosition = PlayerStaticData.GetInitPosition();
    }

    // 저장용 데이터 클래스 to 플레이어 캐릭터 정보
    public void PlayerLoadData()
    {
        PlayerStaticData.SetSaveSlotNum(this.saveSlotNum);
        PlayerStaticData.SetInit(this.init);
        PlayerStaticData.SetPlayerMaxHP(this.playerMaxHP);
        PlayerStaticData.SetPlayerHP(this.playerHP);
        PlayerStaticData.SetTentacleAttack(this.tentacleAttack);
        PlayerStaticData.SetTentacleJump(this.tentacleJump);
        PlayerStaticData.SetStageNum(this.stageNum);
        PlayerStaticData.SetInitPosition(this.initPosition);
    }

}
