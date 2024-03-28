using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RaycastHitTirgger : MonoBehaviour
{
    string raycastTag;
    Vector3 raycastScriptPoint;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void OnTriggerEnter2D(Collider2D coll)
    {
        raycastTag = coll.tag;
        if(coll.tag == "Monster")
        {

        }
        else if(coll.tag == "MovingObject")
        {

        }
        else if(coll.tag == "")
        {

        }
        else // 그외 벽 "another"
        {

        }
    }
    public string GetRayTag()
    {
        return raycastTag;
    }
    public Vector3 GetRayPos()
    {
        return raycastScriptPoint;
    }
    public void SetRayPos(Vector3 rayPoint)
    {
        raycastScriptPoint = rayPoint;//히트 트리거로 위치 계산하면 표면에 닿지 않고 공중에 약간 떨어져서 걸림. 계산 오류 발생
    }
    
}
