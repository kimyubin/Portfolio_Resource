using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class PauseStartSuperScript : MonoBehaviour
{
    public GameObject pauseButton;
    public GameObject startButton;
    public GameObject pauseImage;
    public GameObject StageReset;
    public GameObject MainMenu;
    public Text pauseText;
    //시간 정지한지 구별용. 비상용 점프 함수 오류를 교정하기 위해.

    // Start is called before the first frame update
    void Start()
    {
        pauseButton.gameObject.SetActive(true);
        startButton.gameObject.SetActive(false);        
        pauseImage.gameObject.SetActive(false);
        pauseText.gameObject.SetActive(false);
        StageReset.gameObject.SetActive(false);
        MainMenu.gameObject.SetActive(false);
        StartCoroutine("FlickerEvent");

    }
    public void ButtonRun(int bNum)
    {
        switch (bNum)
        {
            case 1: // pause
                pauseButton.gameObject.SetActive(false);
                startButton.gameObject.SetActive(true);
                pauseImage.gameObject.SetActive(true);
                StageReset.gameObject.SetActive(true);
                MainMenu.gameObject.SetActive(true);
                Time.timeScale = 0f;
                break;
            case 2: // start
                pauseButton.gameObject.SetActive(true);
                startButton.gameObject.SetActive(false);
                pauseImage.gameObject.SetActive(false);
                StageReset.gameObject.SetActive(false);
                MainMenu.gameObject.SetActive(false);
                Time.timeScale = 1f;
                break;
            case 3: // StageReset
                Time.timeScale = 1f;
                GameManager.ContinueLoadGame();
                break;
            case 4: // MainMenu
                Time.timeScale = 1f;
                SceneManager.LoadScene((int)GameManager.SceneNumber.startMenu, LoadSceneMode.Single);

                break;
            default:
                break;
        }
    }
    IEnumerator FlickerEvent()
    {
        //delayTime간격으로 깜빡임
        float refTime = 0;
        float offTime = 0.15f;
        float onTime = 0.3f;

        pauseText.gameObject.SetActive(true);
        while (refTime < onTime)
        {
            yield return null;
            refTime += Time.unscaledDeltaTime;
        }
        refTime = 0;

        pauseText.gameObject.SetActive(false);
        while (refTime < offTime)
        {
            yield return null;
            refTime += Time.unscaledDeltaTime;
        }
        StartCoroutine("FlickerEvent");

    }
}
