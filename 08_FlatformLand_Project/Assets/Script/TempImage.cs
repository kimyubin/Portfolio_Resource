using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TempImage : MonoBehaviour
{
    public GameObject[] image;
    int x,y;
    // Start is called before the first frame update
    void Start()
    {
        image[0].SetActive(true);
        image[1].SetActive(false);
        image[2].SetActive(false);
        image[3].SetActive(false);
        image[4].SetActive(false);
        
    }

    // Update is called once per frame
    void Update()
    {
        SetTempImage();
    }
    void SetTempImage()
    {   
        //위치
        transform.position = GridSys.GetRealPos(GridSys.GetMousePos());

        //표시될 이미지.
        image[0].SetActive(false);
        image[1].SetActive(false);
        image[2].SetActive(false);
        image[3].SetActive(false);
        image[4].SetActive(false);
        

        image[GridSys.GetBlockNum()].SetActive(true);  
    }
}
