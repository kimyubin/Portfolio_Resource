using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerData : MonoBehaviour
{
    //저장용 변수
    public static float HeightM = 0f;// 누적 최고 높이
    public static int score = 0;
    public static int stageNum = 0; // 현재 스테이지
    public static int heart = 30; //life;
    public static List<HeightRecord> HRecord = null;
    public static bool mute = false;
    public static float stageHeightM = 0f; // 현재 스테이지 높이    
    public static float tempPreHeight = 0f; //높이 비교용 변수

    
    public static void SetStgae0()
    {
        stageNum = 0;
    }
    public static void SetScore(int s)
    {
        score += s;
        heart += s/10;
        heart = Mathf.Clamp(heart,0,30);
    }
    public static void SetMute(bool m)
    {
        mute = m;
    }
    public static List<HeightRecord> GetHeightRecords()
    {
        return HRecord;
    }
    public static void SetHeightRecords(List<HeightRecord> hr)
    {
        HRecord = hr;
    }
    public static void SetLoadData(float h, int sc,int hrt, int stage)
    {
        HeightM = h;
        score  = sc;
        heart = hrt;
        stageNum = stage;
    }
}
