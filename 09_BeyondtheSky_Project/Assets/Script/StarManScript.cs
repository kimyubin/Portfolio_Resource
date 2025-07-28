using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StarManScript : MonoBehaviour
{
    void Start()
    {
        //30%확률로 등장함. 
        if (Random.Range(0, 100) > 30)
            Destroy(gameObject);
    }
    // Update is called once per frame
    void Update()
    {
        gameObject.transform.Translate(Vector3.right * Time.deltaTime * 0.1f);
        if(transform.localPosition.y > 2f)
            Destroy(gameObject);
    }
}
