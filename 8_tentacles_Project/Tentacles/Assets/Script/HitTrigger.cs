using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HitTrigger : MonoBehaviour
{
    public float graceTimeMax; //피격무적시간
    public float hitRecoveryMax;  //피격시 조작불가시간

    float graceTime; //남은 피격무적시간
    float hitRecoveryTime; //남은 피격시 조작불가시간
    
    float HP; //남은체력
    public float HP_MAX; //최대체력

    
    bool isHurted; // 피격시 getHurt로 가져가기 전까지 true.
    Vector3 opposite;
    bool oppositeRL;

    // Start is called before the first frame update
    void Start()
    {
        HP = HP_MAX;
        hitRecoveryTime = graceTime = -1f;
        isHurted = false;

        if(name == "PlayerCharacter")
        {
    
            HP_MAX = PlayerStaticData.GetPlayerMaxHP();
            HP = PlayerStaticData.GetPlayerHP();
        }
    }

    void timeCal()
    {
        if (graceTime > 0)
            graceTime -= Time.deltaTime;
        if (hitRecoveryTime > 0)
            hitRecoveryTime -= Time.deltaTime;
    }

    public bool getHitRecovery()
    {
        return hitRecoveryTime > 0;
    }

    public bool getGrace()
    {
        return graceTime > 0;
    }

    public float[] GetHP()
    {
        //최대 체력과 현재 체력 리턴
        return new float[] {HP_MAX,HP};
    }

    public bool isDead()
    {
        bool tempBool = false;
        if(name=="PlayerCharacter")
            tempBool = (PlayerStaticData.GetPlayerHP() <= 0);
        else
            tempBool =(HP <= 0);

        return tempBool;
    }

    public void Attacked(float damage, bool oppositeRL)
    {  
        if(name =="PlayerCharacter")
            PlayerStaticData.SetPlayerHP((PlayerStaticData.GetPlayerHP() - damage));
        else
            HP -= damage;
        StartCoroutine("hitRecovery");
        StartCoroutine("grace");
        isHurted = true;
        this.oppositeRL = oppositeRL;
    }
    

    public bool getOppositeRL()
    {
        return oppositeRL;
    }

    public bool getHurt()
    {
        if (isHurted)
        {
            isHurted = false;
            return true;
        }
        return false;
    }

    IEnumerator hitRecovery()
    {
        hitRecoveryTime = hitRecoveryMax;
        while (hitRecoveryTime > 0)
        {
            hitRecoveryTime -= Time.deltaTime;
            yield return null;
        }
    }

    IEnumerator grace()
    {
        graceTime = graceTimeMax;
        while (graceTime > 0)
        {
            graceTime -= Time.deltaTime;
            yield return null;
        }
    }
}
