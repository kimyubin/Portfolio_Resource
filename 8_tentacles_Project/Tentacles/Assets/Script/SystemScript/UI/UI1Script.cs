using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI1Script : MonoBehaviour
{
    bool touchThis;

    public float delayTime; // 대기 시간. 이 시간이 흐른 후에 pressAnykey 가 뜸.
    public GameObject messageGUI;
    public GameObject pressAnyKey;
    // Start is called before the first frame update
    void Start()
    {
        messageGUI.SetActive(false);
        pressAnyKey.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        ItemGet();
    }

    void ItemGet()
    {
        if(touchThis)
            StartCoroutine("DelayItem");

    }
    IEnumerator DelayItem()
    {
        float elapsedTime = 0f; 

        Time.timeScale = 0f;
        messageGUI.SetActive(true);

        while(elapsedTime < delayTime) 
        { 
            yield return null; 
            elapsedTime += Time.unscaledDeltaTime;         
        }
        touchThis = false;
        pressAnyKey.SetActive(true);
        
        // 지정된 시간뒤에 시간을 흐르게 하고 오브젝트 파괴.
        if(Input.anyKeyDown)
        {
            messageGUI.SetActive(false);
            pressAnyKey.SetActive(false);
            Time.timeScale = 1f;
            Destroy(gameObject);
        }        

    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.tag == "Player")
        {
            touchThis = true;
        }
    }

}
