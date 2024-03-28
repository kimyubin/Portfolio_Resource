using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScoreItem : MonoBehaviour
{
    public int itemCategory2;
    enum category{defaultVal, cherry, gold}
    int myScore;
    // Start is called before the first frame update
    void Start()
    { 
        category cat = (category)itemCategory2;
        switch(cat)
        {
            case category.cherry:
                myScore = 10;
                break;
            case category.gold:
                myScore = 10;
                break;
            default: 
                myScore = 0;
                break;
        }
    }
  
    void OnTriggerEnter2D(Collider2D coll)
    {
        if(coll.tag =="Player")
        {
            PlayerData.SetScore(myScore);
            Destroy(gameObject);
        }            
    }

}
