using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RailFrontTrigger : MonoBehaviour
{

    bool forntRail;
    // Start is called before the first frame update
    void Start()
    {
        forntRail=false;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void FixedUpdate()
    {
        //슬라이더에 타면, 자동으로 오른쪽으로 이동. PlayerCharator에서 inRail이 true면 좌우 이동 불가하게 만듦
        //왼쪽으로 접근했을때도 이동하는 걸 방지
        if(forntRail)
        {
            GetComponentInParent<PlayerCharacter>().transform.Translate(Vector3.up * 7.5f * Time.deltaTime);
        }
        
    }
    
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.gameObject.tag == "Rail" )//&& GetComponentInParent<PlayerCharacter>().GetComponentInChildren<PlayerFeet>().GetInRail())
        {   
            forntRail = true;
        }             
    }
        void OnTriggerExit2D(Collider2D other)
    {
        if(other.gameObject.tag == "Rail")
        {   
            forntRail = false;
        }             
    }
    public bool GetFrontRail()
    {
        return forntRail;
    }
}
