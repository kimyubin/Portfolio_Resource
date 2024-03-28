using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossCinema : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public void Cinematic()
    {
        gameObject.SetActive(true);
        StartCoroutine("ObjectActive");
    }

    IEnumerator ObjectActive()
    {
        yield return new WaitForSeconds(3f);// 줌아웃 3초 후 줌인시작.
        gameObject.SetActive(false);
        Destroy(gameObject,2f);//줌인 작동 후 2초 뒤에 오브젝트 파괴.
    }

}
