using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class EndCreditScript : MonoBehaviour
{
    public GameObject credits;
    float finishPoint;
    bool timeScale;
    public float speed;
    
    // Start is called before the first frame update
    void Start()
    {
        finishPoint = 1920/2f;//5423+987;//(5423 == 4436.344) 실제 값에 987을 더해줘야함.
        timeScale = true;
    }

    // Update is called once per frame
    void Update()
    {
        if(credits != null)
            CreditRun();

    }
    void CreditRun()
    {
        if(credits.transform.position.y < finishPoint)
        {
            credits.transform.Translate(Vector3.up * Time.deltaTime * speed);
        }
    }
    public void MouseButton()
    {
        if(name == "Back")
        {
            SceneManager.LoadScene((int)GameManager.SceneNumber.startMenu, LoadSceneMode.Single);//초기화면으로 넘어감.
        }
        if(name == "Stop")
        {
            if(timeScale)
            {
                Time.timeScale = 0f;
                timeScale = !timeScale;
            }
            else
            {
                Time.timeScale = 1f;
                timeScale = !timeScale;

            }
        }
    }
}
