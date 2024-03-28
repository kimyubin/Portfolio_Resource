using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackTutoMonster : MonoBehaviour
{
    public GameObject monObject;
    bool AttackMonIn;
    Vector3 [] MonsterPosition;
    // Start is called before the first frame update
    void Start()
    {
        AttackMonIn = false;
        MonsterPosition = new Vector3[] {new Vector3(63.5f,-17.48f,1f),new Vector3(66f,-17.48f,1f),new Vector3(67.5f,-17.48f,1f),new Vector3(75f,-17.48f,1f),new Vector3(78f,-17.48f,1f)};
    }

    // Update is called once per frame
    void Update()
    {
        if(AttackMonIn)
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
            AttackMonIn = true;
        }
    }
}
