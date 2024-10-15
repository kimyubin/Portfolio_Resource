using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public class BossHPBar : MonoBehaviour
{
    MouseBoss Mon;
    public Image HPRed;
  
    float HPmax;
    float HP;
    // Start is called before the first frame update
    void Start()
    {        
        Mon = GetComponentInParent<MouseBoss>();
        HPmax = Mon.GetComponentInChildren<HitTrigger>().GetHP()[0];//최대체력
        HP = Mon.GetComponentInChildren<HitTrigger>().GetHP()[1]; //현재 체력 갱신
    }

    // Update is called once per frame
    void Update()
    {
        HPmax = Mon.GetComponentInChildren<HitTrigger>().GetHP()[0];//최대체력
        HP = Mon.GetComponentInChildren<HitTrigger>().GetHP()[1]; //현재 체력 갱신
        HPRed.fillAmount = HP/HPmax;

        //좌우로 움직일때 반전되지 않게.
        if(GetComponentInParent<MouseBoss>().transform.localScale.x > 0) //오른쪽
            HPRed.transform.localScale = new Vector3(1,1,1);
        else
            HPRed.transform.localScale = new Vector3(-1,1,1);
    }

    public void SetHPBarRightLeft(bool rL)
    {
        
    }

}
