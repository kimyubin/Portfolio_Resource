using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TentacleJumpItem : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        // 공용으로 사용되는 UI 스크립트와 분리하기 위해 따로 만듦.
        if(other.tag == "Player")
        {
            PlayerStaticData.SetTentacleJump(true);
        }
    }
}
