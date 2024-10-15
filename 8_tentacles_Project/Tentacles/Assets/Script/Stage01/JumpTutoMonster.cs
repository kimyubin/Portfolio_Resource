using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JumpTutoMonster : MonoBehaviour
{
    public GameObject monObject;
    bool jumpMonsterIn;
    Vector3 [] MonsterPosition;
    // Start is called before the first frame update
    void Start()
    {
        jumpMonsterIn = false;
        MonsterPosition = new Vector3[] {new Vector3(19.5f,-17.48f,1f),new Vector3(22f,-17.48f,1f),new Vector3(22.5f,-17.48f,1f),new Vector3(31f,-17.48f,1f),new Vector3(32f,-17.48f,1f)};
    }

    // Update is called once per frame
    void Update()
    {
        if(jumpMonsterIn)
        {
            for(int i = 0; i < MonsterPosition.Length ; i++ )
            {
		        Instantiate(monObject,MonsterPosition[i],monObject.transform.rotation);
            }
            Destroy(gameObject,0f);
        }
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.tag == "Player")
        {
            jumpMonsterIn = true;
        }
    }
}
