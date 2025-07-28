using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovePlatformScript : MonoBehaviour
{
    float speed;
    float rightLimit = 1f;//오른쪽 한계값
    bool end; // 상하 좌우 끝에 도달하면 바뀜. 없으면 한쪽 끝으로 갔다가 반대 방향으로 돌아가지 못하고 좌우로 빠르게 이동함.

    Vector3 initPos;
    // Start is called before the first frame update
    void Start()
    {
        initPos = transform.position;
        speed = 0.7f;
        end = true;
    }

    // Update is called once per frame
    void Update()
    {
        if (end)
        {
            if (transform.position.x < (initPos.x + rightLimit))//현재 위치가 초기x값 + limit보다 작으면 오른쪽으로
            {
                gameObject.transform.Translate(Vector3.right * Time.deltaTime * speed);
            }
            else
            {
                end = false;
            }
        }
        else
        {
            if (transform.position.x > initPos.x)//현재 위치가 초기x값 + limit보다 작으면 오른쪽으로
            {
                gameObject.transform.Translate(Vector3.left * Time.deltaTime * speed);
            }
            else
            {
                end = true;
            }
        }
    }
}
