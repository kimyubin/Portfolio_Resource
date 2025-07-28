using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class OptionUI : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        //ESC키로 옵션창 엶
        if(Input.GetKeyUp(KeyCode.Escape))
        {
            //꺼져있어서 GetComponentInChildren 대신 find사용. 컴포넌트가 아닌 오브젝트 명으로 찾음.
            Time.timeScale = 0f;//일시정지
            transform.Find("OptionPanel").gameObject.SetActive(true);
        }
    }
}
