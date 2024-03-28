using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class TextUIScript : MonoBehaviour
{
    public Text HeghitUIText;
    public Text ScoreUIText;
    public Font font;
    float heghit;
    float score;
    void Awake()
    {
        font.material.mainTexture.filterMode = FilterMode.Point;//둥근 모꼴+Fixedsys 글꼴 픽셀 퍼펙트 처리; 스크립트로 한번 수정된 거라 주석처리

    }
    // Start is called before the first frame update
    void Start()
    {
        heghit = PlayerData.HeightM;
        score = PlayerData.score;        
        HeghitUIText.text = "Score : 0";
        ScoreUIText.text = "점수 : 0";
    }

    // Update is called once per frame
    void Update()
    {
        HeghitUIText.text = (PlayerData.HeightM + PlayerData.stageHeightM).ToString("N0")+"m";
        ScoreUIText.text =  PlayerData.score.ToString("N0");
    }
}
