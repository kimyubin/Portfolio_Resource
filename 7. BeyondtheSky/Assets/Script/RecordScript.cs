using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;



public class RecordScript : MonoBehaviour
{
    public GameObject rank;
    public GameObject Height;
    public GameObject score;
    public GameObject date;
    public GameObject medals;
    static int recordCount;
    static bool downCount; // 빈 랭킹보드를 뒤로 넘기는 것을 방지. 랭킹이 있어야 뒤로 넘어갈 수 있음.
    
    // Start is called before the first frame update
    void Start()
    {  
        downCount = true;
        recordCount = 0;  
        if(JsonSaveLoad.SaveFileCheck("HeightRecord"))
            JsonSaveLoad.RecordLoad();        
    }

    // Update is called once per frame
    void Update()
    {
        RecordOut();
        if(recordCount == 0)
        {
            medals.SetActive(true);
        }
        else
        {
            medals.SetActive(false);
        }
    }
    void RecordOut()
    {
        if(!JsonSaveLoad.SaveFileCheck("HeightRecord"))
            return;
        
        //순위표는 1~10, 데이터는 0~9. 
        for(int i = 0; i < 10; i++)
        {
            //기록 데이터 전체 갯수가 현재 페이지 * 10 + i개보다 클때(=데이터가 존재하는 만큼만)표기 아니면 '-'만 표기
            if((PlayerData.GetHeightRecords().Count) >  recordCount * 10 + i)
            {                
                //자식 Text array가 각 부모 안에 있는 Text컴포넌트를 포함하기 때문에 +1
                //레코드 카운트가 0부터 시작해서 rank는 + 1
                //레코드 0~9를 1~10으로 생각해서 집어넣음.
                rank.GetComponentsInChildren<Text>()[i + 1].text = (recordCount * 10 + i + 1).ToString();
                Height.GetComponentsInChildren<Text>()[i + 1].text = PlayerData.GetHeightRecords()[(recordCount * 10) + i].Height.ToString("N0") + "m";
                score.GetComponentsInChildren<Text>()[i + 1].text = PlayerData.GetHeightRecords()[(recordCount * 10) + i].score.ToString();
                date.GetComponentsInChildren<Text>()[i + 1].text = PlayerData.GetHeightRecords()[(recordCount * 10) + i].saveDate.ToString();

                downCount = true;
            }
            else
            {
                rank.GetComponentsInChildren<Text>()[i + 1].text = "-";
                Height.GetComponentsInChildren<Text>()[i + 1].text = "-";
                score.GetComponentsInChildren<Text>()[i + 1].text = "-";
                date.GetComponentsInChildren<Text>()[i + 1].text = "-";

                downCount= false;
            }
        }
    }
    public static void UpCount()
    {
        -- recordCount;
        if(recordCount < 0)
            recordCount = 0;
        
    }
    public static void DownCount()
    {
        if(downCount)
        {
            ++recordCount;
            if (recordCount > 9)
                recordCount = 9;
        }
        
    }

    
}
