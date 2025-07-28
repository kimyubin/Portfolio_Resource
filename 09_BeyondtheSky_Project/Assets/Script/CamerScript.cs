using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CamerScript : MonoBehaviour
{
    static bool mute;
    void Awake()
    {
        Camera cam = GetComponent<Camera>();
        Rect rect = cam.rect;
        float scaleHeight = ((float)Screen.width/(float)Screen.height) / ((float)9/(float)16);
        float scaleWidth = 1f/scaleHeight;
          if (scaleHeight < 1)
        {
            rect.height = scaleHeight;
            rect.y = (1f - scaleHeight) / 2f;
        }
        else
        {
            rect.width = scaleWidth;
            rect.x = (1f - scaleWidth) / 2f;
        }
        cam.rect = rect;
    }
  
    void Update()
    {
        if(gameObject.transform.position.y >= 169f)//최대 이동 높이 제한
        {
            transform.position = new Vector3(transform.position.x, 169f, transform.position.z);
        }

    }
    void OnPreCull()
    {
        GL.Clear(true, true, Color.black);
    }
    
}