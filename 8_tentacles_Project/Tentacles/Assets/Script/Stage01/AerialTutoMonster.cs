using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AerialTutoMonster : MonoBehaviour
{
    public GameObject monObject;
    bool AttackMonIn;
    Vector3 [] MonsterPosition;
    // Start is called before the first frame update
    void Start()
    {
        AttackMonIn = false;
        MonsterPosition = new Vector3[] {new Vector3(107.4674f,-15.22955f,1f),new Vector3(112.83f,-12.65f,1f),new Vector3(67.5f,-17.48f,1f),new Vector3(118.82f,-10.85f,1f),new Vector3(127.15f,-8.85f,1f),new Vector3(130.75f,-5.68f,1f)};
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
