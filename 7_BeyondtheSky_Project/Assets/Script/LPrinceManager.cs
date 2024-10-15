using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;


public class LPrinceManager : MonoBehaviour
{
    public GameObject Bubble;
    public GameObject PrinceText;
    public GameObject PrinceNoText;
    public GameObject FoxYesText;
    public GameObject FoxNoText;
    public GameObject ButtonImage;
    public GameObject Fox;

    Animator animator;

    float delay;
    float foxSpeed;

    int typingOrder;


    public static bool ButtonClick;
    public static bool yesNo;   



    // Start is called before the first frame update
    void Start()
    {
        Bubble.SetActive(false);
        PrinceText.SetActive(false);
        PrinceNoText.SetActive(false);
        FoxYesText.SetActive(false);
        FoxNoText.SetActive(false);
        ButtonImage.SetActive(false);

        ButtonClick= false;
        yesNo = false;

        delay = 0.1f;
        typingOrder = 1;
        animator = Fox.GetComponent<Animator>();
        animator.speed = 0.4f;
        foxSpeed = 0.35f;
    }

    // Update is called once per frame
    void Update()
    {
        if(Fox.transform.position.x > 0.5f)
        {
            Fox.transform.Translate(Vector3.left * Time.deltaTime * foxSpeed);
            animator.SetBool("animRunBool", true);
        }
        else
        {
            animator.SetBool("animRunBool", false);
            switch(typingOrder)
            {
                case 1:
                    typingOrder = 0;
                    StartCoroutine(Destination(PrinceText,PrinceText.GetComponent<Text>().text,2));
                    break;
                    
                case 2:
                    ButtonImage.SetActive(true);
                    if(ButtonClick)
                    {
                        typingOrder = 0;
                        ButtonImage.SetActive(false);
                        Bubble.GetComponent<RectTransform>().localScale = new Vector3(-1,1,1);//말풍선 반전.
                        if(yesNo)
                        {
                            StartCoroutine(Destination(FoxYesText,FoxYesText.GetComponent<Text>().text,3));
                        }
                        else
                        {
                            StartCoroutine(Destination(FoxNoText,FoxNoText.GetComponent<Text>().text,4));
                        }
                    }
                    break;
                    
                case 3://yes
                    GameObject.FindWithTag("MainCamera").transform.Translate(Vector3.up * Time.deltaTime * 0.1f);
                    if(GameObject.FindWithTag("MainCamera").transform.position.y > 3f)
                    {
                        GameManager.PrinceGameOver();//어린왕자 전용 게임오버
                        SceneManager.LoadScene((int)GameManager.SceneNumber.creditsScene, LoadSceneMode.Single);//크래딧으로 넘어감.
                    }

                    break;
                    
                case 4://no
                    typingOrder = 0;
                    Bubble.GetComponent<RectTransform>().localScale = new Vector3(1,1,1);//말풍선 완상복구.
                    StartCoroutine(Destination(PrinceNoText,PrinceNoText.GetComponent<Text>().text,5));
                    break;
                case 5://No2
                    SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex+1, LoadSceneMode.Single);
                    break;
                default :
                    break;
            }
            

            

        }


    }
    IEnumerator Typing(GameObject obj, string typeText, int type)
    {
        Bubble.SetActive(true);

        obj.SetActive(true);
        for (int i = 0; i < typeText.Length; i++)
        {
            obj.GetComponent<Text>().text = typeText.Substring(0, i + 1);
            yield return new WaitForSeconds(delay);
        }
        obj.GetComponent<Text>().text = typeText;
        yield return new WaitForSeconds(3f);
        typingOrder = type;
        obj.SetActive(false);
        Bubble.SetActive(false);
    }
    IEnumerator Destination(GameObject obj, string typeText, int type)
    {
        yield return new WaitForSeconds(1f);
        StartCoroutine(Typing(obj,typeText, type));
    }
    
}
