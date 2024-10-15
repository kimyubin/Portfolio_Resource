using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class HP_Bar : MonoBehaviour
{
    public GameObject Player;
    public Image HPRed;
    public Text HPText;
    float HPmax;
    float HP;
    // Start is called before the first frame update
    void Start()
    {        
        HPmax = PlayerStaticData.GetPlayerMaxHP();//최대체력
        HP = PlayerStaticData.GetPlayerHP(); //현재 체력 갱신
    }

    // Update is called once per frame
    void Update()
    {
        HPmax = PlayerStaticData.GetPlayerMaxHP();//최대체력
        HP = PlayerStaticData.GetPlayerHP(); //현재 체력 갱신

        HPRed.fillAmount = HP/HPmax;
        HPText.text = string.Format("{0}/{1}",HP,HPmax);

    }

}
