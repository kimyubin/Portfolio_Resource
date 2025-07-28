using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StoneScript : MonoBehaviour
{
    int count;
    // Start is called before the first frame update
    void Start()
    {
        count = 0;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        Debug.Log(other.name);

        if(other.tag == "TentacleAttack")
            count++;
    }

    public bool GetStoneCount()
    {
        return (count >= 5);//5번 텐타클 어택하면 열림.
    }


}
