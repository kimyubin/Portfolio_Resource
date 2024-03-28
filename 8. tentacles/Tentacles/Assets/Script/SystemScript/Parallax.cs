using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Parallax : MonoBehaviour
{
    public Transform[] bGObject;//배경 오브젝트 배열
    public float moveRefSpeed = 1f; // 움직이는 속도 보정값.
    public Component targetObj; // 쫓아서 움직이는 목표. 카메라 or 플레이어캐릭터
    Transform targetTrans;

    float[] paraScale; // 배경 오브젝트 Z값
    int bgObjNum; // 배경 오브젝트 갯수
    float xTargetPosX; // 이전 프레임 타겟 위치.
    int num;

    void Awake()
    {
    }
    // Start is called before the first frame update
    void Start()
    {
        targetTrans = targetObj.transform;

        bgObjNum = bGObject.Length;
        xTargetPosX = targetTrans.position.x;
        paraScale = new float[bgObjNum];
        for(int i = 0; i < bgObjNum; i++)
        {
            paraScale[i] = bGObject[i].position.z * (-1);
            //speed에 z값을 곱했을때 절대값 32에서 정지화면으로 보여지며, 32를 초과할때 그 속도가 플레이어 이동속도를 넘어섬.
            //speed가 1일때, z값이 32를 넘어가면 플레이어 이동속도 보다 빨라지고, 32에서 정지함. 최댓값 제한. 
            if(paraScale[i] * moveRefSpeed <= -59f)
                paraScale[i] = -59f/moveRefSpeed; 
            
        }
        
    }

    // Update is called once per frame
    void Update()
    {
        for(int i = 0; i< bgObjNum ; i++)
        {
            // 타겟 오브젝트가 한프레임 동안 움직인 x값에 배경 오브젝트 z값(=정렬 순위)를 곱한 값만큼 배경 오브젝트가 이동. 
            //z 절대값이 클수록 = 멀리 있을 수록 많이 움직임. 많이 움직인다 = 카메라 시점이 이동해도 적게 움직임. 
            float backgroundTargetPosX = bGObject[i].position.x + ((xTargetPosX - targetTrans.position.x) * paraScale[i]);
            
            Vector3 backgroundTargetPos = new Vector3 (backgroundTargetPosX, bGObject[i].position.y, bGObject[i].position.z);


            //선형보간. 1프레임에 moveRefSpeed * Time.deltaTime(1이하, %) 만큼 이동. 빠르게 움직이다가 천천히 움직임. speed값이 커질수록 크게 움직임.
            bGObject[i].position = Vector3.Lerp(bGObject[i].position, backgroundTargetPos, moveRefSpeed * Time.deltaTime);

        }
        xTargetPosX = targetTrans.position.x;
    }
}
