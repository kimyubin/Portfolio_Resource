using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TentacleJumpNEW : MonoBehaviour
{
    SpriteRenderer spriteRend;
    Quaternion spriteAngle;
    

    // Start is called before the first frame update
    void Start()
    {
        spriteRend = GetComponentInChildren<SpriteRenderer>();
        //spriteRend.gameObject.transform.rotation = Quaternion.AngleAxis(30, Vector3.forward);

    }

    // Update is called once per frame
    void Update()
    {
        
        //spriteRend.gameObject.transform.rotation = Quaternion.AngleAxis(-angle, Vector3.forward); // 스프라이트 각도 수정

        //if(Input.GetButton("TentacleJump"))
        {
            //if(Input.GetKey(KeyCode.UpArrow))
            {
                while (spriteRend.gameObject.transform.rotation.z != 0f)
                {
                    spriteRend.gameObject.transform.rotation = Quaternion.AngleAxis(30, Vector3.forward);
                }

            }
            //else if(Input.GetKey(KeyCode.DownArrow))
            {

            }
            //else if(Input.GetKey(KeyCode.RightArrow))
            {

            }
            //else if(Input.GetKey(KeyCode.LeftArrow))
            {

            }

        }
    }
}
