using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StalactiteParent : MonoBehaviour
{
    Stalactite stal;
    bool playerIn;
    BoxCollider2D boxColl2D;
    // Start is called before the first frame update
    void Start()
    {
        stal = gameObject.GetComponentInChildren<Stalactite>();
        boxColl2D = GetComponent<BoxCollider2D>();
        Physics2D.IgnoreLayerCollision(9,15,true);
        Physics2D.IgnoreLayerCollision(10,15,true);
        playerIn = false;

    }

   
    void OnTriggerEnter2D(Collider2D other)
    {
        //플레이어가 들어오면 트리거 off
        if(other.tag == "Player")
        {
            playerIn = true;
            boxColl2D.enabled = false;
        }
    }
    public bool GetPlayerIn()
    {
        return playerIn;
    }
    public void SetPlayerIn(bool plIn)
    {
        playerIn = plIn;
    }
    public void SetBoxCollEnabled()
    {
        //트리거 켜짐과 동시에 다시 작동 가능 상태로
        boxColl2D.enabled = true;
        playerIn = false;
    }
    
    public void FallenDust()
    {
        GetComponentInChildren<DustScirpt>().FallenDust();
    }
    public void DustInisital()
    {
        GetComponentInChildren<DustScirpt>().DustInisital();
    }

}
