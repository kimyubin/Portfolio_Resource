using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Rail45BackTrigger : MonoBehaviour
{

    bool backRail; // 전방에 레일이 있으면 True, 없으면 false. 없으면 이동 정지.
    // Start is called before the first frame update
    void Start()
    {
        backRail=false;
    }

    // Update is called once per frame
    void Update()
    {
        //Time.timeScale  =0.2f;
    }
       
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.gameObject.tag == "Rail" )//&& GetComponentInParent<PlayerCharacter>().GetComponentInChildren<PlayerFeet>().GetInRail())
        {   
            backRail = true;
        }             
    }
    void OnTriggerExit2D(Collider2D other)
    {
        if(other.gameObject.tag == "Rail")
        {   
            backRail = false;
        }             
    }

    public bool GetBackRail()
    {
        return backRail;
    }
}
